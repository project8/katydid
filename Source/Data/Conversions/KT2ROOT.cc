/*
 * KT2ROOT.cc
 *
 *  Created on: Dec 23, 2013
 *      Author: nsoblath
 */

#include "KT2ROOT.hh"

#include "KTRawTimeSeries.hh"
#include "KTTimeSeriesDist.hh"
#include "KTTimeSeriesFFTW.hh"
#include "KTTimeSeriesReal.hh"

#include "TH1.h"

#include <cfloat>
#include "stdint.h"
#ifndef UINT16_MAX
#define UINT16_MAX (32767)
#endif
//#include <stdint.h>

using std::string;

namespace Katydid
{

    KT2ROOT::KT2ROOT()
    {
    }

    KT2ROOT::~KT2ROOT()
    {
    }

    TH1I* KT2ROOT::CreateHistogram(const KTRawTimeSeries* ts, const string& histName)
    {
        unsigned nBins = ts->size();
        TH1I* hist = new TH1I(histName.c_str(), "Time Series", (int)nBins, ts->GetRangeMin(), ts->GetRangeMax());
        for (unsigned iBin=0; iBin<nBins; ++iBin)
        {
            hist->SetBinContent((int)iBin+1, (*ts)(iBin));
        }
        hist->SetXTitle("Time (s)");
        hist->SetYTitle("Voltage (V)");
        return hist;

    }

    TH1I* KT2ROOT::CreateHistogram(const KTTimeSeriesDist* tsDist, const string& histName)
    {
        unsigned nBins = ts->size();
        TH1I* hist = new TH1I(histName.c_str(), "Time Series", (int)nBins, ts->GetRangeMin(), ts->GetRangeMax());
        for (unsigned iBin=0; iBin<nBins; ++iBin)
        {
            hist->SetBinContent((int)iBin+1, (*ts)(iBin));
        }
        hist->SetXTitle("Time (s)");
        hist->SetYTitle("Voltage (V)");
        return hist;

    }

    TH1I* KT2ROOT::CreateAmplitudeDistributionHistogram(const KTRawTimeSeries* ts, const string& histName)
    {
        unsigned tMaxMag = 0;
        unsigned tMinMag = UINT16_MAX;//std::numeric_limits<int16_t>::max();
        unsigned nBins = ts->GetNBins();
        unsigned value;
        for (unsigned iBin=0; iBin<nBins; ++iBin)
        {
            value = (*ts)(iBin);
            //value *= value;
            if (value < tMinMag) tMinMag = value;
            if (value > tMaxMag) tMaxMag = value;
        }
        if (tMinMag < 1. && tMaxMag > 1.) tMinMag = 0.;
        TH1I* hist = new TH1I(histName.c_str(), "Voltage Distribution", 100, tMinMag*0.95, tMaxMag*1.05);
        for (unsigned iBin=0; iBin<nBins; ++iBin)
        {
            //value = (*this)(iBin);
            //hist->Fill(value*value);
            hist->Fill((*ts)(iBin));
        }
        hist->SetXTitle("Voltage (V)");
        return hist;
    }

    TH1D* KT2ROOT::CreateHistogram(const KTTimeSeriesFFTW* ts, const std::string& histName)
    {
        unsigned nBins = ts->GetNBins();
        TH1D* hist = new TH1D(histName.c_str(), "Time Series", (int)nBins, ts->GetRangeMin(), ts->GetRangeMax());
        for (unsigned iBin=0; iBin<nBins; ++iBin)
        {
            hist->SetBinContent((int)iBin+1, (*ts)(iBin)[0]);
        }
        hist->SetXTitle("Time (s)");
        hist->SetYTitle("Voltage (V)");
        return hist;
    }

    TH1D* KT2ROOT::CreateAmplitudeDistributionHistogram(const KTTimeSeriesFFTW* ts, const std::string& histName)
    {
        double tMaxMag = -DBL_MAX;
        double tMinMag = DBL_MAX;
        unsigned nBins = ts->GetNTimeBins();
        double value;
        for (unsigned iBin=0; iBin<nBins; ++iBin)
        {
            value = (*ts)(iBin)[0];
            //value *= value;
            if (value < tMinMag) tMinMag = value;
            if (value > tMaxMag) tMaxMag = value;
        }
        if (tMinMag < 1. && tMaxMag > 1.) tMinMag = 0.;
        TH1D* hist = new TH1D(histName.c_str(), "Voltage Distribution", 100, tMinMag*0.95, tMaxMag*1.05);
        for (unsigned iBin=0; iBin<nBins; ++iBin)
        {
            //value = (*this)(iBin);
            //hist->Fill(value*value);
            hist->Fill((*ts)(iBin)[0]);
        }
        hist->SetXTitle("Voltage (V)");
        return hist;
    }

    TH1D* KT2ROOT::CreateHistogram(const KTTimeSeriesReal* ts, const std::string& histName)
    {
        unsigned nBins = ts->GetNBins();
        TH1D* hist = new TH1D(histName.c_str(), "Time Series", (int)nBins, ts->GetRangeMin(), ts->GetRangeMax());
        for (unsigned iBin=0; iBin<nBins; ++iBin)
        {
            hist->SetBinContent((int)iBin+1, (*ts)(iBin));
        }
        hist->SetXTitle("Time (s)");
        hist->SetYTitle("Voltage (V)");
        return hist;
    }

    TH1D* KT2ROOT::CreateAmplitudeDistributionHistogram(const KTTimeSeriesReal* ts, const std::string& histName)
    {
        double tMaxMag = -DBL_MAX;
        double tMinMag = DBL_MAX;
        unsigned nBins = ts->GetNTimeBins();
        double value;
        for (unsigned iBin=0; iBin<nBins; ++iBin)
        {
            value = (*ts)(iBin);
            //value *= value;
            if (value < tMinMag) tMinMag = value;
            if (value > tMaxMag) tMaxMag = value;
        }
        if (tMinMag < 1. && tMaxMag > 1.) tMinMag = 0.;
        TH1D* hist = new TH1D(histName.c_str(), "Voltage Distribution", 100, tMinMag*0.95, tMaxMag*1.05);
        for (unsigned iBin=0; iBin<nBins; ++iBin)
        {
            //value = (*this)(iBin);
            //hist->Fill(value*value);
            hist->Fill((*ts)(iBin));
        }
        hist->SetXTitle("Voltage (V)");
        return hist;
    }

} /* namespace Katydid */
