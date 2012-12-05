/*
 * KTFrequencySpectrumFFTW.cc
 *
 *  Created on: Aug 28, 2012
 *      Author: nsoblath
 */

#include "KTFrequencySpectrumFFTW.hh"

#include "KTLogger.hh"
#include "KTPowerSpectrum.hh"

#ifdef ROOT_FOUND
#include "TH1.h"
#endif

#include <sstream>

using std::stringstream;

namespace Katydid
{
    KTLOGGER(fslog, "katydid.fft");

    KTFrequencySpectrumFFTW::KTFrequencySpectrumFFTW() :
            KTPhysicalArray< 1, fftw_complex >(),
            fIsSizeEven(true),
            fNegFreqOffset(0),
            fDCBin(0)
    {
    }

    KTFrequencySpectrumFFTW::KTFrequencySpectrumFFTW(size_t nBins, Double_t rangeMin, Double_t rangeMax) :
            KTPhysicalArray< 1, fftw_complex >(nBins, rangeMin, rangeMax),
            fIsSizeEven(nBins%2 == 0),
            fNegFreqOffset((nBins+1)/2),
            fDCBin(nBins/2)
    {
        //KTINFO(fslog, "number of bins: " << nBins << "   is size even? " << fIsSizeEven);
        //KTINFO(fslog, "neg freq offset: " << fNegFreqOffset);
    }

    KTFrequencySpectrumFFTW::KTFrequencySpectrumFFTW(const KTFrequencySpectrumFFTW& orig) :
            KTPhysicalArray< 1, fftw_complex >(orig),
            fIsSizeEven(orig.fIsSizeEven),
            fNegFreqOffset(orig.fNegFreqOffset),
            fDCBin(orig.fDCBin)
    {
    }

    KTFrequencySpectrumFFTW::~KTFrequencySpectrumFFTW()
    {
    }

    KTFrequencySpectrumFFTW& KTFrequencySpectrumFFTW::operator=(const KTFrequencySpectrumFFTW& rhs)
    {
        KTPhysicalArray< 1, fftw_complex >::operator=(rhs);
        fIsSizeEven = rhs.fIsSizeEven;
        fNegFreqOffset = rhs.fNegFreqOffset;
        fDCBin = rhs.fDCBin;
        return *this;
    }

    KTFrequencySpectrumFFTW& KTFrequencySpectrumFFTW::CConjugate()
    {
        UInt_t nBins = size();
        for (UInt_t iBin=0; iBin<nBins; iBin++)
        {
            // order doesn't matter, so use fData[] to access values
            fData[iBin][1] = -fData[iBin][1];
        }
        return *this;
    }


    KTPowerSpectrum* KTFrequencySpectrumFFTW::CreatePowerSpectrum() const
    {
        // The negative frequency values will be combined with the positive ones,
        // so the power spectrum will go from the DC bin to the max frequency

        UInt_t nBins = fDCBin + 1;
        KTPowerSpectrum* newPS = new KTPowerSpectrum(nBins, -0.5 * GetBinWidth(), GetRangeMax());
        Double_t value, valueImag, valueReal;
        Double_t scaling = 1. / KTPowerSpectrum::GetResistance();

        // DC bin
        value = (*this)(fDCBin)[0] * (*this)(fDCBin)[0] + (*this)(fDCBin)[1] * (*this)(fDCBin)[1];
        (*newPS)(0) = value * scaling;

        // All bins besides the Nyquist and DC bins
        UInt_t totalBins = size();
        UInt_t iPosBin = fDCBin + 1;
        UInt_t iNegBin = fDCBin - 1;
        for (UInt_t iBin=1; iBin<nBins-1; iBin++)
        {
            //std::cout << iBin << "  " << iPosBin << "  " << iNegBin << std::endl;
            // order matters, so use (*this)() to access values
            valueReal = (*this)(iNegBin)[0] + (*this)(iPosBin)[0];
            valueImag = (*this)(iNegBin)[1] + (*this)(iPosBin)[1];
            value = valueReal * valueReal + valueImag * valueImag;
            (*newPS)(iBin) = value * scaling; // use iNegBin, even though the power spectrum only has positive frequencies
            iPosBin++;
            iNegBin--;
        }

        // Nyquist bin
        if (fIsSizeEven)
        {
            value = (*this)(0)[0] * (*this)(0)[0] + (*this)(0)[1] * (*this)(0)[1];
            (*newPS)(nBins-1) = value * scaling;
        }
        else
        {
            valueReal = (*this)(0)[0] + (*this)(totalBins-1)[0];
            valueImag = (*this)(0)[1] + (*this)(totalBins-1)[1];
            value = valueReal * valueReal + valueImag * valueImag;
            (*newPS)(nBins-1) = value * scaling;
        }

        return newPS;
    }

