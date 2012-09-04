/*
 * KTTimeSeries.cc
 *
 *  Created on: Aug 28, 2012
 *      Author: nsoblath
 */

#include "KTTimeSeries.hh"

#ifdef ROOT_FOUND
#include "TH1.h"
#endif

namespace Katydid
{

    KTTimeSeries::KTTimeSeries() :
            KTPhysicalArray< 1, Double_t >()
    {
    }

    KTTimeSeries::KTTimeSeries(size_t nBins, Double_t rangeMin, Double_t rangeMax) :
            KTPhysicalArray< 1, Double_t >(nBins, rangeMin, rangeMax)
    {
    }
    KTTimeSeries::KTTimeSeries(const KTTimeSeries& orig) :
            KTPhysicalArray< 1, Double_t >(orig)
    {
    }

    KTTimeSeries::~KTTimeSeries()
    {
    }

    KTTimeSeries& KTTimeSeries::operator=(const KTTimeSeries& rhs)
    {
        KTPhysicalArray< 1, Double_t >::operator=(rhs);
        return *this;
    }

#ifdef ROOT_FOUND
    TH1D* KTTimeSeries::CreateHistogram(const std::string& name) const
    {
        UInt_t nBins = GetNBins();
        TH1D* hist = new TH1D(name.c_str(), "Power Spectrum", (Int_t)nBins, GetRangeMin(), GetRangeMax());
        for (UInt_t iBin=0; iBin<nBins; iBin++)
        {
            hist->SetBinContent((Int_t)iBin+1, (*this)[iBin]);
        }
        hist->SetXTitle("Frequency (Hz)");
        hist->SetYTitle("Power");
        return hist;
    }

    TH1D* KTTimeSeries::CreateAmplitudeDistributionHistogram(const std::string& name) const
    {
        Double_t tMaxMag = -1.;
        Double_t tMinMag = 1.e9;
        UInt_t nBins = GetNBins();
        Double_t value;
        for (UInt_t iBin=0; iBin<nBins; iBin++)
        {
            value = (*this)[iBin];
            value *= value;
            if (value < tMinMag) tMinMag = value;
            if (value > tMaxMag) tMaxMag = value;
        }
        if (tMinMag < 1. && tMaxMag > 1.) tMinMag = 0.;
        TH1D* hist = new TH1D(name.c_str(), "Power Distribution", 100, tMinMag*0.95, tMaxMag*1.05);
        for (UInt_t iBin=0; iBin<nBins; iBin++)
        {
            value = (*this)[iBin];
            hist->Fill(value*value);
        }
        hist->SetXTitle("Power");
        return hist;
    }

#endif

} /* namespace Katydid */
