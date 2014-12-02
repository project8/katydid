/*
 * KTLowPassFilter.cc
 *
 *  Created on: Nov 3, 2014
 *      Author: N.S. Oblath
 */

#include "KTLowPassFilter.hh"

#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTFrequencySpectrumFFTW.hh"
#include "KTLogger.hh"
#include "KTLowPassFilteredData.hh"
#include "KTMath.hh"
#include "KTParam.hh"
#include "KTPowerSpectrum.hh"
#include "KTPowerSpectrumData.hh"


namespace Katydid
{
    KTLOGGER(fftlog_comp, "KTShiftCenterFFTW");

    // Register the processor
    KT_REGISTER_PROCESSOR(KTShiftCenterFFTW, "shiftcenter-fftw");

    KTShiftCenterFFTW::KTShiftCenterFFTW(const std::string& name) :
            KTProcessor(name),
            fFSFFTWSignal("fs-fftw", this),
            fFSFFTWSlot("fs-fftw", this, &KTLowPassFilter::Filter, &fFSFFTWSignal)
    {
    }

    KTShiftCenterFFTW::~KTShiftCenterFFTW()
    {
    }

    bool KTShiftCenterFFTW::Configure(const KTParamNode* node)
    {
        if (node == NULL) return false;

        return true;
    }


    bool KTShiftCenterFFTW::ShiftCenterFFTW(KTFrequencySpectrumDataFFTW& fsData)
    {
        unsigned nComponents = fsData.GetNComponents();
        KTFrequencySpectrumDataFFTW& newData = fsData.Of< KTFrequencySpectrumDataFFTW >().SetNComponents(nComponents);

        for (unsigned iComponent=0; iComponent<nComponents; ++iComponent)
        {
            KTFrequencySpectrumFFTW* newSpectrum = ShiftCenterFFTW(fsData.GetSpectrumFFTW(iComponent));
            if (newSpectrum == NULL)
            {
                KTERROR(fftlog_comp, "Shift Center of spectrum " << iComponent << " failed for some reason. Continuing processing.");
                continue;
            }
            KTDEBUG(fftlog_comp, "Shifted Center of Spectrum");
            newData.SetSpectrum(newSpectrum, iComponent);
        }
        KTINFO(fftlog_comp, "Completed ShiftCenterFFTW of " << nComponents << " frequency spectra (FFTW)");

        return true;
    }


    KTFrequencySpectrumFFTW* KTLowPassFilter::ShiftCenterFFTW(const KTFrequencySpectrumFFTW* frequencySpectrum) const
    {
        // Read the Header Information
        fSpanCenterFreq = fHeader.GetCenterFrequency();
        fMinFreq = fHeader.GetMinimumFrequency();
        fMaxFreq = fHeader.GetMaximumFrequency();

        // Shift Spectrum
        KTDEBUG(fftlog_comp, "Creating new FS for shifted data");
        unsigned nBins = frequencySpectrum->size();
        KTFrequencySpectrumFFTW* newSpectrum = new KTFrequencySpectrumFFTW(nBins, fMinFreq, fMaxFreq);
        newSpectrum->SetNTimeBins(frequencySpectrum->GetNTimeBins());

        for (unsigned iBin = 0; iBin < nBins; ++iBin)
        {
            // newSpectrum->SetBinCenter(iBin) = frequencySpectrum->GetBinCenter(iBin) + fCenterFrequency;
        }

        return newSpectrum;
    }


} /* namespace Katydid */