    void KTFrequencySpectrumFFTW::Print(unsigned startPrint, unsigned nToPrint) const
    {
        stringstream printStream;
        for (unsigned iBin = startPrint; iBin < startPrint + nToPrint; iBin++)
        {
            // order matters, so use (*this)() to access values
            printStream << "Bin " << iBin << ";   x = " << GetBinCenter(iBin) <<
                    ";   y = " << (*this)(iBin) << "\n";
        }
        KTDEBUG(fslog, "\n" << printStream.str());
        return;
    }


#ifdef ROOT_FOUND
    TH1D* KTFrequencySpectrumFFTW::CreateMagnitudeHistogram(const std::string& name) const
    {
        UInt_t nBins = size();
        TH1D* hist = new TH1D(name.c_str(), "Frequency Spectrum: Magnitude", (Int_t)nBins, GetRangeMin(), GetRangeMax());
        for (UInt_t iBin=0; iBin<nBins; iBin++)
        {
            // order matters, so use (*this)() to access values
            hist->SetBinContent((Int_t)iBin+1, std::sqrt((*this)(iBin)[0] * (*this)(iBin)[0] + (*this)(iBin)[1] * (*this)(iBin)[1]));
        }
        hist->SetXTitle("Frequency (Hz)");
        hist->SetYTitle("Voltage (V)");
        return hist;
    }

    TH1D* KTFrequencySpectrumFFTW::CreatePhaseHistogram(const std::string& name) const
    {
        UInt_t nBins = size();
        TH1D* hist = new TH1D(name.c_str(), "Frequency Spectrum: Phase", (Int_t)nBins, GetRangeMin(), GetRangeMax());
        for (UInt_t iBin=0; iBin<nBins; iBin++)
        {
            // order matters, so use (*this)() to access values
            hist->SetBinContent((Int_t)iBin+1, std::atan2((*this)(iBin)[1], (*this)(iBin)[0]));
        }
        hist->SetXTitle("Frequency (Hz)");
        hist->SetYTitle("Phase");
        return hist;
    }

    TH1D* KTFrequencySpectrumFFTW::CreatePowerHistogram(const std::string& name) const
    {
        UInt_t nBins = size();
        TH1D* hist = new TH1D(name.c_str(), "Power Spectrum", (Int_t)nBins, GetRangeMin(), GetRangeMax());
        Double_t value;
        Double_t scaling = 1. / KTPowerSpectrum::GetResistance();
        for (UInt_t iBin=0; iBin<nBins; iBin++)
        {
            // order matters, so use (*this)() to access values
            value = (*this)(iBin)[0] * (*this)(iBin)[0] + (*this)(iBin)[1] * (*this)(iBin)[1];
            hist->SetBinContent((Int_t)iBin + 1, value * scaling);
        }
        hist->SetXTitle("Frequency (Hz)");
        hist->SetYTitle("Power (W)");
        return hist;
    }

    TH1D* KTFrequencySpectrumFFTW::CreatePowerDistributionHistogram(const std::string& name) const
    {
        UInt_t nBins = size();
        Double_t tMaxMag = -1.;
        Double_t tMinMag = 1.e9;
        Double_t value;
        Double_t scaling = 1. / KTPowerSpectrum::GetResistance();
        for (UInt_t iBin=0; iBin<nBins; iBin++)
        {
            // order doesn't matter, so use fData[iBin] to access values
            value = (fData[iBin][0] * fData[iBin][0] + fData[iBin][1] * fData[iBin][1]) * scaling;
            if (value < tMinMag) tMinMag = value;
            if (value > tMaxMag) tMaxMag = value;
        }
        if (tMinMag < 1. && tMaxMag > 1.) tMinMag = 0.;
        TH1D* hist = new TH1D(name.c_str(), "Power Distribution", 100, tMinMag*0.95, tMaxMag*1.05);
        for (UInt_t iBin=0; iBin<nBins; iBin++)
        {
            // order matters, so use (*this)() to access values
            value = (*this)(iBin)[0] * (*this)(iBin)[0] + (*this)(iBin)[1] * (*this)(iBin)[1];
            hist->Fill(value * scaling);
        }
        hist->SetXTitle("Power (W)");
        return hist;
    }

#endif

} /* namespace Katydid */
