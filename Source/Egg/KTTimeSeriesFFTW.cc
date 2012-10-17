/*
 * KTTimeSeriesFFTW.cc
 *
 *  Created on: Aug 28, 2012
 *      Author: nsoblath
 */

#include "KTTimeSeriesFFTW.hh"

#include "KTLogger.hh"

#ifdef ROOT_FOUND
#include "TH1.h"
#endif

#include <sstream>

using std::stringstream;

namespace Katydid
{
    KTLOGGER(tslog, "katydid.egg");

    KTTimeSeriesFFTW::KTTimeSeriesFFTW() :
            KTTimeSeries(),
            KTPhysicalArray< 1, fftw_complex >()
    {
    }

    KTTimeSeriesFFTW::KTTimeSeriesFFTW(size_t nBins, Double_t rangeMin, Double_t rangeMax) :
            KTTimeSeries(),
            KTPhysicalArray< 1, fftw_complex >(nBins, rangeMin, rangeMax)
    {
    }
    KTTimeSeriesFFTW::KTTimeSeriesFFTW(const KTTimeSeriesFFTW& orig) :
            KTTimeSeries(),
            KTPhysicalArray< 1, fftw_complex >(orig)
    {
    }

    KTTimeSeriesFFTW::~KTTimeSeriesFFTW()
    {
    }

    KTTimeSeriesFFTW& KTTimeSeriesFFTW::operator=(const KTTimeSeriesFFTW& rhs)
    {
        KTPhysicalArray< 1, fftw_complex >::operator=(rhs);
        return *this;
    }

    void KTTimeSeriesFFTW::Print(unsigned startPrint, unsigned nToPrint) const
    {
        stringstream printStream;
        for (unsigned iBin = startPrint; iBin < startPrint + nToPrint; iBin++)
        {
            printStream << "Bin " << iBin << ";   x = " << GetBinCenter(iBin) <<
                    ";   y = " << (*this)(iBin) << "\n";
        }
        KTDEBUG(tslog, "\n" << printStream.str());
        return;
    }

#ifdef ROOT_FOUND
    TH1D* KTTimeSeriesFFTW::CreateHistogram(const std::string& name) const
    {
        UInt_t nBins = GetNBins();
        TH1D* hist = new TH1D(name.c_str(), "Time Series", (Int_t)nBins, GetRangeMin(), GetRangeMax());
        for (UInt_t iBin=0; iBin<nBins; iBin++)
        {
            hist->SetBinContent((Int_t)iBin+1, (*this)(iBin)[0]);
        }
        hist->SetXTitle("Time (s)");
        hist->SetYTitle("Voltage (V)");
        return hist;
    }

    TH1D* KTTimeSeriesFFTW::CreateAmplitudeDistributionHistogram(const std::string& name) const
    {
        Double_t tMaxMag = -1.;
        Double_t tMinMag = 1.e9;
        UInt_t nBins = GetNBins();
        Double_t value;
        for (UInt_t iBin=0; iBin<nBins; iBin++)
        {
            value = (*this)(iBin)[0];
            value *= value;
            if (value < tMinMag) tMinMag = value;
            if (value > tMaxMag) tMaxMag = value;
        }
        if (tMinMag < 1. && tMaxMag > 1.) tMinMag = 0.;
        TH1D* hist = new TH1D(name.c_str(), "Voltage Distribution", 100, tMinMag*0.95, tMaxMag*1.05);
        for (UInt_t iBin=0; iBin<nBins; iBin++)
        {
            value = (*this)(iBin)[0];
            hist->Fill(value*value);
        }
        hist->SetXTitle("Voltage (V)");
        return hist;
    }

#endif

} /* namespace Katydid */
