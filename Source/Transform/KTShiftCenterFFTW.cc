/*
 * KTShiftCenterFFTW.cc
 *
 *  Created on: Dec 1, 2014
 *      Author: L. de Viveiros
 */

#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTFrequencySpectrumFFTW.hh"
#include "KTLogger.hh"
#include "KTMath.hh"
#include "KTParam.hh"
#include "KTEggHeader.hh"
#include "KTShiftCenterFFTW.hh"


namespace Katydid
{
    KTLOGGER(fftlog_comp, "KTShiftCenterFFTW");

    // Register the processor
    KT_REGISTER_PROCESSOR(KTShiftCenterFFTW, "shiftcenter-fftw");

    KTShiftCenterFFTW::KTShiftCenterFFTW(const std::string& name) :
            KTProcessor(name),
            fRSAMinimumFrequency(0.),
            fRSAMaximumFrequency(0.),
            fMinimumFrequency(0.),
            fMaximumFrequency(0.),
            fFSFFTWSignal("fs-fftw-out", this),
            fHeaderSlot("header", this, &KTShiftCenterFFTW::InitializeWithHeader),
            fFSFFTWSlot("fs-fftw-in", this, &KTShiftCenterFFTW::ShiftCenterFFTW, &fFSFFTWSignal)
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

    bool KTShiftCenterFFTW::InitializeWithHeader(KTEggHeader& header)
    {
        SetRSAMinimumFrequency(header.GetMinimumFrequency());
        SetRSAMaximumFrequency(header.GetMaximumFrequency());
        return true;
    }


    bool KTShiftCenterFFTW::ShiftCenterFFTW(KTFrequencySpectrumDataFFTW& fsData)
    {

        unsigned nComponents = fsData.GetNComponents();
        double fDCBinLeftEdge, fSpan, fFreqBinWidth;


        KTFrequencySpectrumDataFFTW& newData = fsData.Of< KTFrequencySpectrumDataFFTW >().SetNComponents(nComponents);

        for (unsigned iComponent=0; iComponent<nComponents; ++iComponent)
        {
            fFreqBinWidth = fsData.GetSpectrumFFTW(iComponent)->GetBinWidth();
            fDCBinLeftEdge = -0.5 * fFreqBinWidth;
            fSpan = fRSAMaximumFrequency - fRSAMinimumFrequency;
            SetMinimumFrequency(fDCBinLeftEdge);
            SetMaximumFrequency(fDCBinLeftEdge + fSpan);

            KTFrequencySpectrumFFTW* newSpectrum = ShiftCenterFFTW(fsData.GetSpectrumFFTW(iComponent));
            if (newSpectrum == NULL)
            {
                KTERROR(fftlog_comp, "Shift Center of spectrum " << iComponent << " failed for some reason. Continuing processing.");
                continue;
            }
            newData.SetSpectrum(newSpectrum, iComponent);
        }
        KTINFO(fftlog_comp, "Completed ShiftCenterFFTW of " << nComponents << " frequency spectra (FFTW)");

        return true;
    }


    KTFrequencySpectrumFFTW* KTShiftCenterFFTW::ShiftCenterFFTW(const KTFrequencySpectrumFFTW* frequencySpectrum) const
    {
        // Read the Header Information
        //fCenterFrequency = GetCenterFrequency();
        //fMinimumFrequency = GetMinimumFrequency();
        //fMaximumFrequency = GetMaximumFrequency();

        // Shift Spectrum
        KTDEBUG(fftlog_comp, "Creating new FS for shifted data");
        unsigned nBins = frequencySpectrum->size();
        KTFrequencySpectrumFFTW* newSpectrum = new KTFrequencySpectrumFFTW(nBins, fMinimumFrequency, fMaximumFrequency);
        newSpectrum->SetNTimeBins(frequencySpectrum->GetNTimeBins());

        for (unsigned iBin = 0; iBin < nBins; ++iBin)
        {
            (*newSpectrum)(iBin)[0] = (*frequencySpectrum)(iBin)[0];
            (*newSpectrum)(iBin)[1] = (*frequencySpectrum)(iBin)[1];
            // newSpectrum->SetBinCenter(iBin) = frequencySpectrum->GetBinCenter(iBin) + fCenterFrequency;
        }

        KTDEBUG(fftlog_comp, "Shifted Center of Spectrum to [ " << fMinimumFrequency << " - " << fMaximumFrequency << " ] Hz" );

        return newSpectrum;
    }


} /* namespace Katydid */
