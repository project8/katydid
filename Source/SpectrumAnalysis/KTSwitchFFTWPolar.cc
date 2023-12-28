/*
 * KTSwitchFFTWPolar.cc
 *
 *  Created on: Mar 19, 2013
 *      Author: nsoblath
 */

#include "KTSwitchFFTWPolar.hh"

#include "KTFrequencySpectrumDataPolar.hh"
#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTFrequencySpectrumFFTW.hh"
#include "KTFrequencySpectrumPolar.hh"
#include "KTNormalizedFSData.hh"
#include "KTNormalizedFSData.hh"
#include "KTWignerVilleData.hh"

using std::string;


namespace Katydid
{
    LOGGER(swlog, "KTSwitchFFTWPolar");

    KT_REGISTER_PROCESSOR(KTSwitchFFTWPolar, "switch-fftw-polar");

    KTSwitchFFTWPolar::KTSwitchFFTWPolar(const std::string& name) :
            KTProcessor(name),
            fFSPolarSignal("fs-polar", this),
            //fFSFFTWSignal("fs-fftw", this),
            fFSFFTWSlot("fs-fftw", this, &KTSwitchFFTWPolar::SwitchToPolar, &fFSPolarSignal),
            fNormFSFFTWSlot("norm-fs-fftw", this, &KTSwitchFFTWPolar::SwitchToPolar, &fFSPolarSignal),
            fWignerVilleSlot("wv", this, &KTSwitchFFTWPolar::SwitchToPolar, &fFSPolarSignal)
    {
    }

    KTSwitchFFTWPolar::~KTSwitchFFTWPolar()
    {
    }

    bool KTSwitchFFTWPolar::Configure(const scarab::param_node* node)
    {
        return true;
    }


    bool KTSwitchFFTWPolar::SwitchToPolar(KTFrequencySpectrumDataFFTW& fsData)
    {
        unsigned nComponents = fsData.GetNComponents();

        KTFrequencySpectrumDataPolar& newData = fsData.Of< KTFrequencySpectrumDataPolar >().SetNComponents(nComponents);

        for (unsigned iComponent=0; iComponent<nComponents; ++iComponent)
        {
            KTFrequencySpectrumPolar* newSpectrum = fsData.GetSpectrumFFTW(iComponent)->CreateFrequencySpectrumPolar();
            if (newSpectrum == NULL)
            {
                LERROR(swlog, "Switch of spectrum " << iComponent << " (fftw->polar) failed for some reason. Continuing processing.");
                continue;
            }
            else
            {
                KTFrequencySpectrumFFTW* oldSpectrum = fsData.GetSpectrumFFTW(iComponent);
                LDEBUG(swlog, "fftw array: " << oldSpectrum->size() << " bins; range: " << oldSpectrum->GetRangeMin() << " - " << oldSpectrum->GetRangeMax());
                LDEBUG(swlog, "polar array: " << newSpectrum->size() << " bins; range: " << newSpectrum->GetRangeMin() << " - " << newSpectrum->GetRangeMax());
            }
            newData.SetSpectrum(newSpectrum, iComponent);
        }
        LINFO(swlog, "Completed switch (fftw->polar) of " << nComponents << " frequency spectra (polar)");

        return true;
    }

    bool KTSwitchFFTWPolar::SwitchToPolar(KTNormalizedFSDataFFTW& fsData)
    {
        unsigned nComponents = fsData.GetNComponents();

        KTFrequencySpectrumDataPolar& newData = fsData.Of< KTFrequencySpectrumDataPolar >().SetNComponents(nComponents);

        for (unsigned iComponent=0; iComponent<nComponents; ++iComponent)
        {
            KTFrequencySpectrumPolar* newSpectrum = fsData.GetSpectrumFFTW(iComponent)->CreateFrequencySpectrumPolar();
            if (newSpectrum == NULL)
            {
                LERROR(swlog, "Switch of spectrum " << iComponent << " (fftw->polar) failed for some reason. Continuing processing.");
                continue;
            }
            else
            {
                KTFrequencySpectrumFFTW* oldSpectrum = fsData.GetSpectrumFFTW(iComponent);
                LDEBUG(swlog, "fftw array: " << oldSpectrum->size() << " bins; range: " << oldSpectrum->GetRangeMin() << " - " << oldSpectrum->GetRangeMax());
                LDEBUG(swlog, "polar array: " << newSpectrum->size() << " bins; range: " << newSpectrum->GetRangeMin() << " - " << newSpectrum->GetRangeMax());
            }
            newData.SetSpectrum(newSpectrum, iComponent);
        }
        LINFO(swlog, "Completed switch (fftw->polar) of " << nComponents << " frequency spectra (polar)");

        return true;
    }

    bool KTSwitchFFTWPolar::SwitchToPolar(KTWignerVilleData& fsData)
    {
        unsigned nComponents = fsData.GetNComponents();

        KTFrequencySpectrumDataPolar& newData = fsData.Of< KTFrequencySpectrumDataPolar >().SetNComponents(nComponents);

        for (unsigned iComponent=0; iComponent<nComponents; ++iComponent)
        {
            KTFrequencySpectrumPolar* newSpectrum = fsData.GetSpectrumFFTW(iComponent)->CreateFrequencySpectrumPolar();
            if (newSpectrum == NULL)
            {
                LERROR(swlog, "Switch of spectrum " << iComponent << " (fftw->polar) failed for some reason. Continuing processing.");
                continue;
            }
            else
            {
                KTFrequencySpectrumFFTW* oldSpectrum = fsData.GetSpectrumFFTW(iComponent);
                LDEBUG(swlog, "fftw array: " << oldSpectrum->size() << " bins; range: " << oldSpectrum->GetRangeMin() << " - " << oldSpectrum->GetRangeMax());
                LDEBUG(swlog, "polar array: " << newSpectrum->size() << " bins; range: " << newSpectrum->GetRangeMin() << " - " << newSpectrum->GetRangeMax());
            }
            newData.SetSpectrum(newSpectrum, iComponent);
        }
        LINFO(swlog, "Completed switch (fftw->polar) of " << nComponents << " frequency spectra (polar)");

        return true;
    }

} /* namespace Katydid */
