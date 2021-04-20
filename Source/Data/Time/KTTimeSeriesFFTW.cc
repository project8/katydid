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
    KTLOGGER(tslog, "KTTimeSeriesFFTW");

    KTTimeSeriesFFTW::KTTimeSeriesFFTW() :
            KTTimeSeries(),
            KTPhysicalArray< 1, std::complex<double> >()
    {
    }

    KTTimeSeriesFFTW::KTTimeSeriesFFTW(size_t nBins, double rangeMin, double rangeMax) :
            KTTimeSeries(),
            KTPhysicalArray< 1, std::complex<double> >(nBins, rangeMin, rangeMax)
    {
    }

    KTTimeSeriesFFTW::KTTimeSeriesFFTW(std::initializer_list<double> value, size_t nBins, double rangeMin, double rangeMax) :
            KTTimeSeriesFFTW(nBins, rangeMin, rangeMax)
    {
        if (value.size() != 2)
        {
            throw std::runtime_error("Invalid initial complex value; requires 2 elements");
        }
        for (unsigned iBin = 0; iBin < nBins; ++iBin)
        {
            std::copy(value.begin(), value.end(), &fData[iBin]);
        }
        
    }
    
    // rule of zero, the default ones should do the job

    //~ KTTimeSeriesFFTW::KTTimeSeriesFFTW(const KTTimeSeriesFFTW& orig) :
            //~ KTTimeSeries(),
            //~ KTPhysicalArray< 1, std::complex<double> >(orig)
    //~ {
    //~ }

    //~ KTTimeSeriesFFTW::~KTTimeSeriesFFTW()
    //~ {
    //~ }

    //~ KTTimeSeriesFFTW& KTTimeSeriesFFTW::operator=(const KTTimeSeriesFFTW& rhs)
    //~ {
        //~ KTPhysicalArray< 1, std::complex<double> >::operator=(rhs);
        //~ return *this;
    //~ }

    void KTTimeSeriesFFTW::Print(unsigned startPrint, unsigned nToPrint) const
    {
        stringstream printStream;
        for (unsigned iBin = startPrint; iBin < startPrint + nToPrint; ++iBin)
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
        unsigned nBins = GetNBins();
        TH1D* hist = new TH1D(name.c_str(), "Time Series", (int)nBins, GetRangeMin(), GetRangeMax());
        for (unsigned iBin=0; iBin<nBins; ++iBin)
        {
            hist->SetBinContent((int)iBin+1, (*this)(iBin).real());
        }
        hist->SetXTitle("Time (s)");
        hist->SetYTitle("Voltage (V)");
        return hist;
    }

    TH1D* KTTimeSeriesFFTW::CreateAmplitudeDistributionHistogram(const std::string& name) const
    {
        double tMaxMag = -1.;
        double tMinMag = 1.e9;
        unsigned nBins = GetNTimeBins();
        double value;
        for (unsigned iBin=0; iBin<nBins; ++iBin)
        {
            value = (*this)(iBin).real();
            //value *= value;
            if (value < tMinMag) tMinMag = value;
            if (value > tMaxMag) tMaxMag = value;
        }
        if (tMinMag < 1. && tMaxMag > 1.) tMinMag = 0.;
        TH1D* hist = new TH1D(name.c_str(), "Voltage Distribution", 100, tMinMag*0.95, tMaxMag*1.05);
        for (unsigned iBin=0; iBin<nBins; ++iBin)
        {
            //value = (*this)(iBin)[0];
            //hist->Fill(value*value);
            hist->Fill((*this)(iBin).real());
        }
        hist->SetXTitle("Voltage (V)");
        return hist;
    }

#endif

} /* namespace Katydid */
