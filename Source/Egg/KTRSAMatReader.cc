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
        mxArray *dt_mat, *fc_mat, *bw_mat, *rsaxml_mat;
        char *rsaxml_str;
        int   buflen;
        int   status;
        rapidxml::xml_document<> doc;
        fSliceSize = GetSliceSize();
        fStride = GetStride();


        if (fStride == 0) fStride = fSliceSize;

        // open the file
        KTINFO(eggreadlog, "Opening egg file <" << filename << ">");
        matfilep = matOpen(filename.c_str(), "r");
        if (matfilep == NULL) {
            KTERROR(eggreadlog, "Unable to break egg: " << filename);
            return NULL;
        }

        // Read XML Configuration
        rsaxml_mat = matGetVariable(matfilep, "rsaMetadata");
        buflen = mxGetN(rsaxml_mat)+1;
        rsaxml_str = (char*)malloc(buflen * sizeof(char));
        status = mxGetString(rsaxml_mat, rsaxml_str, buflen);
        if(status != 0) {
            KTERROR(eggreadlog, "Unable to read XML Configuration string.");
            return NULL;
        }

        // Parse XML
        doc.parse<0>(rsaxml_str);
        rapidxml::xml_node<> * data_node;
        rapidxml::xml_node<> * curr_node;
        data_node = doc.first_node("DataFile")->first_node("DataSetsCollection")->first_node("DataSets")->first_node("DataDescription");
        curr_node = data_node->first_node("SamplingFrequency");
        // For debugging:
        //cout << "Name of my current node is: " << curr_node->name() << "\n";
        //printf("Sampling Frequency: %s\n", curr_node->value());

        // Write configuration from XML into fHeader variable
        fHeader.SetFilename(filename);
        //fHeader.SetAcquisitionMode(monarchHeader->GetAcquisitionMode());
        fHeader.SetNChannels(1);
        curr_node = data_node->first_node("NumberSamples");
        fHeader.SetRecordSize((size_t) atoi(curr_node->value()));
        curr_node = data_node->first_node("SamplingFrequency");
        fHeader.SetAcquisitionRate(atof(curr_node->value()));
        fHeader.SetRunDuration( (double) fHeader.GetRecordSize() / fHeader.GetAcquisitionRate());
        curr_node = data_node->first_node("DateTime");
        fHeader.SetTimestamp(curr_node->value());
        curr_node = data_node->first_node("NumberFormat");
        if (strcmp(curr_node->value() , "Int32") == 0) {
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

        // Log the contents of the header
        stringstream headerBuff;
        headerBuff << fHeader;
        KTDEBUG(eggreadlog, "Parsed header:\n" << headerBuff.str());

        // A few last useful variables
        fRecordSize = fHeader.GetRecordSize();
        fBinWidth = 1. / fHeader.GetAcquisitionRate();
        fSliceNumber = 0;
        fRecordsRead = 0;
        fSamplesRead = 0;

        // Get the pointer to the data array
        ts_array_mat = matGetVariable(matfilep, "Y");

        return new KTEggHeader(fHeader);
    }
    KTDataPtr KTRSAMatReader::HatchNextSlice()
    {

        // IMPORTANT:
        // KTRSAMatReader::HatchNextSlice is currently only capable of reading MAT files containing a single acquisitiona, a single record and a single channel

        unsigned recordSize = fHeader.GetRecordSize();
        KTDataPtr newData(new KTData());

        // ********************************************************* //
        // Check whether we still have enough data to fill up slice  //
        // ********************************************************* //
        if (fSamplesRead+fSliceSize > fRecordSize) {
                    KTWARN(eggreadlog, "End of mat file reached");
                    KTDEBUG(eggreadlog,"fSamplesRead: " << fSamplesRead << "; fSliceSize: " << fSliceSize << "; fRecordSize: " << fRecordSize);

                    // Return Empty Pointer
                    return KTDataPtr();
        }


        // ********************************** //
        // Fill out slice header information  //
        // ********************************** //
        KTSliceHeader& sliceHeader = newData->Of< KTSliceHeader >().SetNComponents(fHeader.GetNChannels());
        if (fSliceNumber==0)
        {
            sliceHeader.SetIsNewAcquisition(true);
        }
        else
        {
            sliceHeader.SetIsNewAcquisition(false);
        }
        ++fSliceNumber;

        // Slice Header Variables
        sliceHeader.SetSampleRate(fHeader.GetAcquisitionRate());
        sliceHeader.SetRawSliceSize(fSliceSize);
        sliceHeader.SetSliceSize(fSliceSize);
        sliceHeader.CalculateBinWidthAndSliceLength();
        sliceHeader.SetNonOverlapFrac((double)fStride / (double)fSliceSize);
        sliceHeader.SetTimeInRun(GetTimeInRun());
        sliceHeader.SetSliceNumber(fSliceNumber);
        sliceHeader.SetStartRecordNumber(fRecordsRead);
        sliceHeader.SetStartSampleNumber(fSamplesRead);
        sliceHeader.SetRecordSize(fHeader.GetRecordSize());
        // Slice Header Variables that depend on channel number
        unsigned iChannel = 0;
        sliceHeader.SetAcquisitionID(0, iChannel);
        sliceHeader.SetRecordID(fRecordsRead, iChannel);
        sliceHeader.SetTimeStamp(sliceHeader.GetTimeInRun() / SEC_PER_NSEC, iChannel);
        KTDEBUG(eggreadlog, sliceHeader << "\nNote: some fields may not be filled in correctly yet");

        // ********************************** //
        // Read data                          //
        // ********************************** //
        KTTimeSeriesFFTW* newSliceComplex = new KTTimeSeriesFFTW(sliceHeader.GetSliceSize(), 0., double(sliceHeader.GetSliceSize()) * sliceHeader.GetBinWidth());
        float *data_real_ptr;
        float *data_imag_ptr;

        data_real_ptr = (float *)mxGetData(ts_array_mat);
        data_imag_ptr = (float *)mxGetImagData(ts_array_mat);
        for (unsigned iBin=0; iBin<fSliceSize; iBin++)
        {
            (*newSliceComplex)(iBin)[0] = double(data_real_ptr[iBin + fSamplesRead]);
            (*newSliceComplex)(iBin)[1] = double(data_imag_ptr[iBin + fSamplesRead]);
        }
        KTTimeSeries* newSlice = newSliceComplex;
        fSamplesRead = fSamplesRead+fSliceSize;
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
        if (matClose(matfilep) != 0) {
            KTERROR(eggreadlog, "Something went wrong while closing the mat file");
            return false;
        }

        
        return true;
    }



} /* namespace Katydid */


