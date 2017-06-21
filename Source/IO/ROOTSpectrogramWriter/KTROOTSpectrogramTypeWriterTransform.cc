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
#include "KTProcessedTrackData.hh"
#include "KTScoredSpectrumData.hh"

#include "TLine.h"

//#include "KTLogger.hh"

using std::vector;

namespace Katydid
{
    //KTLOGGER(publog, "KTROOTSpectrogramTypeWriterTransform");

    static Nymph::KTTIRegistrar< KTROOTSpectrogramTypeWriter, KTROOTSpectrogramTypeWriterTransform > sRSTWTRegistrar;

    KTROOTSpectrogramTypeWriterTransform::KTROOTSpectrogramTypeWriterTransform() :
            KTROOTSpectrogramTypeWriter(),
            //KTTypeWriterTransform()
            fFSPolarSpectrograms(),
            fFSFFTWSpectrograms(),
            fPowerSpectrograms(),
            fPSDSpectrograms(),
			fScoredSpectrograms(),
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
        OutputASpectrogramSet(fScoredSpectrograms);

        KTDEBUG("calling output lines")
        OutputLines();

        return;
    }

    void KTROOTSpectrogramTypeWriterTransform::OutputLines()
    {
        KTDEBUG("grab file and cd")
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
        ClearASpectrogramSet(fScoredSpectrograms);

        ClearLines();
        return;
    }

    void KTROOTSpectrogramTypeWriterTransform::RegisterSlots()
    {
        fWriter->RegisterSlot("fs-polar", this, &KTROOTSpectrogramTypeWriterTransform::AddFrequencySpectrumDataPolar);
        fWriter->RegisterSlot("fs-fftw", this, &KTROOTSpectrogramTypeWriterTransform::AddFrequencySpectrumDataFFTW);
        fWriter->RegisterSlot("ps", this, &KTROOTSpectrogramTypeWriterTransform::AddPowerSpectrumData);
        fWriter->RegisterSlot("psd", this, &KTROOTSpectrogramTypeWriterTransform::AddPSDData);
        fWriter->RegisterSlot("proc-track", this, &KTROOTSpectrogramTypeWriterTransform::AddProcessedTrackData);
        fWriter->RegisterSlot("all-lines", this, &KTROOTSpectrogramTypeWriterTransform::TakeLine);
        fWriter->RegisterSlot("scores-1d", this, &KTROOTSpectrogramTypeWriterTransform::AddScoredSpectrumData);
        fWriter->RegisterSlot("track", this, &KTROOTSpectrogramTypeWriterTransform::AddProcessedTrackData);

        return;
    }


    //************************
    // Frequency Spectrum Data
    //************************

    void KTROOTSpectrogramTypeWriterTransform::AddFrequencySpectrumDataPolar(Nymph::KTDataPtr data)
    {
        AddFrequencySpectrumDataHelper< KTFrequencySpectrumDataPolar >(data, fFSPolarSpectrograms, "FSPolarSpectrogram_");
        return;
    }

    void KTROOTSpectrogramTypeWriterTransform::AddFrequencySpectrumDataFFTW(Nymph::KTDataPtr data)
    {
        AddFrequencySpectrumDataHelper< KTFrequencySpectrumDataFFTW >(data, fFSFFTWSpectrograms, "FSFFTWSpectrogram_");
        return;
    }

    //********************
    // Power Spectrum Data
    //********************

    void KTROOTSpectrogramTypeWriterTransform::AddPowerSpectrumData(Nymph::KTDataPtr data)
    {
        AddPowerSpectrumDataCoreHelper< KTPowerSpectrumData >(data, fPowerSpectrograms, "PowerSpectrogram_");
        return;
    }

    void KTROOTSpectrogramTypeWriterTransform::AddPSDData(Nymph::KTDataPtr data)
    {
        AddPowerSpectralDensityDataCoreHelper< KTPowerSpectrumData >(data, fPSDSpectrograms, "PSDSpectrogram_");
        return;
    }

    void KTROOTSpectrogramTypeWriterTransform::AddScoredSpectrumData(KTDataPtr data)
        {
            AddScoredSpectrumDataCoreHelper< KTScoredSpectrumData >(data, fScoredSpectrograms, "ScoredSpectrogram_");
            return;
        }

    //*********************
    // Processed Track Data
    //*********************

    void KTROOTSpectrogramTypeWriterTransform::AddProcessedTrackData(Nymph::KTDataPtr data)
    {
        KTProcessedTrackData thisTrack = data->Of< KTProcessedTrackData >();

        if( !thisTrack.GetIsCut() )
        {
            fWriter->SetMinTime( thisTrack.GetStartTimeInRunC() - fWriter->GetBufferTime() );
            fWriter->SetMaxTime( thisTrack.GetEndTimeInRunC() + fWriter->GetBufferTime() );
            fWriter->SetMinFreq( thisTrack.GetStartFrequency() - fWriter->GetBufferFreq() );
            fWriter->SetMaxFreq( thisTrack.GetEndFrequency() + fWriter->GetBufferFreq() );
        }

        return;
    }


    void KTROOTSpectrogramTypeWriterTransform::TakeLine(Nymph::KTDataPtr data)
    {
        KTProcessedTrackData thisLine = data->Of< KTProcessedTrackData >();
        
//        if( thisLine.GetIsCut() )
//        {
//            KTINFO("The track is cut! Will not add it to the Line Collection");
//            return;
//        }

        if (fLineCollection == NULL)
        {
            fLineCollection = new TOrdCollection();
        }
        TLine* rootLine = new TLine(thisLine.GetStartTimeInRunC(), thisLine.GetStartFrequency(), thisLine.GetEndTimeInRunC(), thisLine.GetEndFrequency());
        fLineCollection->Add(rootLine);
    }

} /* namespace Katydid */
