/*
 * KT2ROOT.cc
 *
 *  Created on: Dec 23, 2013
 *      Author: nsoblath
 */

#include "KT2ROOT.hh"

#include "KTLogger.hh"

#include "KTHoughData.hh"
#include "KTPowerSpectrum.hh"
#include "KTRawTimeSeries.hh"
#include "KTTimeSeriesDist.hh"
#include "KTTimeSeriesFFTW.hh"
#include "KTTimeSeriesReal.hh"

#include "TH1.h"
#include "TH2.h"

#include <cfloat>
#include "stdint.h"
#ifndef UINT16_MAX
#define UINT16_MAX (32767)
#endif
//#include <stdint.h>

using std::string;

namespace Katydid
{
    KTLOGGER(dblog, "KT2ROOT");

    KT2ROOT::KT2ROOT()
    {
    }

    KT2ROOT::~KT2ROOT()
    {
    }

    TH1I* KT2ROOT::CreateHistogram(const KTVarTypePhysicalArray< uint64_t >* ts, const string& histName)
    {
        unsigned nBins = ts->size();
        TH1I* hist = new TH1I(histName.c_str(), "Raw Time Series", (int)nBins, ts->GetRangeMin(), ts->GetRangeMax());
        for (unsigned iBin=0; iBin<nBins; ++iBin)
        {
            hist->SetBinContent((int)iBin+1, (*ts)(iBin));
        }
        hist->SetXTitle("Time (s)");
        hist->SetYTitle("Voltage (ADC)");
        return hist;

    }

    TH1I* KT2ROOT::CreateHistogram(const KTVarTypePhysicalArray< int64_t >* ts, const string& histName)
    {
        unsigned nBins = ts->size();
        TH1I* hist = new TH1I(histName.c_str(), "Raw Time Series", (int)nBins, ts->GetRangeMin(), ts->GetRangeMax());
        for (unsigned iBin=0; iBin<nBins; ++iBin)
        {
            hist->SetBinContent((int)iBin+1, (*ts)(iBin));
        }
        hist->SetXTitle("Time (s)");
        hist->SetYTitle("Voltage (ADC)");
        return hist;

    }

    TH1I* KT2ROOT::CreateHistogram(const KTTimeSeriesDist* tsDist, const string& histName)
    {
        unsigned nBins = tsDist->size();
        TH1I* hist = new TH1I(histName.c_str(), "Time Series Distribution", (int)nBins, tsDist->GetRangeMin(), tsDist->GetRangeMax());
        for (unsigned iBin=0; iBin<nBins; ++iBin)
        {
            hist->SetBinContent((int)iBin+1, (*tsDist)(iBin));
        }
        hist->SetXTitle("Amplitude");
        hist->SetYTitle("Occurrences (#)");
        return hist;
    }

    /*TH1I* KT2ROOT::CreateAmplitudeDistributionHistogram(const KTRawTimeSeries* ts, const string& histName)
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
    }*/

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

/*    TH1D* KT2ROOT::CreateAmplitudeDistributionHistogram(const KTTimeSeriesFFTW* ts, const std::string& histName)
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
*/

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

/*    TH1D* KT2ROOT::CreateAmplitudeDistributionHistogram(const KTTimeSeriesReal* ts, const std::string& histName)
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
*/

    TH1D* KT2ROOT::CreatePowerHistogram(const KTPowerSpectrum* ps, const std::string& name)
    {
        unsigned nBins = ps->size();
        TH1D* hist = new TH1D(name.c_str(), "Power Spectrum", (int)nBins, ps->GetRangeMin(), ps->GetRangeMax());
        //double value;
        for (unsigned int iBin=0; iBin<nBins; iBin++)
        {
            hist->SetBinContent((int)iBin+1, (*ps)(iBin));
        }
        hist->SetXTitle(ps->GetAxisLabel().c_str());
        hist->SetYTitle(ps->GetDataLabel().c_str());
        return hist;
    }

    TH1D* KT2ROOT::CreatePowerDistributionHistogram(const KTPowerSpectrum* ps, const std::string& name)
    {
        double value = (*ps)(0);
        double tMaxMag = value;
        double tMinMag = value;
        unsigned nBins = ps->size();
        for (unsigned iBin=1; iBin<nBins; iBin++)
        {
            value = (*ps)(iBin);
            if (value < tMinMag) tMinMag = value;
            if (value > tMaxMag) tMaxMag = value;
        }
        if (tMinMag < 1. && tMaxMag > 1.) tMinMag = 0.;
        TH1D* hist = new TH1D(name.c_str(), "Power Distribution", 100, tMinMag*0.95, tMaxMag*1.05);
        for (unsigned iBin=0; iBin<nBins; iBin++)
        {
            hist->Fill((*ps)(iBin));
        }
        hist->SetXTitle(ps->GetDataLabel().c_str());
        return hist;
    }

    TH2D* KT2ROOT::CreateHistogram(const KTPhysicalArray< 2, double >* ht, const std::string& histName)
    {
        TH2D* hist = new TH2D(histName.c_str(), histName.c_str(),
                ht->size(1), ht->GetRangeMin(1), ht->GetRangeMax(1),
                ht->size(2), ht->GetRangeMin(2), ht->GetRangeMax(2));

        for (int iBinX=1; iBinX<=(int)ht->size(1); iBinX++)
        {
            for (int iBinY=1; iBinY<=hist->GetNbinsY(); iBinY++)
            {
                hist->SetBinContent(iBinX, iBinY, (*ht)(iBinX-1, iBinY-1));
            }
        }

        return hist;

    }
} /* namespace Katydid */
