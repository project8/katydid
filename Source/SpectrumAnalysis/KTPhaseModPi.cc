/*
 * KTLowPassFilter.cc
 *
 *  Created on: Nov 3, 2014
 *      Author: N.S. Oblath
 */

#include "KTLowPassFilter.hh"

#include "KTFrequencySpectrumDataPolar.hh"
#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTFrequencySpectrumFFTW.hh"
#include "KTFrequencySpectrumPolar.hh"
#include "KTLogger.hh"
#include "KTLowPassFilteredData.hh"
#include "KTMath.hh"
#include "KTPowerSpectrum.hh"
#include "KTPowerSpectrumData.hh"

#include "param.hh"


namespace Katydid
{
    KTLOGGER(gclog, "KTLowPassFilter");

    // Register the processor
    KT_REGISTER_PROCESSOR(KTLowPassFilter, "low-pass-filter");

    KTLowPassFilter::KTLowPassFilter(const std::string& name) :
            KTProcessor(name),
            fRC(0.),
            fFSPolarSignal("fs-polar", this),
            fFSFFTWSignal("fs-fftw", this),
            fPSSignal("ps", this),
            fFSPolarSlot("fs-polar", this, &KTLowPassFilter::Filter, &fFSPolarSignal),
            fFSFFTWSlot("fs-fftw", this, &KTLowPassFilter::Filter, &fFSFFTWSignal),
            fPSSlot("ps", this, &KTLowPassFilter::Filter, &fPSSignal)
    {
    }

    KTLowPassFilter::~KTLowPassFilter()
    {
    }

    bool KTLowPassFilter::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return false;

        SetRC(node->get_value("rc", GetRC()));

