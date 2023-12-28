/*
 * KTDataTypeDisplayTime.cc
 *
 *  Created on: December 13, 2013
 *      Author: nsoblath
 */

#include "KT2ROOT.hh"
#include "KTTIFactory.hh"
#include "logger.hh"
#include "KTRawTimeSeriesData.hh"
#include "KTRawTimeSeries.hh"
#include "KTSliceHeader.hh"
#include "KTTimeSeries.hh"
#include "KTTimeSeriesData.hh"

#include "TH1.h"

#include <sstream>

#include "KTDataTypeDisplayTime.hh"

using std::stringstream;
using std::string;

namespace Katydid
{
    LOGGER(publog, "katydid.output");

    static Nymph::KTTIRegistrar< KTDataTypeDisplay, KTDataTypeDisplayTime > sDTDERegistrar;

    KTDataTypeDisplayTime::KTDataTypeDisplayTime() :
            KTDataTypeDisplay()
    {
    }

    KTDataTypeDisplayTime::~KTDataTypeDisplayTime()
    {
    }


    void KTDataTypeDisplayTime::RegisterSlots()
    {
        fWriter->RegisterSlot("raw-ts", this, &KTDataTypeDisplayTime::DrawRawTimeSeriesData);
        fWriter->RegisterSlot("ts", this, &KTDataTypeDisplayTime::DrawTimeSeriesData);
        fWriter->RegisterSlot("ts-dist", this, &KTDataTypeDisplayTime::DrawTimeSeriesDataDistribution);
        return;
    }


    //*********************
    // Raw Time Series Data
    //*********************

    void KTDataTypeDisplayTime::DrawRawTimeSeriesData(Nymph::KTDataPtr data)
    {
        if (! data) return;

        KTSliceHeader& slHeader = data->Of<KTSliceHeader>();
        ULong64_t sliceNumber = slHeader.GetSliceNumber();

        KTRawTimeSeriesData& tsData = data->Of<KTRawTimeSeriesData>();
        UInt_t nComponents = tsData.GetNComponents();

        if (! fWriter->OpenWindow()) return;

        for (UInt_t iComponent=0; iComponent<nComponents; ++iComponent)
        {
            const KTRawTimeSeries* timeSeries = tsData.GetTimeSeries(iComponent);
            if (timeSeries != NULL)
            {
                stringstream conv;
                conv << "histRawTS_" << sliceNumber << "_" << iComponent;
                string histName;
                conv >> histName;
                if (slHeader.GetRawDataFormatType(iComponent) == sDigitizedUS)
                {
                    TH1I* tsHist = KT2ROOT::CreateHistogram(timeSeries, histName);
                    fWriter->Draw(tsHist);
                }
                else if(slHeader.GetRawDataFormatType(iComponent) == sDigitizedS)
                {
                    KTVarTypePhysicalArray< int64_t > array(*timeSeries, false);
                    TH1I* tsHist = KT2ROOT::CreateHistogram(&array, histName);
                    fWriter->Draw(tsHist);
                }
            }
        }
        return;
    }

    //*****************
    // Time Series Data
    //*****************

    void KTDataTypeDisplayTime::DrawTimeSeriesData(Nymph::KTDataPtr data)
    {
        if (! data) return;

        ULong64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTTimeSeriesData& tsData = data->Of<KTTimeSeriesData>();
        UInt_t nComponents = tsData.GetNComponents();

        if (! fWriter->OpenWindow()) return;

        for (UInt_t iComponent=0; iComponent<nComponents; ++iComponent)
        {
            const KTTimeSeries* timeSeries = tsData.GetTimeSeries(iComponent);
            if (timeSeries != NULL)
            {
                stringstream conv;
                conv << "histTS_" << sliceNumber << "_" << iComponent;
                string histName;
                conv >> histName;
                TH1D* tsHist = timeSeries->CreateHistogram(histName);
                fWriter->Draw(tsHist);
            }
        }
        return;
    }

    void KTDataTypeDisplayTime::DrawTimeSeriesDataDistribution(Nymph::KTDataPtr data)
    {
        if (! data) return;

        ULong64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTTimeSeriesData& tsData = data->Of<KTTimeSeriesData>();
        UInt_t nComponents = tsData.GetNComponents();

        if (! fWriter->OpenWindow()) return;

        for (UInt_t iComponent=0; iComponent<nComponents; ++iComponent)
        {
            const KTTimeSeries* timeSeries = tsData.GetTimeSeries(iComponent);
            if (timeSeries != NULL)
            {
                stringstream conv;
                conv << "histTSDist_" << sliceNumber << "_" << iComponent;
                string histName;
                conv >> histName;
                TH1D* tsDistHist = timeSeries->CreateAmplitudeDistributionHistogram(histName);
                fWriter->Draw(tsDistHist);
            }
        }
        return;
    }

} /* namespace Katydid */
