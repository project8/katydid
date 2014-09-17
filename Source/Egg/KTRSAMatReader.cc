/*
 * KTEgg.cc
 *
 *  Created on: Sep 9, 2011
 *      Author: nsoblath
 */

#include "KTRSAMatReader.hh"
#include "KTLogger.hh"
#include "KTSliceHeader.hh"
#include "KTTimeSeries.hh"
#include "KTTimeSeriesData.hh"
#include "KTTimeSeriesFFTW.hh"
#include "rapidxml.hpp"
#include <cstring>
#include "boost/date_time/posix_time/posix_time.hpp"

using namespace std;

using std::map;
using std::string;
using std::vector;

namespace Katydid
{
    KTLOGGER(eggreadlog, "KTRSAMatReader");

    KTRSAMatReader::KTRSAMatReader() :
            KTEggReader(),
            fSliceSize(1024),
            fStride(0),
            fHeader(),
            fSampleRateUnitsInHz(1.e6),
            fBinWidth(0.),
            fSliceNumber(0),
            fSamplesRead(0),
            fSamplesPerFile(0),
            fRecordsPerFile(1),
            fRecordsRead(0)
    {
    }

    KTRSAMatReader::~KTRSAMatReader()
    {
    }

    unsigned KTRSAMatReader::GetMaxChannels()
    {
        return fMaxChannels;
    }