        return true;
    }

    bool KTLowPassFilter::Filter(KTFrequencySpectrumDataPolar& fsData)
    {
        unsigned nComponents = fsData.GetNComponents();
        KTLowPassFilteredFSDataPolar& newData = fsData.Of< KTLowPassFilteredFSDataPolar >().SetNComponents(nComponents);

        for (unsigned iComponent=0; iComponent<nComponents; ++iComponent)
        {
            KTFrequencySpectrumPolar* newSpectrum = Filter(fsData.GetSpectrumPolar(iComponent));
            if (newSpectrum == NULL)
            {
                KTERROR(gclog, "Low-pass filter of spectrum " << iComponent << " failed for some reason. Continuing processing.");
                continue;
            }
            KTDEBUG(gclog, "Computed low-pass filter");
            newData.SetSpectrum(newSpectrum, iComponent);
        }
        KTINFO(gclog, "Completed low-pass filter of " << nComponents << " frequency spectra (polar)");

        return true;
    }

    bool KTLowPassFilter::Filter(KTFrequencySpectrumDataFFTW& fsData)
    {
        unsigned nComponents = fsData.GetNComponents();
        KTLowPassFilteredFSDataFFTW& newData = fsData.Of< KTLowPassFilteredFSDataFFTW >().SetNComponents(nComponents);

        for (unsigned iComponent=0; iComponent<nComponents; ++iComponent)
        {
            KTFrequencySpectrumFFTW* newSpectrum = Filter(fsData.GetSpectrumFFTW(iComponent));
            if (newSpectrum == NULL)
            {
                KTERROR(gclog, "Low-pass filter of spectrum " << iComponent << " failed for some reason. Continuing processing.");
                continue;
            }
            KTDEBUG(gclog, "Computed low-pass filter");
            newData.SetSpectrum(newSpectrum, iComponent);
        }
        KTINFO(gclog, "Completed low-pass filter of " << nComponents << " frequency spectra (FFTW)");

        return true;
    }

    bool KTLowPassFilter::Filter(KTPowerSpectrumData& psData)
    {
        unsigned nComponents = psData.GetNComponents();
        KTLowPassFilteredPSData& newData = psData.Of< KTLowPassFilteredPSData >().SetNComponents(nComponents);

        for (unsigned iComponent=0; iComponent<nComponents; ++iComponent)
        {
            KTPowerSpectrum* newSpectrum = Filter(psData.GetSpectrum(iComponent));
            if (newSpectrum == NULL)
            {
                KTERROR(gclog, "Low-pass filter of spectrum " << iComponent << " failed for some reason. Continuing processing.");
                continue;
            }
            KTDEBUG(gclog, "Computed low-pass filter");
            newData.SetSpectrum(newSpectrum, iComponent);
        }
        KTINFO(gclog, "Completed low-pass filter of " << nComponents << " power spectra");

        return true;
    }

    KTFrequencySpectrumPolar* KTLowPassFilter::Filter(const KTFrequencySpectrumPolar* frequencySpectrum) const
    {
        KTDEBUG(gclog, "Creating new FS for filtered data");
        unsigned nBins = frequencySpectrum->size();
        KTFrequencySpectrumPolar* newSpectrum = new KTFrequencySpectrumPolar(nBins, frequencySpectrum->GetRangeMin(), frequencySpectrum->GetRangeMax());
        newSpectrum->SetNTimeBins(frequencySpectrum->GetNTimeBins());

        double twoPiRCf, abs, arg;
        for (unsigned iBin = 0; iBin < nBins; ++iBin)
        {
            abs = (*frequencySpectrum)(iBin).abs();
            arg = (*frequencySpectrum)(iBin).arg();
            twoPiRCf = KTMath::TwoPi() * fRC * frequencySpectrum->GetBinCenter(iBin);
            (*newSpectrum)(iBin).set_polar(abs / sqrt(1 + twoPiRCf*twoPiRCf), arg - atan(twoPiRCf));
        }

        return newSpectrum;
    }

    KTFrequencySpectrumFFTW* KTLowPassFilter::Filter(const KTFrequencySpectrumFFTW* frequencySpectrum) const
    {
        KTDEBUG(gclog, "Creating new FS for filtered data");
        unsigned nBins = frequencySpectrum->size();
        KTFrequencySpectrumFFTW* newSpectrum = new KTFrequencySpectrumFFTW(nBins, frequencySpectrum->GetRangeMin(), frequencySpectrum->GetRangeMax());
        newSpectrum->SetNTimeBins(frequencySpectrum->GetNTimeBins());

        double twoPiRCf, real, imag, denom, newReal, newImag;
        for (unsigned iBin = 0; iBin < nBins; ++iBin)
        {
            real = (*frequencySpectrum)(iBin)[0];
            imag = (*frequencySpectrum)(iBin)[1];
            twoPiRCf = KTMath::TwoPi() * fRC * frequencySpectrum->GetBinCenter(iBin);
            denom = 1 + twoPiRCf * twoPiRCf;
            (*newSpectrum)(iBin)[0] = (real + imag * twoPiRCf) / denom;
            (*newSpectrum)(iBin)[1] = (imag - real * twoPiRCf) / denom;
        }

        return newSpectrum;
    }

    KTPowerSpectrum* KTLowPassFilter::Filter(const KTPowerSpectrum* powerSpectrum) const
    {
        KTDEBUG(gclog, "Creating new PS for filtered data");
        unsigned nBins = powerSpectrum->size();
        KTPowerSpectrum* newSpectrum = new KTPowerSpectrum(nBins, powerSpectrum->GetRangeMin(), powerSpectrum->GetRangeMax());

        double twoPiRCf;
        for (unsigned iBin = 0; iBin < nBins; ++iBin)
        {
            twoPiRCf = KTMath::TwoPi() * fRC * powerSpectrum->GetBinCenter(iBin);
            (*newSpectrum)(iBin) = (*powerSpectrum)(iBin) / (1 + twoPiRCf * twoPiRCf);
        }

        return newSpectrum;
    }

} /* namespace Katydid */
