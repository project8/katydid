/*
 * KTDataTypeDisplayEgg.cc
 *
 *  Created on: December 13, 2013
 *      Author: nsoblath
 */

#include "KTDataTypeDisplayEgg.hh"

#include "KT2ROOT.hh"
#include "KTTIFactory.hh"
#include "KTLogger.hh"
#include "KTRawTimeSeriesData.hh"
#include "KTRawTimeSeries.hh"
#include "KTSliceHeader.hh"
#include "KTTimeSeries.hh"
#include "KTTimeSeriesData.hh"

#include "TH1.h"

#include <sstream>

using std::stringstream;
using std::string;

namespace Katydid
{
    KTLOGGER(publog, "katydid.output");

    static KTTIRegistrar< KTDataTypeDisplay, KTDataTypeDisplayEgg > sDTDERegistrar;

    KTDataTypeDisplayEgg::KTDataTypeDisplayEgg() :
            KTDataTypeDisplay()
    {
    }

    KTDataTypeDisplayEgg::~KTDataTypeDisplayEgg()
    {
    }


    void KTDataTypeDisplayEgg::RegisterSlots()
    {
        fWriter->RegisterSlot("raw-ts", this, &KTDataTypeDisplayEgg::DrawRawTimeSeriesData);
        fWriter->RegisterSlot("ts", this, &KTDataTypeDisplayEgg::DrawTimeSeriesData);
        fWriter->RegisterSlot("ts-dist", this, &KTDataTypeDisplayEgg::DrawTimeSeriesDataDistribution);
        return;
    }


    //*********************
    // Raw Time Series Data
    //*********************

    void KTDataTypeDisplayEgg::DrawRawTimeSeriesData(KTDataPtr data)
    {
        if (! data) return;

        ULong64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

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
                TH1I* tsHist = KT2ROOT::CreateHistogram(timeSeries, histName);
                fWriter->Draw(tsHist);
            }
        }
        return;
    }

    //*****************
    // Time Series Data
    //*****************

    void KTDataTypeDisplayEgg::DrawTimeSeriesData(KTDataPtr data)
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

    void KTDataTypeDisplayEgg::DrawTimeSeriesDataDistribution(KTDataPtr data)
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
