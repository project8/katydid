/*
 * KTEgg.cc
 *
 *  Created on: Sep 9, 2011
 *      Author: nsoblath
 */

#ifdef USE_MATLAB
#endif

#include "KTRSAMatReader.hh"
#include "KTLogger.hh"
#include "KTSliceHeader.hh"
#include "KTRawTimeSeriesData.hh"
#include "KTRawTimeSeries.hh"

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
        //MATFile *pmat;
        if (fStride == 0) fStride = fSliceSize;

        // open the file
        KTINFO(eggreadlog, "Opening egg file <" << filename << ">");
        //pmat = matOpen(filename.c_str(), "r");
        //if (pmat == NULL) {
            KTERROR(eggreadlog, "Unable to break egg: " << filename);
            return NULL;
        //}


        //  ????????????????????


        KTEggHeader* eggHeader = new KTEggHeader();

        return eggHeader;
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


    void KTRSAMatReader::GetHeaderInformation(char *rsaxml_str)
    {
        /* 
        fHeader.SetFilename(monarchHeader->GetFilename());
        fHeader.SetAcquisitionMode(monarchHeader->GetAcquisitionMode());
        fHeader.SetNChannels(fNumberOfChannels[fHeader.GetAcquisitionMode()]);
        fHeader.SetRecordSize(monarchHeader->GetRecordSize());
        fHeader.SetRunDuration(monarchHeader->GetRunDuration());
        fHeader.SetAcquisitionRate(monarchHeader->GetAcquisitionRate() * fSampleRateUnitsInHz);
        fHeader.SetTimestamp(monarchHeader->GetTimestamp());
        fHeader.SetDescription(monarchHeader->GetDescription());
        fHeader.SetRunType(monarchHeader->GetRunType());
        fHeader.SetRunSource(monarchHeader->GetRunSource());
        fHeader.SetFormatMode(monarchHeader->GetFormatMode());
        fHeader.SetDataTypeSize(monarchHeader->GetDataTypeSize());
        fHeader.SetBitDepth(monarchHeader->GetBitDepth());
        fHeader.SetVoltageMin(monarchHeader->GetVoltageMin());
        fHeader.SetVoltageRange(monarchHeader->GetVoltageRange());
        */
        return;
    }


} /* namespace Katydid */


