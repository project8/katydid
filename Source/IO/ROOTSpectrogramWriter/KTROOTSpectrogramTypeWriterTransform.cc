/*
 * KTROOTSpectrogramTypeWriterTransform.cc
 *
 *  Created on: Jun 18, 2015
 *      Author: nsoblath
 */

#include "KTROOTSpectrogramTypeWriterTransform.hh"

#include "KTFrequencySpectrumDataPolar.hh"
#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTPowerSpectrumData.hh"

//#include "KTLogger.hh"

using std::vector;

namespace Katydid
{
    //KTLOGGER(publog, "KTROOTSpectrogramTypeWriterTransform");

    static Nymph::KTTIRegistrar< KTROOTSpectrogramTypeWriter, KTROOTSpectrogramTypeWriterTransform > sRSTWTRegistrar;

    KTROOTSpectrogramTypeWriterTransform::KTROOTSpectrogramTypeWriterTransform() :
            KTROOTSpectrogramTypeWriter(),
            //KTTypeWriterTransform()
            fFSPolarBundle("FSPolarSpectrogram"),
            fFSFFTWBundle("FSFFTWSpectrogram"),
            fFSFFTWPhaseBundle("FSFFTWPhaseSpectrogram"),
            fPowerBundle("PowerSpectrogram"),
            fPSDBundle("PSDSpectrogram")
    {
    }

    KTROOTSpectrogramTypeWriterTransform::~KTROOTSpectrogramTypeWriterTransform()
    {
    }

    void KTROOTSpectrogramTypeWriterTransform::OutputSpectrograms()
    {
        if (! fWriter->OpenAndVerifyFile()) return;

        KTDEBUG("calling output each spectrogram set")
        OutputASpectrogramSet(fFSPolarBundle, false);
        OutputASpectrogramSet(fFSFFTWBundle, false);
        OutputASpectrogramSet(fFSFFTWPhaseBundle, false);
        OutputASpectrogramSet(fPowerBundle, false);
        OutputASpectrogramSet(fPSDBundle, false);

        return;
    }

    void KTROOTSpectrogramTypeWriterTransform::ClearSpectrograms()
    {
        ClearASpectrogramSet(fFSPolarBundle);
        ClearASpectrogramSet(fFSFFTWBundle);
        ClearASpectrogramSet(fFSFFTWPhaseBundle);
        ClearASpectrogramSet(fPowerBundle);
        ClearASpectrogramSet(fPSDBundle);
        return;
    }

    void KTROOTSpectrogramTypeWriterTransform::RegisterSlots()
    {
        fWriter->RegisterSlot("fs-polar", this, &KTROOTSpectrogramTypeWriterTransform::AddFrequencySpectrumDataPolar);
        fWriter->RegisterSlot("fs-fftw", this, &KTROOTSpectrogramTypeWriterTransform::AddFrequencySpectrumDataFFTW);
        fWriter->RegisterSlot("fs-fftw-phase", this, &KTROOTSpectrogramTypeWriterTransform::AddFrequencySpectrumPhaseDataFFTW);
        fWriter->RegisterSlot("ps", this, &KTROOTSpectrogramTypeWriterTransform::AddPowerSpectrumData);
        fWriter->RegisterSlot("psd", this, &KTROOTSpectrogramTypeWriterTransform::AddPSDData);
        return;
    }


    //************************
    // Frequency Spectrum Data
    //************************

    void KTROOTSpectrogramTypeWriterTransform::AddFrequencySpectrumDataPolar(Nymph::KTDataPtr data)
    {
        AddFrequencySpectrumDataHelper< KTFrequencySpectrumDataPolar >(data, fFSPolarBundle);
        return;
    }

    void KTROOTSpectrogramTypeWriterTransform::AddFrequencySpectrumDataFFTW(Nymph::KTDataPtr data)
    {
        AddFrequencySpectrumDataHelper< KTFrequencySpectrumDataFFTW >(data, fFSFFTWBundle);
        return;
    }

    void KTROOTSpectrogramTypeWriterTransform::AddFrequencySpectrumPhaseDataFFTW(Nymph::KTDataPtr data)
    {
        AddFrequencySpectrumPhaseDataHelper< KTFrequencySpectrumDataFFTW >(data, fFSFFTWPhaseBundle);
        return;
    }

    //********************
    // Power Spectrum Data
    //********************

    void KTROOTSpectrogramTypeWriterTransform::AddPowerSpectrumData(Nymph::KTDataPtr data)
    {
        AddPowerSpectrumDataCoreHelper< KTPowerSpectrumData >(data, fPowerBundle);
        return;
    }

    void KTROOTSpectrogramTypeWriterTransform::AddPSDData(Nymph::KTDataPtr data)
    {
        AddPowerSpectralDensityDataCoreHelper< KTPowerSpectrumData >(data, fPSDBundle);
        return;
    }

} /* namespace Katydid */
