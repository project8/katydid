/*
 * KTTimeSeriesReal.cc
 *
 *  Created on: Aug 28, 2012
 *      Author: nsoblath
 */

#include "KTTimeSeriesReal.hh"

#include "logger.hh"

#ifdef ROOT_FOUND
#include "TH1.h"
#endif

#include <sstream>

using std::stringstream;

namespace Katydid
{
    LOGGER(tslog, "KTTimeSeriesReal");

    KTTimeSeriesReal::KTTimeSeriesReal() :
            KTTimeSeries(),
            KTPhysicalArray< 1, double >()
    {
    }

    KTTimeSeriesReal::KTTimeSeriesReal(size_t nBins, double rangeMin, double rangeMax) :
            KTTimeSeries(),
            KTPhysicalArray< 1, double >(nBins, rangeMin, rangeMax)
    {
    }

    KTTimeSeriesReal::KTTimeSeriesReal(double value, size_t nBins, double rangeMin, double rangeMax) :
            KTTimeSeriesReal(nBins, rangeMin, rangeMax)
    {
        for (unsigned iBin = 0; iBin < nBins; ++iBin)
        {
            fData[iBin] = value;
        }
    }

    KTTimeSeriesReal::KTTimeSeriesReal(const KTTimeSeriesReal& orig) :
            KTTimeSeries(),
            KTPhysicalArray< 1, double >(orig)
    {
    }

    KTTimeSeriesReal::~KTTimeSeriesReal()
    {
    }

    KTTimeSeriesReal& KTTimeSeriesReal::operator=(const KTTimeSeriesReal& rhs)
    {
        KTPhysicalArray< 1, double >::operator=(rhs);
        return *this;
    }

    void KTTimeSeriesReal::Print(unsigned startPrint, unsigned nToPrint) const
    {
        stringstream printStream;
        for (unsigned iBin = startPrint; iBin < startPrint + nToPrint; ++iBin)
        {
            printStream << "Bin " << iBin << ";   x = " << GetBinCenter(iBin) <<
                    ";   y = " << (*this)(iBin) << "\n";
        }
        LDEBUG(tslog, "\n" << printStream.str());
        return;
    }

#ifdef ROOT_FOUND
    TH1D* KTTimeSeriesReal::CreateHistogram(const std::string& name) const
    {
        unsigned nBins = GetNBins();
        TH1D* hist = new TH1D(name.c_str(), "Time Series", (int)nBins, GetRangeMin(), GetRangeMax());
        for (unsigned iBin=0; iBin<nBins; ++iBin)
        {
            hist->SetBinContent((int)iBin+1, (*this)(iBin));
        }
        /*
        std::stringstream tsstream, histstream;
        for (unsigned i=0; i<30; ++i)
        {
            tsstream << (*this)(i) << "  ";
            histstream << hist->GetBinContent((int)i+1) << "  ";
        }
        LWARN( tslog, "ts: " << tsstream.str() );
        LWARN( tslog, "hist: " << histstream.str() );
        */
        //**** DEBUG ****//
        hist->SetXTitle("Time (s)");
        hist->SetYTitle("Voltage (V)");
        return hist;
    }

    TH1D* KTTimeSeriesReal::CreateAmplitudeDistributionHistogram(const std::string& name) const
    {
        double tMaxMag = -1.;
        double tMinMag = 1.e9;
        unsigned nBins = GetNTimeBins();
        double value;
        for (unsigned iBin=0; iBin<nBins; ++iBin)
        {
            value = (*this)(iBin);
            //value *= value;
            if (value < tMinMag) tMinMag = value;
            if (value > tMaxMag) tMaxMag = value;
        }
        if (tMinMag < 1. && tMaxMag > 1.) tMinMag = 0.;
        TH1D* hist = new TH1D(name.c_str(), "Voltage Distribution", 100, tMinMag*0.95, tMaxMag*1.05);
        for (unsigned iBin=0; iBin<nBins; ++iBin)
        {
            //value = (*this)(iBin);
            //hist->Fill(value*value);
            hist->Fill((*this)(iBin));
        }
        hist->SetXTitle("Voltage (V)");
        return hist;
    }

#endif

} /* namespace Katydid */
