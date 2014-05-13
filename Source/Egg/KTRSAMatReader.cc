/*
 * KTEgg.cc
 *
 *  Created on: Sep 9, 2011
 *      Author: nsoblath
 */


#include "KTRSAMatReader.hh"
#include "KTLogger.hh"
#include "KTSliceHeader.hh"
#include "KTRawTimeSeriesData.hh"
#include "KTRawTimeSeries.hh"
#include "rapidxml.hpp"

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
            fSliceNumber(0)
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
        cout << "Name of my current node is: " << curr_node->name() << "\n";
        printf("Sampling Frequency: %s\n", curr_node->value());

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
        //fHeader.SetDescription(monarchHeader->GetDescription());
        //fHeader.SetRunType(monarchHeader->GetRunType());
        //fHeader.SetRunSource(monarchHeader->GetRunSource());
        //fHeader.SetFormatMode(monarchHeader->GetFormatMode());
        //fHeader.SetBitDepth(monarchHeader->GetBitDepth());
        //fHeader.SetVoltageMin(monarchHeader->GetVoltageMin());
        //fHeader.SetVoltageRange(monarchHeader->GetVoltageRange());


        // Get configuration from JSON config file
        fHeader.SetRawSliceSize(fSliceSize);
        fHeader.SetSliceSize(fSliceSize);


        stringstream headerBuff;
        headerBuff << fHeader;
        KTDEBUG(eggreadlog, "Parsed header:\n" << headerBuff.str());

        fRecordSize = fHeader.GetRecordSize();
        fBinWidth = 1. / fHeader.GetAcquisitionRate();
        fSliceNumber = 0;

        return new KTEggHeader(fHeader);
    }
    KTDataPtr KTRSAMatReader::HatchNextSlice()
    {
        KTDataPtr newData(new KTData());
        return newData;

    }


    bool KTRSAMatReader::CloseEgg()
    {
        //KTERROR(eggreadlog, "Something went wrong while closing the file: " << "e.what()");
        return true;
    }



} /* namespace Katydid */