    KTEggHeader* KTRSAMatReader::BreakEgg(const string& filename)
    {
        mxArray *dt_mat, *fc_mat, *bw_mat, *rsaxml_mat, *fileinfostruct;
        char *rsaxml_str;
        int buflen;
        int status;
        rapidxml::xml_document< > doc;
        rapidxml::xml_node< > * data_node;
        rapidxml::xml_node< > * curr_node;
        fSliceSize = GetSliceSize();
        fStride = GetStride();
        // Temporary variable to read time stamps
        double TimeFromFirstToLastRecord;
        char *RecordsTimeStampStr;
        boost::posix_time::ptime ptime1temp, ptime1temp_1st; // From Boost
        boost::posix_time::time_duration tdur1temp; // From Boost

        if (fStride == 0) fStride = fSliceSize;

        // open the file
        KTINFO(eggreadlog, "Opening mat file <" << filename << ">");
        matfilep = matOpen(filename.c_str(), "r");
        if (matfilep == NULL)
        {
            KTERROR(eggreadlog, "Unable to open mat file: " << filename);
            return NULL;
        }

        // Get the pointer to the data array
        ts_array_mat = matGetVariable(matfilep, "Y");

        // Check if the file has the variable "fileinfo", and if it has more than 1 entry;
        //  -> this variable contains the info on individual files when hey are concatenated;
        //  -> If there more than one entry, then it's a concatenated file, and we have 1 fileinfo
        //     per original file
        fileinfostruct = matGetVariable(matfilep, "fileinfo");
        if (fileinfostruct == NULL)
        {
            KTINFO(eggreadlog, "No fileinfo variable in file - this is not a proper Concatenatd MAT file");
        }

        if (fileinfostruct != NULL)
        {
            // If fileinfostruct exists, then this is a concatenated file

            // Get the number of records (that is, original mat files),
            //  then create an array to save the timestamps
            fRecordsPerFile = mxGetNumberOfElements(fileinfostruct);
            KTINFO(eggreadlog, "Number of Records in File: fRecordsPerFile = " << fRecordsPerFile << " ");
            RecordsTimeStampSeconds = (double *) calloc(fRecordsPerFile, sizeof(double));
            // Read the timestamps into a string, then convert the string to Epoch seconds
            rsaxml_mat = mxGetField(fileinfostruct, 0, "rsaMetadata");
            if (rsaxml_mat == NULL)
            {
                KTERROR(eggreadlog, "Unable to read RSA XML config from MAT file");
                return NULL;
            }
            for (int ii = 0; ii < fRecordsPerFile; ii++)
            {
                // Read XML Configuration for this Record (original MAT file)
                rsaxml_mat = mxGetField(fileinfostruct, ii, "rsaMetadata");
                buflen = mxGetN(rsaxml_mat) + 1;
                rsaxml_str = (char*) calloc(buflen, sizeof(char));
                status = mxGetString(rsaxml_mat, rsaxml_str, buflen);
                // For debugging // KTINFO(eggreadlog, "rsaxml_str: " << rsaxml_str << " \n ii = " << ii );
                // Parse XML
                doc.parse< 0 >(rsaxml_str);
                data_node = doc.first_node("DataFile")->first_node("DataSetsCollection")->first_node("DataSets")->first_node("DataDescription");
                curr_node = data_node->first_node("DateTime");
                RecordsTimeStampStr = (char*) calloc(curr_node->value_size(), sizeof(char));
                strncpy(RecordsTimeStampStr, curr_node->value(), curr_node->value_size() - 6);
                strncpy(&RecordsTimeStampStr[10], " ", 1);
                // For Debugging: // cout << "DateTime: " << RecordsTimeStampStr << "\n";
                // Convert from String to Epoch Seconds
                ptime1temp = boost::posix_time::time_from_string(RecordsTimeStampStr);
                if (ii == 0) ptime1temp_1st = ptime1temp;
                tdur1temp = ptime1temp - ptime1temp_1st;
                RecordsTimeStampSeconds[ii] = ((double) tdur1temp.total_nanoseconds()) / 1e9;
                // For Debugging: // RecordsTimeStampSeconds[ii] = 0;
            }
            TimeFromFirstToLastRecord = RecordsTimeStampSeconds[fRecordsPerFile - 1] - RecordsTimeStampSeconds[0];
            // For Debugging:
            // fRecordsPerFile = 1;
            // TimeFromFirstToLastRecord = 0;
            // RecordsTimeStampSeconds = (double *) calloc(1, sizeof(RecordsTimeStampSeconds));
            // RecordsTimeStampSeconds[0] = 0;

        }
        else
        {
            // If fileinfostruct doesn't exist or is not a structure, then it's an original MAT file
            fRecordsPerFile = 1;
            TimeFromFirstToLastRecord = 0;
            RecordsTimeStampSeconds = (double *) calloc(1, sizeof(RecordsTimeStampSeconds));
            RecordsTimeStampSeconds[0] = 0;
        }

        if (1 == 0)
        { // For Debugging
            KTINFO(eggreadlog, "Number of Records in File: fRecordsPerFile = " << fRecordsPerFile << " ");
            KTERROR(eggreadlog, "Done for now, we are debugging");
            return NULL;
        }

        // Read XML Configuration
        rsaxml_mat = matGetVariable(matfilep, "rsaMetadata");
        if (rsaxml_mat == NULL)
        {
            KTERROR(eggreadlog, "Unable to read RSA XML config from MAT file");
            return NULL;
        }
        buflen = mxGetN(rsaxml_mat) + 1;
        rsaxml_str = (char*) calloc(buflen, sizeof(char));
        status = mxGetString(rsaxml_mat, rsaxml_str, buflen);
        if (status != 0)
        {
            KTERROR(eggreadlog, "Unable to read XML Configuration string.");
            return NULL;
        }
        KTINFO(eggreadlog, "Read XML Run Configuration");

        // Parse XML
        doc.parse< 0 >(rsaxml_str);
        data_node = doc.first_node("DataFile")->first_node("DataSetsCollection")->first_node("DataSets")->first_node("DataDescription");
        curr_node = data_node->first_node("SamplingFrequency");
        // For debugging:
        //cout << "Name of my current node is: " << curr_node->name() << "\n";
        //printf("Sampling Frequency: %s\n", curr_node->value());

        // Write configuration from XML into fHeader variable
        fHeader.SetFilename(filename);
        fHeader.SetNChannels(1);
        curr_node = data_node->first_node("NumberSamples");
        fHeader.SetRecordSize((size_t) atoi(curr_node->value()));
        curr_node = data_node->first_node("AcquisitionBandwidth");
        fHeader.SetAcquisitionRate(2. * atof(curr_node->value()));
        fHeader.SetRunDuration(TimeFromFirstToLastRecord + (double) fHeader.GetRecordSize() / fHeader.GetAcquisitionRate());
        curr_node = data_node->first_node("DateTime");
        fHeader.SetTimestamp(curr_node->value());
        curr_node = data_node->first_node("NumberFormat");
        if (strcmp(curr_node->value(), "Int32") == 0)
        {
            fHeader.SetDataTypeSize(sizeof(int32_t));
        }
        // The variables below could not be obtained from the XML configuration:
        //fHeader.SetDescription(monarchHeader->GetDescription());
        //fHeader.SetRunType(monarchHeader->GetRunType());
        //fHeader.SetRunSource(monarchHeader->GetRunSource());
        //fHeader.SetFormatMode(monarchHeader->GetFormatMode());
        //fHeader.SetBitDepth(monarchHeader->GetBitDepth());
        //fHeader.SetVoltageMin(monarchHeader->GetVoltageMin());
        //fHeader.SetVoltageRange(monarchHeader->GetVoltageRange());

        // Close the XML variable
        mxDestroyArray(rsaxml_mat);

        // Get configuration from JSON config file
        fHeader.SetRawSliceSize(fSliceSize);
        fHeader.SetSliceSize(fSliceSize);
        fHeader.SetSliceStride(fStride);

        // Log the contents of the header
        stringstream headerBuff;
        headerBuff << fHeader;
        KTDEBUG(eggreadlog, "Parsed header:\n" << headerBuff.str());

        // A few last useful variables

        fRecordSize = fHeader.GetRecordSize();
        fBinWidth = 1. / fHeader.GetAcquisitionRate();
        fSliceNumber = 0; // Number of Slices saved
        fRecordsRead = 0; // Number of records read from file
        fSamplesRead = 0; // Number of samples read from file (not from record)
        fSamplesPerFile = (unsigned) mxGetNumberOfElements(ts_array_mat);

        return new KTEggHeader(fHeader);
    }
    KTDataPtr KTRSAMatReader::HatchNextSlice()
    {

        // IMPORTANT:
        // Updated: KTRSAMatReader::HatchNextSlice is currently capable of reading MAT files containing multiple records, *as long as they have
        //          the same size* - that is, MAT files of identical sizes that were concatenated;
        //          it still cannot read multiple channels

        // Initialize output
        KTDataPtr newData(new KTData());

        // ********************************************************* //
        // Check whether we still have enough data to fill up slice  //
        // ********************************************************* //
        // fSamplesRead is used as a pointer to where we are in the file;
        // It gets incremented by the fSliceSize at the end of reading a slice;
        // So here we must:
        // (1) test if there is still enough data in the file to fill the slice,
        //     and if not, exit;
        // (2) test if there is still enough data in the record to fill the slice,
        //     and if not, increment it to the start of the next record.

        // Check if you still have enough data in the file
        if (fSamplesRead + fSliceSize > fSamplesPerFile)
        {
            KTWARN(eggreadlog, "End of mat file reached");
            KTDEBUG(eggreadlog, "fSamplesRead: " << fSamplesRead << "; fSliceSize: " << fSliceSize << "; fRecordSize: " << fRecordSize << "; fSamplesPerFile: " << fSamplesPerFile);

            // Return Empty Pointer
            return KTDataPtr();
        }

        // Create Slice Header
        KTSliceHeader& sliceHeader = newData->Of< KTSliceHeader >().SetNComponents(fHeader.GetNChannels());
        sliceHeader.SetIsNewAcquisition(false);

        // If this slice if going to cross over a record boundary, then 
        //  increment the record number, set slice to 0, move fSamplesRead
        //  to the start of the next record, and return empty data
        //  (you don't want a slice with data from 2 records...)
        if ((fSamplesRead - fRecordsRead * fRecordSize + fSliceSize) > fRecordSize)
        {
            KTINFO(eggreadlog, "End of Record reached");
            KTDEBUG(eggreadlog, "fRecordsRead: " << fRecordsRead << "; fSamplesRead: " << fSamplesRead << "; fSliceSize: " << fSliceSize << "; fRecordSize: " << fRecordSize << "; fSamplesPerFile: " << fSamplesPerFile);

            ++fRecordsRead;
            fSamplesRead = fRecordsRead * fRecordSize;
            sliceHeader.SetIsNewAcquisition(true);
        }

        // ********************************** //
        // Fill out slice header information  //
        // ********************************** //
        if (fSliceNumber == 0) sliceHeader.SetIsNewAcquisition(true);

        // Slice Header Variables
        sliceHeader.SetSampleRate(fHeader.GetAcquisitionRate());
        sliceHeader.SetRawSliceSize(fSliceSize);
        sliceHeader.SetSliceSize(fSliceSize);
        sliceHeader.CalculateBinWidthAndSliceLength();
        sliceHeader.SetNonOverlapFrac((double) fStride / (double) fSliceSize);
        sliceHeader.SetTimeInRun(GetTimeInRun());
        sliceHeader.SetSliceNumber(fSliceNumber);
        sliceHeader.SetStartRecordNumber(fRecordsRead);
        sliceHeader.SetStartSampleNumber(fSamplesRead);
        sliceHeader.SetRecordSize(fHeader.GetRecordSize());
        // Slice Header Variables that depend on channel number
        unsigned iChannel = 0;
        sliceHeader.SetAcquisitionID(fRecordsRead, iChannel);
        sliceHeader.SetRecordID(fRecordsRead, iChannel);
        sliceHeader.SetTimeStamp(sliceHeader.GetTimeInRun() / SEC_PER_NSEC, iChannel);
        KTDEBUG(eggreadlog, sliceHeader << "\nNote: some fields may not be filled in correctly yet");

        // ********************************** //
        // Read data                          //
        // ********************************** //
        KTTimeSeriesFFTW* newSliceComplex = new KTTimeSeriesFFTW(sliceHeader.GetSliceSize(), 0., double(sliceHeader.GetSliceSize()) * sliceHeader.GetBinWidth());
        float *data_real_ptr;
        float *data_imag_ptr;

        data_real_ptr = (float *) mxGetData(ts_array_mat);
        data_imag_ptr = (float *) mxGetImagData(ts_array_mat);
        for (unsigned iBin = 0; iBin < fSliceSize; iBin++)
        {
            (*newSliceComplex)(iBin)[0] = double(data_real_ptr[iBin + fSamplesRead]);
            (*newSliceComplex)(iBin)[1] = double(data_imag_ptr[iBin + fSamplesRead]);
        }
        KTTimeSeries* newSlice = newSliceComplex;
        fSamplesRead = fSamplesRead + fSliceSize;
        ++fSliceNumber;
        KTTimeSeriesData& tsData = newData->Of< KTTimeSeriesData >().SetNComponents(1);
        tsData.SetTimeSeries(newSlice);
        sliceHeader.SetEndRecordNumber(fRecordsRead);
        sliceHeader.SetEndSampleNumber(fSamplesRead);

        return newData;

    }

    bool KTRSAMatReader::CloseEgg()
    {
        /* clean matlab variable before exit */
        mxDestroyArray(ts_array_mat);

        // Close matlab file
        if (matClose(matfilep) != 0)
        {
            KTERROR(eggreadlog, "Something went wrong while closing the mat file");
            return false;
        }

        return true;
    }

} /* namespace Katydid */

