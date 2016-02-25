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

#include "TLine.h"

//#include "KTLogger.hh"

using std::vector;

namespace Katydid
{
    //KTLOGGER(publog, "KTROOTSpectrogramTypeWriterTransform");

    static KTTIRegistrar< KTROOTSpectrogramTypeWriter, KTROOTSpectrogramTypeWriterTransform > sRSTWTRegistrar;

    KTROOTSpectrogramTypeWriterTransform::KTROOTSpectrogramTypeWriterTransform() :
            KTROOTSpectrogramTypeWriter(),
            //KTTypeWriterTransform()
            fFSPolarSpectrograms(),
            fFSFFTWSpectrograms(),
            fPowerSpectrograms(),
            fPSDSpectrograms(),
            fLineCollection()
    {
    }

    KTROOTSpectrogramTypeWriterTransform::~KTROOTSpectrogramTypeWriterTransform()
    {
        delete fLineCollection;
    }

    void KTROOTSpectrogramTypeWriterTransform::OutputASpectrogramSet(vector< SpectrogramData >& aSpectrogramSet)
    {
        // this function does not check the root file; it's assumed to be opened and verified already
        while (! aSpectrogramSet.empty())
        {
            TH2D* spectrogram = aSpectrogramSet.back().fSpectrogram;
            spectrogram->SetDirectory(fWriter->GetFile());
            spectrogram->Write();
            aSpectrogramSet.pop_back();
        }
        return;
    }

    void KTROOTSpectrogramTypeWriterTransform::ClearASpectrogramSet(vector< SpectrogramData >& aSpectrogramSet)
    {
        while (! aSpectrogramSet.empty())
        {
            delete aSpectrogramSet.back().fSpectrogram;
            aSpectrogramSet.pop_back();
        }
        return;
    }

    void KTROOTSpectrogramTypeWriterTransform::ClearLines()
    {
        delete fLineCollection;
        fLineCollection = NULL;
    }

    void KTROOTSpectrogramTypeWriterTransform::OutputSpectrograms()
    {
        if (! fWriter->OpenAndVerifyFile()) return;

        KTDEBUG("calling output each spectrogram set")
        OutputASpectrogramSet(fFSPolarSpectrograms);
        OutputASpectrogramSet(fFSFFTWSpectrograms);
        OutputASpectrogramSet(fPowerSpectrograms);
        OutputASpectrogramSet(fPSDSpectrograms);

        KTINFO("calling output lines")
        OutputLines();

        return;
    }

    void KTROOTSpectrogramTypeWriterTransform::OutputLines()
    {
        KTINFO("grab file and cd")
        fWriter->GetFile()->WriteTObject(fLineCollection, "AllLines", "SingleKey");
        fLineCollection = NULL;
        return;
    }

    void KTROOTSpectrogramTypeWriterTransform::ClearSpectrograms()
    {
        ClearASpectrogramSet(fFSPolarSpectrograms);
        ClearASpectrogramSet(fFSFFTWSpectrograms);
        ClearASpectrogramSet(fPowerSpectrograms);
        ClearASpectrogramSet(fPSDSpectrograms);

        ClearLines();
        return;
    }

    void KTROOTSpectrogramTypeWriterTransform::RegisterSlots()
    {
        fWriter->RegisterSlot("fs-polar", this, &KTROOTSpectrogramTypeWriterTransform::AddFrequencySpectrumDataPolar);
        fWriter->RegisterSlot("fs-fftw", this, &KTROOTSpectrogramTypeWriterTransform::AddFrequencySpectrumDataFFTW);
        fWriter->RegisterSlot("ps", this, &KTROOTSpectrogramTypeWriterTransform::AddPowerSpectrumData);
        fWriter->RegisterSlot("psd", this, &KTROOTSpectrogramTypeWriterTransform::AddPSDData);
        fWriter->RegisterSlot("all-lines", this, &KTROOTSpectrogramTypeWriterTransform::TakeLine);
        //fWriter->RegisterSlot("write-file", this, &KTROOTSpectrogramTypeWriterTransform::WriteFile);
        return;
    }


    //************************
    // Frequency Spectrum Data
    //************************

    void KTROOTSpectrogramTypeWriterTransform::AddFrequencySpectrumDataPolar(KTDataPtr data)
    {
        AddFrequencySpectrumDataHelper< KTFrequencySpectrumDataPolar >(data, fFSPolarSpectrograms, "FSPolarSpectrogram_");
        return;
    }

    void KTROOTSpectrogramTypeWriterTransform::AddFrequencySpectrumDataFFTW(KTDataPtr data)
    {
        AddFrequencySpectrumDataHelper< KTFrequencySpectrumDataFFTW >(data, fFSFFTWSpectrograms, "FSFFTWSpectrogram_");
        return;
    }

    //********************
    // Power Spectrum Data
    //********************

    void KTROOTSpectrogramTypeWriterTransform::AddPowerSpectrumData(KTDataPtr data)
    {
        AddPowerSpectrumDataCoreHelper< KTPowerSpectrumData >(data, fPowerSpectrograms, "PowerSpectrogram_");
        return;
    }

    void KTROOTSpectrogramTypeWriterTransform::AddPSDData(KTDataPtr data)
    {
        AddPowerSpectralDensityDataCoreHelper< KTPowerSpectrumData >(data, fPSDSpectrograms, "PSDSpectrogram_");
        return;
    }

    void KTROOTSpectrogramTypeWriterTransform::TakeLine(KTDataPtr data)
    {
        if (fLineCollection == NULL)
        {
            fLineCollection = new TOrdCollection();
        }
        KTProcessedTrackData thisLine = data->Of< KTProcessedTrackData >();
        TLine* rootLine = new TLine(thisLine.GetStartTimeInRunC(), thisLine.GetStartFrequency(), thisLine.GetEndTimeInRunC(), thisLine.GetEndFrequency());
        fLineCollection->Add(rootLine);
    }

} /* namespace Katydid */
