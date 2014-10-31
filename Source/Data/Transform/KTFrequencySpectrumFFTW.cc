/*
 * KTFrequencySpectrumFFTW.cc
 *
 *  Created on: Aug 28, 2012
 *      Author: nsoblath
 */

#include "KTFrequencySpectrumFFTW.hh"

#include "KTLogger.hh"
#include "KTPowerSpectrum.hh"
#include "KTFrequencySpectrumPolar.hh"

#ifdef ROOT_FOUND
#include "TH1.h"
#endif

#include <sstream>

#ifdef USE_OPENMP
#include <omp.h>
#endif

using std::stringstream;

namespace Katydid
{
    KTLOGGER(fslog, "KTFrequencySpectrumFFTW");

    KTFrequencySpectrumFFTW::KTFrequencySpectrumFFTW() :
            KTPhysicalArray< 1, fftw_complex >(),
            KTFrequencySpectrum(),
            fIsSizeEven(true),
            fNegFreqOffset(0),
            fDCBin(0),
            fNTimeBins(0),
            fPointCache()
    {
    }

    KTFrequencySpectrumFFTW::KTFrequencySpectrumFFTW(size_t nBins, double rangeMin, double rangeMax) :
            KTPhysicalArray< 1, fftw_complex >(nBins, rangeMin, rangeMax),
            KTFrequencySpectrum(),
            fIsSizeEven(nBins%2 == 0),
            fNegFreqOffset((nBins+1)/2),
            fDCBin(nBins/2),
            fNTimeBins(0),
            fPointCache()
    {
        //KTINFO(fslog, "number of bins: " << nBins << "   is size even? " << fIsSizeEven);
        //KTINFO(fslog, "neg freq offset: " << fNegFreqOffset);
    }

