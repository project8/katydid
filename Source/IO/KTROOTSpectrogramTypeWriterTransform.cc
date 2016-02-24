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
        KTINFO("write spectrograms to root file");
        OutputSpectrograms();
        KTINFO("output lines to root file");
        OutputLines();
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

    void KTROOTSpectrogramTypeWriterTransform::OutputSpectrograms()
    {
        if (! fWriter->OpenAndVerifyFile()) return;

        OutputASpectrogramSet(fFSPolarSpectrograms);
        OutputASpectrogramSet(fFSFFTWSpectrograms);
        OutputASpectrogramSet(fPowerSpectrograms);
        OutputASpectrogramSet(fPSDSpectrograms);

        return;
    }

    void KTROOTSpectrogramTypeWriterTransform::OutputLines()
    {
        KTINFO("get file");
        TFile *aFile = fWriter->GetFile();
        aFile->cd();
        KTINFO("there are " << fLineCollection->GetEntries() << " lines in Collection");
        KTINFO("write lines... (this is where it seems to crash)");
        //aFile->WriteTObject(fLineCollection, "AllLines", "SingleKey");
        TOrdCollection* newLines = (TOrdCollection*)fLineCollection->Clone("AllLines");
        aFile->WriteTObject(newLines, "AllLines", "SingleKey");
        // this fails, breaking it into bits above to try and trace
        //fWriter->GetFile()->WriteTObject(&fLineCollection, "AllLines");
        fLineCollection = NULL;
        KTINFO("lines written");
        return;
    }

    void KTROOTSpectrogramTypeWriterTransform::ClearSpectrograms()
    {
        ClearASpectrogramSet(fFSPolarSpectrograms);
        ClearASpectrogramSet(fFSFFTWSpectrograms);
        ClearASpectrogramSet(fPowerSpectrograms);
        ClearASpectrogramSet(fPSDSpectrograms);
        return;
    }

    void KTROOTSpectrogramTypeWriterTransform::RegisterSlots()
    {
        fWriter->RegisterSlot("fs-polar", this, &KTROOTSpectrogramTypeWriterTransform::AddFrequencySpectrumDataPolar);
        fWriter->RegisterSlot("fs-fftw", this, &KTROOTSpectrogramTypeWriterTransform::AddFrequencySpectrumDataFFTW);
        fWriter->RegisterSlot("ps", this, &KTROOTSpectrogramTypeWriterTransform::AddPowerSpectrumData);
        fWriter->RegisterSlot("psd", this, &KTROOTSpectrogramTypeWriterTransform::AddPSDData);
        fWriter->RegisterSlot("all-lines", this, &KTROOTSpectrogramTypeWriterTransform::TakeLine);
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
