/*
 * KT2ROOT.cc
 *
 *  Created on: Dec 23, 2013
 *      Author: nsoblath
 */

#include "KT2ROOT.hh"

#include "KTRawTimeSeries.hh"

#include "TH1I.h"

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

    TH1I* KT2ROOT::CreateAmplitudeDistributionHistogram(const KTRawTimeSeries* ts, const string& histName)
    {
        unsigned tMaxMag = 0;
        unsigned tMinMag = UINT16_MAX;
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


} /* namespace Katydid */
