/*
 * KTEgg.cc
 *
 *  Created on: Sep 9, 2011
 *      Author: nsoblath
 */

#include "KTRSAMatReader.hh"

#include "KTEggHeader.hh"
#include "KTEggProcessor.hh"
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

    KT_REGISTER_EGGREADER(KTRSAMatReader, "rsamat");

    KTRSAMatReader::KTRSAMatReader() :
            KTEggReader(),
            fSliceSize(1024),
            fStride(0),
            fHeaderPtr(new KTData()),
            fHeader(fHeaderPtr->Of< KTEggHeader >()),
            fSampleRateUnitsInHz(1.e6),
            fRecordSize(1),
            fBinWidth(0.),
            fSliceNumber(0),
            fRecordsRead(0),
            fSamplesRead(0),
            fSamplesPerFile(0),
            fRecordsPerFile(1),
            fRecordsTimeStampSeconds(NULL),
            fTSArrayMat(NULL),
            fMatFilePtr(NULL)
    {
    }

    KTRSAMatReader::~KTRSAMatReader()
    {
        if (fRecordsTimeStampSeconds != NULL) delete [] fRecordsTimeStampSeconds;
    }

    unsigned KTRSAMatReader::GetMaxChannels()
    {
        return fMaxChannels;
    }

    bool KTRSAMatReader::Configure(const KTEggProcessor& eggProc)
    {
        SetSliceSize(eggProc.GetSliceSize());
        SetStride(eggProc.GetStride());
        return true;
    }

    KTDataPtr KTRSAMatReader::BreakEgg(const string& filename)
    {
        // Temporary variable to read time stamps
        boost::posix_time::ptime ptime1temp, ptime1temp_1st; // From Boost
        boost::posix_time::time_duration tdur1temp; // From Boost

        if (fStride == 0) fStride = fSliceSize;
        if (fStride != fSliceSize)
        {
            KTERROR(eggreadlog, "The RSA matlab reader requires the stride equal the slice size");
            return KTDataPtr();
        }

        // open the file
        KTINFO(eggreadlog, "Opening mat file <" << filename << ">");
        fMatFilePtr = Mat_Open(filename.c_str(), MAT_ACC_RDONLY);
        if (fMatFilePtr == NULL)
        {
            KTERROR(eggreadlog, "Unable to open mat file: " << filename);
            return KTDataPtr();
        }

        matvar_t* data = NULL;
        while ( (data = Mat_VarReadNextInfo(fMatFilePtr)) != NULL )
        {
            std::cout << "data in file: " << data->name << std::endl;;
            Mat_VarFree(data);
            data = NULL;
        }

        double timeFromFirstToLastRecord = 0.;

        // Check if the file has the variable "fileinfo", and if it has more than 1 entry;
        //  -> this variable contains the info on individual files when hey are concatenated;
        //  -> If there more than one entry, then it's a concatenated file, and we have 1 fileinfo
        //     per original file
        matvar_t* fileInfoStruct = Mat_VarRead(fMatFilePtr, "fileinfo");
        if (fileInfoStruct == NULL)
        {
            KTINFO(eggreadlog, "No fileinfo variable in file, therefore this is probably an original MAT file");
            // If fileInfoStruct doesn't exist or is not a structure, then it's an original MAT file
            fRecordsPerFile = 1;
            timeFromFirstToLastRecord = 0;
            if (fRecordsTimeStampSeconds != NULL) delete [] fRecordsTimeStampSeconds;
            fRecordsTimeStampSeconds = new double[1]; //(double *) calloc(1, sizeof(double));
            fRecordsTimeStampSeconds[0] = 0;
        }
        else
        {
            // If fileInfoStruct exists, then this is a concatenated file

            // Get the number of records (that is, original mat files),
            //  then create an array to save the timestamps
            std::cout << "file info class type: " << fileInfoStruct->class_type << std::endl;
            std::cout << "file info data type: " << fileInfoStruct->data_type << std::endl;
            /*
            fRecordsPerFile = mxGetNumberOfElements(fileInfoStruct);
            KTINFO(eggreadlog, "Number of Records in File: fRecordsPerFile = " << fRecordsPerFile << " ");
            if (fRecordsTimeStampSeconds != NULL) delete [] fRecordsTimeStampSeconds;
            fRecordsTimeStampSeconds = new double[fRecordsPerFile]; //(double *) calloc(fRecordsPerFile, sizeof(double));
            // Read the timestamps into a string, then convert the string to Epoch seconds
            mxArray* rsaxml_mat = mxGetField(fileInfoStruct, 0, "rsaMetadata");
            if (rsaxml_mat == NULL)
            {
                KTERROR(eggreadlog, "Unable to read RSA XML config from MAT file");
                return KTDataPtr();
            }
            for (unsigned ii = 0; ii < fRecordsPerFile; ++ii)
            {
                // Read XML Configuration for this Record (original MAT file)
                rsaxml_mat = mxGetField(fileInfoStruct, ii, "rsaMetadata");
                if (rsaxml_mat == NULL)
                {
                    KTERROR(eggreadlog, "Unable to read RSA XML config from fileInfoStruct (ii=" << ii << ")");
                    return KTDataPtr();
                }
                int buflen = mxGetN(rsaxml_mat) + 1;
                char* rsaxml_str = new char[buflen];
                mxGetString(rsaxml_mat, rsaxml_str, buflen);
                // For debugging // KTINFO(eggreadlog, "rsaxml_str: " << rsaxml_str << " \n ii = " << ii );
                // Parse XML
                rapidxml::xml_document< > doc;
                doc.parse< 0 >(rsaxml_str);
                rapidxml::xml_node< >* data_node = doc.first_node("DataFile")->first_node("DataSetsCollection")->first_node("DataSets")->first_node("DataDescription");
                rapidxml::xml_node< >* curr_node = data_node->first_node("DateTime");
                char* recordsTimeStampStr = new char[curr_node->value_size()]; //(char*) calloc(curr_node->value_size(), sizeof(char));
                strncpy(recordsTimeStampStr, curr_node->value(), curr_node->value_size() - 6);
                strncpy(&recordsTimeStampStr[10], " ", 1);
                // For Debugging: // cout << "DateTime: " << recordsTimeStampStr << "\n";
                // Convert from String to Epoch Seconds
                ptime1temp = boost::posix_time::time_from_string(recordsTimeStampStr);
                if (ii == 0) ptime1temp_1st = ptime1temp;
                tdur1temp = ptime1temp - ptime1temp_1st;
                fRecordsTimeStampSeconds[ii] = ((double) tdur1temp.total_nanoseconds()) * SEC_PER_NSEC;
                // For Debugging: // fRecordsTimeStampSeconds[ii] = 0;
                delete [] recordsTimeStampStr;
                delete [] rsaxml_str;
            }
            timeFromFirstToLastRecord = fRecordsTimeStampSeconds[fRecordsPerFile - 1] - fRecordsTimeStampSeconds[0];
            // For Debugging:
            // fRecordsPerFile = 1;
            // timeFromFirstToLastRecord = 0;
            // fRecordsTimeStampSeconds = (double *) calloc(1, sizeof(fRecordsTimeStampSeconds));
            // fRecordsTimeStampSeconds[0] = 0;
            mxDestroyArray(rsaxml_mat);
            */
        }

#if 0
        { // For Debugging
            KTINFO(eggreadlog, "Number of Records in File: fRecordsPerFile = " << fRecordsPerFile << " ");
            KTERROR(eggreadlog, "Done for now, we are debugging");
            return KTDataPtr();
        }
#endif

        // Get the pointer to the data array
        fTSArrayMat = Mat_VarRead(fMatFilePtr, "Y");
        if (fTSArrayMat == NULL)
        {
            KTERROR(eggreadlog, "Unable to find variable \"Y\"");
            Mat_Close(fMatFilePtr);
            fMatFilePtr = NULL;
            return KTDataPtr();
        }




        // Read XML Configuration
        matvar_t* rsaxml_mat = Mat_VarRead(fMatFilePtr, "rsaMetadata");
        if (rsaxml_mat == NULL)
        {
            KTERROR(eggreadlog, "Unable to read RSA XML config from MAT file");
            Mat_VarFree(fTSArrayMat);
            Mat_Close(fMatFilePtr);
            fMatFilePtr = NULL;
            return KTDataPtr();
        }
        if (rsaxml_mat->class_type != MAT_C_CHAR || rsaxml_mat->data_type != MAT_T_UINT8)
        {
            KTERROR(eggreadlog, "I don't know how to read a header with class type <" << rsaxml_mat->class_type << "> and data type <" << rsaxml_mat->data_type << ">");
            Mat_VarFree(fTSArrayMat);
            Mat_VarFree(rsaxml_mat);
            Mat_Close(fMatFilePtr);
            fMatFilePtr = NULL;
            return KTDataPtr();
        }
        //Mat_VarPrint(rsaxml_mat, 1);
        size_t buflen = rsaxml_mat->dims[1];  //mxGetN(rsaxml_mat) + 1;
        char* rsaxml_str = new char [buflen]; //(char*) calloc(buflen, sizeof(char));
        memcpy(rsaxml_str, rsaxml_mat->data, buflen);
        //int status = mxGetString(rsaxml_mat, rsaxml_str, buflen);
        //if (status != 0)
        //{
        //    KTERROR(eggreadlog, "Unable to read XML Configuration string.");
        //    return KTDataPtr();
        //}
        KTINFO(eggreadlog, "Read XML Run Configuration");
        //std::cout << "xml string: \n" << rsaxml_str << std::endl;

        // Parse XML
        rapidxml::xml_document< > doc;
        doc.parse< 0 >(rsaxml_str);
        rapidxml::xml_node< >* data_node = doc.first_node("DataFile")->first_node("DataSetsCollection")->first_node("DataSets")->first_node("DataDescription");
        rapidxml::xml_node< >* curr_node;
        // For debugging:
        //curr_node = data_node->first_node("SamplingFrequency");
        //cout << "Name of my current node is: " << curr_node->name() << "\n";
        //printf("Sampling Frequency: %s\n", curr_node->value());

        // Write configuration from XML into fHeader variable
        fHeader.SetTSDataType(KTEggHeader::kIQ);
        fHeader.SetFilename(filename);
        fHeader.SetNChannels(1);
        curr_node = data_node->first_node("NumberSamples");
        fHeader.SetRecordSize((size_t) atoi(curr_node->value()));
        //rapidxml::xml_node< >* prod_spec_node = doc.first_node("DataFile")->first_node("DataSetsCollection")->first_node("DataSets")->first_node("ProductSpecific");
        // The RSA samples at 100 MHz, then decimates by 2 (50 MHz); the number reported by the RSA is "SamplingFrequency" = 50 MHz
        // So the frequency of the samples in the data is 50 MHz, and since this is IQ data, the Bandwidth should also be 50 MHz.
        curr_node = data_node->first_node("SamplingFrequency");
        double fAcqBW = atof(curr_node->value());
        fHeader.SetAcquisitionRate(fAcqBW);
        fHeader.SetRunDuration(timeFromFirstToLastRecord + (double) fHeader.GetRecordSize() / fHeader.GetAcquisitionRate());
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
        Mat_VarFree(rsaxml_mat);
        rsaxml_mat = NULL;
        delete [] rsaxml_str;
        rsaxml_str = NULL;

        // Get configuration from JSON config file
        fHeader.SetRawSliceSize(fSliceSize);
        fHeader.SetSliceSize(fSliceSize);
        fHeader.SetSliceStride(fStride);

        Mat_VarPrint(fTSArrayMat, 1);
        // A few last useful variables
        fRecordSize = fHeader.GetRecordSize();
        fBinWidth = 1. / fHeader.GetAcquisitionRate();
        fSliceNumber = 0; // Number of Slices saved
        fRecordsRead = 0; // Number of records read from file
        fSamplesRead = 0; // Number of samples read from file (not from record)
        fSamplesPerFile = fTSArrayMat->dims[0];

        // Read center frequency, and derive minimum and maximum frequencies 
        // from the span.
        // Note: On second thought, you need to be very careful here.  The RSA
        // tells you the center frequency __of the span__.  So the Minimum 
        // frequency should be the center frequency - span/2, and maximum 
        // frequency should be minimum frequency + fAcqBW.  
        matvar_t* cfArray = Mat_VarRead(fMatFilePtr, "InputCenter");
        matvar_t* spanArray = Mat_VarRead(fMatFilePtr, "Span");
        if (cfArray == NULL || spanArray == NULL)
        {
            KTERROR(eggreadlog, "Could not find either the center frequency (" << cfArray << ") or the span ("
                    << spanArray << ") in the MAT file");
            Mat_VarFree(fTSArrayMat);
            if (cfArray != NULL) Mat_VarFree(cfArray);
            if (spanArray != NULL) Mat_VarFree(spanArray);
            Mat_Close(fMatFilePtr);
            fMatFilePtr = NULL;
            return KTDataPtr();
        }
        //Mat_VarPrint(cfArray, 1);
        //Mat_VarPrint(spanArray, 1);

        double spanCenterFreq = ((double*)cfArray->data)[0];
        double span = ((double*)spanArray->data)[0];

        fHeader.SetCenterFrequency(spanCenterFreq);
        fHeader.SetMinimumFrequency(spanCenterFreq - fAcqBW/2.0);
        fHeader.SetMaximumFrequency(spanCenterFreq + fAcqBW/2.0);

        // Log the contents of the header
        KTDEBUG(eggreadlog, "Parsed header:\n" << fHeader);
        return fHeaderPtr;
    }
    KTDataPtr KTRSAMatReader::HatchNextSlice()
    {
        /*
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
        sliceHeader.SetTimeInAcq(GetTimeInAcq());
        sliceHeader.SetSliceNumber(fSliceNumber);
        sliceHeader.SetStartRecordNumber(fRecordsRead);
        sliceHeader.SetStartSampleNumber(fSamplesRead);
        sliceHeader.SetRecordSize(fHeader.GetRecordSize());
        // Slice Header Variables that depend on channel number
        unsigned iChannel = 0;
        sliceHeader.SetAcquisitionID(fRecordsRead, iChannel);
        sliceHeader.SetRecordID(fRecordsRead, iChannel);
        sliceHeader.SetTimeStamp(sliceHeader.GetTimeInRun() / SEC_PER_NSEC, iChannel);

        // ********************************** //
        // Read data                          //
        // ********************************** //
        KTTimeSeriesFFTW* newSliceComplex = new KTTimeSeriesFFTW(sliceHeader.GetSliceSize(), 0., double(sliceHeader.GetSliceSize()) * sliceHeader.GetBinWidth());
        float *data_real_ptr;
        float *data_imag_ptr;

        data_real_ptr = (float *) mxGetData(fTSArrayMat);
        data_imag_ptr = (float *) mxGetImagData(fTSArrayMat);
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

        KTDEBUG(eggreadlog, sliceHeader << "\nNote: some fields may not be filled in correctly yet");

        return newData;
*/
        return KTDataPtr();
    }

    bool KTRSAMatReader::CloseEgg()
    {
        if (fRecordsTimeStampSeconds != NULL) delete [] fRecordsTimeStampSeconds;
        fRecordsTimeStampSeconds = NULL;

        /* clean matlab variable before exit */
        if (fTSArrayMat != NULL) Mat_VarFree(fTSArrayMat);

        // Close matlab file
        if (fMatFilePtr == NULL)
        {
            KTWARN(eggreadlog, "Mat file is not open");
            return true;
        }
        if (Mat_Close(fMatFilePtr) != 0)
        {
            KTERROR(eggreadlog, "Something went wrong while closing the mat file");
            return false;
        }

        return true;
    }

} /* namespace Katydid */