    KTFrequencySpectrumFFTW::KTFrequencySpectrumFFTW(const KTFrequencySpectrumFFTW& orig) :
            KTPhysicalArray< 1, fftw_complex >(orig),
            KTFrequencySpectrum(),
            fIsSizeEven(orig.fIsSizeEven),
            fNegFreqOffset(orig.fNegFreqOffset),
            fDCBin(orig.fDCBin),
            fNTimeBins(orig.fNTimeBins),
            fPointCache()
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
        fNTimeBins = rhs.fNTimeBins;
        return *this;
    }

    KTFrequencySpectrumFFTW& KTFrequencySpectrumFFTW::CConjugate()
    {
        unsigned nBins = size();
#pragma omp parallel for
        for (unsigned iBin=0; iBin<nBins; ++iBin)
        {
            // order doesn't matter, so use fData[] to access values
            fData[iBin][1] = -fData[iBin][1];
        }
        return *this;
    }

    KTFrequencySpectrumFFTW& KTFrequencySpectrumFFTW::AnalyticAssociate()
    {
        // This is only valid if the original signal is Real only (not complex)
        // Note: the data storage array is accessed directly, so the FFTW data storage format is used.
        // Nyquist bin(s) and negative frequency bins are set to 0 (from size/2 to the end of the array)
        // DC bin stays as is (array position 0).
        // Positive frequency bins are multiplied by 2 (from array position 1 to size/2).
        unsigned nBins = size();
        unsigned nyquistPos = nBins / 2; // either the sole nyquist bin (if even # of bins) or the first of the two (if odd # of bins; bins are sequential in the array).
#pragma omp parallel for
        for (unsigned arrayPos=1; arrayPos<nyquistPos; arrayPos++)
        {
            fData[arrayPos][0] = fData[arrayPos][0] * 2.;
            fData[arrayPos][1] = fData[arrayPos][1] * 2.;
        }
#pragma omp parallel for
        for (unsigned arrayPos=nyquistPos; arrayPos<nBins; arrayPos++)
        {
            fData[arrayPos][0] = 0.;
            fData[arrayPos][1] = 0.;
        }
        return *this;
    }

    KTFrequencySpectrumFFTW& KTFrequencySpectrumFFTW::Scale(double scale)
    {
        unsigned nBins = size();
#pragma omp parallel for
        for (unsigned iBin=0; iBin<nBins; ++iBin)
        {
            // order doesn't matter, so use fData[] to access values
            fData[iBin][0] = scale * fData[iBin][0];
            fData[iBin][1] = scale * fData[iBin][1];
        }
        return *this;
    }


    KTFrequencySpectrumPolar* KTFrequencySpectrumFFTW::CreateFrequencySpectrumPolar(bool addNegFreqs) const
    {
        // The negative frequency values will be combined with the positive ones,
        // so the power spectrum will go from the DC bin to the max frequency

        unsigned nBins = fDCBin + 1;
        KTFrequencySpectrumPolar* newFS = new KTFrequencySpectrumPolar(nBins, -0.5 * GetBinWidth(), GetRangeMax());
        newFS->SetNTimeBins(fNTimeBins);

        // DC bin
        (*newFS)(0).set_rect((*this)(fDCBin)[0], (*this)(fDCBin)[1]);

        // All bins besides the Nyquist and DC bins
        unsigned totalBins = size();
        /*
        unsigned iPosBin = fDCBin + 1;
        unsigned iNegBin = fDCBin - 1;
        for (unsigned iBin=1; iBin<nBins-1; ++iBin)
        {
            //std::cout << iBin << "  " << iPosBin << "  " << iNegBin << std::endl;
            // order matters, so use (*this)() to access values
            valueReal = (*this)(iNegBin)[0] + (*this)(iPosBin)[0];
            valueImag = (*this)(iNegBin)[1] + (*this)(iPosBin)[1];
            (*newFS)(iBin).set_rect(valueReal, valueImag);
            iPosBin++;
            iNegBin--;
        }
         */
        // he negative and positive frequencies must be added in quadrature
        if (addNegFreqs)
        {
            double valueImag, valueReal;
#pragma omp parallel for private(valueReal, valueImag)
            for (unsigned iBin=1; iBin<nBins-1; ++iBin)
            {
                valueReal = sqrt( ((*this)(fDCBin - iBin)[0]*(*this)(fDCBin - iBin)[0]) + ((*this)(fDCBin + iBin)[0]*(*this)(fDCBin + iBin)[0]) );
                valueImag = sqrt( ((*this)(fDCBin - iBin)[1]*(*this)(fDCBin - iBin)[1]) + ((*this)(fDCBin + iBin)[1]*(*this)(fDCBin + iBin)[1]) );
                (*newFS)(iBin).set_rect(valueReal, valueImag);
            }

            // Nyquist bin
            if (fIsSizeEven)
            {
                (*newFS)(nBins-1).set_rect((*this)(0)[0], (*this)(0)[1]);
            }
            else
            {
                valueReal = sqrt( ((*this)(0)[0]*(*this)(0)[0]) + ((*this)(totalBins-1)[0]*(*this)(totalBins-1)[0]) );
                valueImag = sqrt( ((*this)(0)[1]*(*this)(0)[1]) + ((*this)(totalBins-1)[1]*(*this)(totalBins-1)[1]) );
                (*newFS)(nBins-1).set_rect(valueReal, valueImag);
            }
        }
        else
        {
#pragma omp parallel for
            for (unsigned iBin=1; iBin<nBins-1; ++iBin)
            {
                (*newFS)(iBin).set_rect((*this)(fDCBin + iBin)[0], (*this)(fDCBin + iBin)[1]);
            }

            // Nyquist bin
            if (fIsSizeEven)
            {
                // in the event of even size, this is the only nyquist bin, so i have to use it, even though it's frequency is negative
                (*newFS)(nBins-1).set_rect((*this)(0)[0], (*this)(0)[1]);
            }
            else
            {
                (*newFS)(nBins-1).set_rect((*this)(totalBins-1)[0], (*this)(totalBins-1)[1]);
            }
        }

        return newFS;
    }

    KTPowerSpectrum* KTFrequencySpectrumFFTW::CreatePowerSpectrum() const
    {
        // The negative frequency values will be combined with the positive ones,
        // so the power spectrum will go from the DC bin to the max frequency

        // hmm, hardcoded for now
        bool addNegFreqs = true;

        unsigned nBins = fDCBin + 1;
        KTPowerSpectrum* newPS = new KTPowerSpectrum(nBins, -0.5 * GetBinWidth(), GetRangeMax());

        double valueImag, valueReal;
        double scaling = 1. / KTPowerSpectrum::GetResistance() / (double)GetNTimeBins();

        // DC bin
        valueReal = (*this)(fDCBin)[0];
        valueImag = (*this)(fDCBin)[1];
        (*newPS)(0) = (valueReal * valueReal + valueImag * valueImag) * scaling;

        // All bins besides the Nyquist and DC bins
        unsigned totalBins = size();
        /*
        unsigned iPosBin = fDCBin + 1;
        unsigned iNegBin = fDCBin - 1;
        for (unsigned iBin=1; iBin<nBins-1; ++iBin)
        {
            //std::cout << iBin << "  " << iPosBin << "  " << iNegBin << std::endl;
            // order matters, so use (*this)() to access values
            valueReal = (*this)(iNegBin)[0] + (*this)(iPosBin)[0];
            valueImag = (*this)(iNegBin)[1] + (*this)(iPosBin)[1];
            (*newFS)(iBin).set_rect(valueReal, valueImag);
            iPosBin++;
            iNegBin--;
        }
         */
        // the negative and positive frequencies must be added in quadrature
        if (addNegFreqs)
        {
#pragma omp parallel for private(valueReal, valueImag)
            for (unsigned iBin=1; iBin<nBins-1; ++iBin)
            {
                valueReal = sqrt( ((*this)(fDCBin - iBin)[0]*(*this)(fDCBin - iBin)[0]) + ((*this)(fDCBin + iBin)[0]*(*this)(fDCBin + iBin)[0]) );
                valueImag = sqrt( ((*this)(fDCBin - iBin)[1]*(*this)(fDCBin - iBin)[1]) + ((*this)(fDCBin + iBin)[1]*(*this)(fDCBin + iBin)[1]) );
                (*newPS)(iBin) = (valueReal * valueReal + valueImag * valueImag) * scaling;
            }

            // Nyquist bin
            if (fIsSizeEven)
            {
                valueReal = (*this)(0)[0];
                valueImag = (*this)(0)[1];
                (*newPS)(nBins-1) = (valueReal * valueReal + valueImag * valueImag) * scaling;
            }
            else
            {
                valueReal = sqrt( ((*this)(0)[0]*(*this)(0)[0]) + ((*this)(totalBins-1)[0]*(*this)(totalBins-1)[0]) );
                valueImag = sqrt( ((*this)(0)[1]*(*this)(0)[1]) + ((*this)(totalBins-1)[1]*(*this)(totalBins-1)[1]) );
                (*newPS)(nBins-1) = (valueReal * valueReal + valueImag * valueImag) * scaling;
            }
        }
        else
        {
#pragma omp parallel for private(valueReal, valueImag)
            for (unsigned iBin=1; iBin<nBins-1; ++iBin)
            {
                valueReal = (*this)(fDCBin + iBin)[0];
                valueImag = (*this)(fDCBin + iBin)[1];
                (*newPS)(iBin) = (valueReal * valueReal + valueImag * valueImag) * scaling;
            }

            // Nyquist bin
            if (fIsSizeEven)
            {
                // in the event of even size, this is the only nyquist bin, so i have to use it, even though it's frequency is negative
                valueReal = (*this)(0)[0];
                valueImag = (*this)(0)[1];
                (*newPS)(nBins-1) = (valueReal * valueReal + valueImag * valueImag) * scaling;
            }
            else
            {
                valueReal = (*this)(totalBins-1)[0];
                valueImag = (*this)(totalBins-1)[1];
                (*newPS)(nBins-1) = (valueReal * valueReal + valueImag * valueImag) * scaling;
            }
        }

        return newPS;





        // The version below does not add negative frequencies
        /*
        unsigned nBins = size();
        KTPowerSpectrum* newPS = new KTPowerSpectrum(nBins, GetRangeMin(), GetRangeMax());
        double value, valueImag, valueReal;
        double scaling = 1. / KTPowerSpectrum::GetResistance() / (double)GetNTimeBins();

#pragma omp parallel for private(valueReal, valueImag)
        for (unsigned iBin=0; iBin<nBins; ++iBin)
        {
            valueReal = (*this)(iBin)[0];
            valueImag = (*this)(iBin)[1];
            value = valueReal * valueReal + valueImag * valueImag;
            (*newPS)(iBin) = value * scaling;
        }

        return newPS;
        */
    }

    void KTFrequencySpectrumFFTW::Print(unsigned startPrint, unsigned nToPrint) const
    {
        stringstream printStream;
        for (unsigned iBin = startPrint; iBin < startPrint + nToPrint; ++iBin)
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
        unsigned nBins = size();
        TH1D* hist = new TH1D(name.c_str(), "Frequency Spectrum: Magnitude", (int)nBins, GetRangeMin(), GetRangeMax());
        for (unsigned iBin=0; iBin<nBins; ++iBin)
        {
            // order matters, so use (*this)() to access values
            hist->SetBinContent((int)iBin+1, std::sqrt((*this)(iBin)[0] * (*this)(iBin)[0] + (*this)(iBin)[1] * (*this)(iBin)[1]));
        }
        hist->SetXTitle("Frequency (Hz)");
        hist->SetYTitle("Voltage (V)");
        return hist;
    }

    TH1D* KTFrequencySpectrumFFTW::CreatePhaseHistogram(const std::string& name) const
    {
        unsigned nBins = size();
        TH1D* hist = new TH1D(name.c_str(), "Frequency Spectrum: Phase", (int)nBins, GetRangeMin(), GetRangeMax());
        for (unsigned iBin=0; iBin<nBins; ++iBin)
        {
            // order matters, so use (*this)() to access values
            hist->SetBinContent((int)iBin+1, std::atan2((*this)(iBin)[1], (*this)(iBin)[0]));
        }
        hist->SetXTitle("Frequency (Hz)");
        hist->SetYTitle("Phase");
        return hist;
    }

    TH1D* KTFrequencySpectrumFFTW::CreatePowerHistogram(const std::string& name) const
    {
        unsigned nBins = size();
        TH1D* hist = new TH1D(name.c_str(), "Power Spectrum", (int)nBins, GetRangeMin(), GetRangeMax());
        double value, valueImag, valueReal;
        double scaling = 1. / KTPowerSpectrum::GetResistance() / (double)GetNTimeBins();

        for (unsigned iBin=0; iBin<nBins; ++iBin)
        {
            // order matters, so use (*this)() to access values
            hist->SetBinContent((int)iBin+1, scaling * ((*this)(iBin)[0] * (*this)(iBin)[0] + (*this)(iBin)[1] * (*this)(iBin)[1]));
        }

        hist->SetXTitle("Frequency (Hz)");
        hist->SetYTitle("Power (W)");
        return hist;
    }

    TH1D* KTFrequencySpectrumFFTW::CreateMagnitudeDistributionHistogram(const std::string& name) const
    {
        unsigned nBins = size();
        double tMaxMag = -1.;
        double tMinMag = 1.e9;
        double value;
        // skip the DC bin; start at iBin = 1
        for (unsigned iBin=1; iBin<nBins; ++iBin)
        {
            // order doesn't matter, so use fData[iBin] to access values
            value = std::sqrt(fData[iBin][0] * fData[iBin][0] + fData[iBin][1] * fData[iBin][1]);
            if (value < tMinMag) tMinMag = value;
            if (value > tMaxMag) tMaxMag = value;
        }
        if (tMinMag < 1. && tMaxMag > 1.) tMinMag = 0.;
        TH1D* hist = new TH1D(name.c_str(), "Magnitude Distribution", 100, tMinMag*0.95, tMaxMag*1.05);
        for (unsigned iBin=0; iBin<nBins; ++iBin)
        {
            // order matters, so use (*this)() to access values
            value = sqrt((*this)(iBin)[0] * (*this)(iBin)[0] + (*this)(iBin)[1] * (*this)(iBin)[1]);
            hist->Fill(value);
        }
        hist->SetXTitle("Voltage (V)");
        return hist;
    }

    TH1D* KTFrequencySpectrumFFTW::CreatePowerDistributionHistogram(const std::string& name) const
    {
        unsigned nBins = size();
        double tMaxMag = -1.;
        double tMinMag = 1.e9;
        double value;
        double scaling = 1. / KTPowerSpectrum::GetResistance() / (double)GetNTimeBins();
        // skip the DC bin; start at iBin = 1
        for (unsigned iBin=1; iBin<nBins; ++iBin)
        {
            // order doesn't matter, so use fData[iBin] to access values
            value = (fData[iBin][0] * fData[iBin][0] + fData[iBin][1] * fData[iBin][1]) * scaling;
            if (value < tMinMag) tMinMag = value;
            if (value > tMaxMag) tMaxMag = value;
        }
        if (tMinMag < 1. && tMaxMag > 1.) tMinMag = 0.;
        TH1D* hist = new TH1D(name.c_str(), "Power Distribution", 100, tMinMag*0.95, tMaxMag*1.05);
        for (unsigned iBin=0; iBin<nBins; ++iBin)
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
