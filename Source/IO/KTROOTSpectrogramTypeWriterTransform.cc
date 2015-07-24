/*
 * KTROOTSpectrogramTypeWriterTransform.cc
 *
 *  Created on: Jun 18, 2015
 *      Author: nsoblath
 */

#include "KTROOTSpectrogramTypeWriterTransform.hh"

//#include "KT2ROOT.hh"
//#include "KTTIFactory.hh"
#include "KTLogger.hh"
#include "KTFrequencySpectrumPolar.hh"
#include "KTFrequencySpectrumFFTW.hh"
#include "KTFrequencySpectrumDataPolar.hh"
#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTSliceHeader.hh"

//#include "TCanvas.h"
#include "TH2.h"
//#include "TStyle.h"

#include <sstream>


using std::stringstream;
using std::string;
using std::vector;

namespace Katydid
{
    KTLOGGER(publog, "KTROOTSpectrogramTypeWriterTransform");

    static KTTIRegistrar< KTROOTSpectrogramTypeWriter, KTROOTSpectrogramTypeWriterTransform > sRSTWTRegistrar;

    KTROOTSpectrogramTypeWriterTransform::KTROOTSpectrogramTypeWriterTransform() :
            KTROOTSpectrogramTypeWriter(),
            //KTTypeWriterTransform()
            fFSPolarSpectrograms(),
            fFSFFTWSpectrograms()
    {
    }

    KTROOTSpectrogramTypeWriterTransform::~KTROOTSpectrogramTypeWriterTransform()
    {
        OutputSpectrograms();
    }

    void KTROOTSpectrogramTypeWriterTransform::OutputSpectrograms()
    {
        if (! fWriter->OpenAndVerifyFile()) return;

        OutputASpectrogramSet(fFSPolarSpectrograms);
        OutputASpectrogramSet(fFSFFTWSpectrograms);

        return;
    }

    void KTROOTSpectrogramTypeWriterTransform::ClearSpectrograms()
    {
        ClearASpectrogramSet(fFSPolarSpectrograms);
        ClearASpectrogramSet(fFSFFTWSpectrograms);
        return;
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

    void KTROOTSpectrogramTypeWriterTransform::RegisterSlots()
    {
        fWriter->RegisterSlot("fs-polar", this, &KTROOTSpectrogramTypeWriterTransform::AddFrequencySpectrumDataPolar);
        fWriter->RegisterSlot("fs-fftw", this, &KTROOTSpectrogramTypeWriterTransform::AddFrequencySpectrumDataFFTW);
        return;
    }


    //**********************
    // Frequency Series Data
    //**********************

    void KTROOTSpectrogramTypeWriterTransform::AddFrequencySpectrumDataPolar(KTDataPtr data)
    {
        KTSliceHeader& sliceHeader = data->Of< KTSliceHeader >();
        double timeInRun = sliceHeader.GetTimeInRun();
        double sliceLength = sliceHeader.GetSliceLength();
        // Check if this is a slice we should care about.
        // The first slice of interest will contain the writer's min time;
        // The last slice of interest will contain the writer's max time.
        if (timeInRun + sliceLength >= fWriter->GetMinTime() && timeInRun <= fWriter->GetMaxTime())
        {
            // Ok, this is a slice we should pay attention to.
            KTFrequencySpectrumDataPolar& fsData = data->Of< KTFrequencySpectrumDataPolar >();
            unsigned nComponents = fsData.GetNComponents();

            if (fFSPolarSpectrograms.size() < fsData.GetNComponents())
            {
                // Yes, we do need to resize the vector of histograms
                // Get number of components and resize the vector of histograms
                unsigned currentSize = fFSPolarSpectrograms.size();
                fFSPolarSpectrograms.resize(fsData.GetNComponents());

                // calculate the properties of the time axis
                double startTime = timeInRun;
                unsigned nSlices = unsigned((fWriter->GetMaxTime() - startTime) / sliceLength) + 1; // the +1 is so that the end time is the first slice ending outside the max time.
                double endTime = startTime + sliceLength * (double)nSlices;
                for (unsigned iComponent = currentSize; iComponent < nComponents; ++iComponent)
                {
                    // calculate the properties of the frequency axis
                    const KTFrequencySpectrumPolar* spectrum = fsData.GetSpectrumPolar(iComponent);
                    double freqBinWidth = spectrum->GetBinWidth();
                    fFSPolarSpectrograms[iComponent].fFirstFreqBin = unsigned((fWriter->GetMinFreq() - spectrum->GetBinLowEdge(0)) / freqBinWidth);
                    fFSPolarSpectrograms[iComponent].fLastFreqBin = unsigned((fWriter->GetMaxFreq() - spectrum->GetBinLowEdge(0)) / freqBinWidth) + 1;
                    unsigned nFreqBins = fFSPolarSpectrograms[iComponent].fLastFreqBin - fFSPolarSpectrograms[iComponent].fFirstFreqBin + 1;
                    double startFreq = fFSPolarSpectrograms[iComponent].fFirstFreqBin * freqBinWidth;
                    double endFreq = fFSPolarSpectrograms[iComponent].fLastFreqBin * freqBinWidth;
                    // form the histogram name
                    stringstream conv;
                    conv << iComponent;
                    string histName = string("FSPolarSpectrogram_") + conv.str();
                    fFSPolarSpectrograms[iComponent].fSpectrogram = new TH2D(histName.c_str(), "Spectrogram", nSlices, startTime, endTime, nFreqBins, startFreq, endFreq );
                    fFSPolarSpectrograms[iComponent].fNextTimeBinToFill = 0;
                }
            } // done initializing new spectrograms

            // add this slice's data to the spectrogram
            for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
            {
                const KTFrequencySpectrumPolar* spectrum = fsData.GetSpectrumPolar(iComponent);
                unsigned iSpectFreqBin = 0;
                for (unsigned iFreqBin = fFSPolarSpectrograms[iComponent].fFirstFreqBin; iFreqBin <= fFSPolarSpectrograms[iComponent].fLastFreqBin; ++iFreqBin)
                {
                    fFSPolarSpectrograms[iComponent].fSpectrogram->SetBinContent(fFSPolarSpectrograms[iComponent].fNextTimeBinToFill, iSpectFreqBin, spectrum->GetAbs(iFreqBin));
                    ++iSpectFreqBin;
                }
            }
        }

        return;
    }

    void KTROOTSpectrogramTypeWriterTransform::AddFrequencySpectrumDataFFTW(KTDataPtr data)
    {
        KTSliceHeader& sliceHeader = data->Of< KTSliceHeader >();
        double timeInRun = sliceHeader.GetTimeInRun();
        double sliceLength = sliceHeader.GetSliceLength();
        // Check if this is a slice we should care about.
        // The first slice of interest will contain the writer's min time;
        // The last slice of interest will contain the writer's max time.
        if (timeInRun + sliceLength >= fWriter->GetMinTime() && timeInRun <= fWriter->GetMaxTime())
        {
            // Ok, this is a slice we should pay attention to.
            KTFrequencySpectrumDataFFTW& fsData = data->Of< KTFrequencySpectrumDataFFTW >();
            unsigned nComponents = fsData.GetNComponents();

            if (fFSFFTWSpectrograms.size() < fsData.GetNComponents())
            {
                // Yes, we do need to resize the vector of histograms
                // Get number of components and resize the vector of histograms
                unsigned currentSize = fFSPolarSpectrograms.size();
                fFSFFTWSpectrograms.resize(fsData.GetNComponents());

                // calculate the properties of the time axis
                double startTime = timeInRun;
                unsigned nSlices = unsigned((fWriter->GetMaxTime() - startTime) / sliceLength) + 1; // the +1 is so that the end time is the first slice ending outside the max time.
                double endTime = startTime + sliceLength * (double)nSlices;
                for (unsigned iComponent = currentSize; iComponent < nComponents; ++iComponent)
                {
                    // calculate the properties of the frequency axis
                    const KTFrequencySpectrumFFTW* spectrum = fsData.GetSpectrumFFTW(iComponent);
                    double freqBinWidth = spectrum->GetBinWidth();
                    fFSFFTWSpectrograms[iComponent].fFirstFreqBin = unsigned((fWriter->GetMinFreq() - spectrum->GetBinLowEdge(0)) / freqBinWidth);
                    fFSFFTWSpectrograms[iComponent].fLastFreqBin = unsigned((fWriter->GetMaxFreq() - spectrum->GetBinLowEdge(0)) / freqBinWidth) + 1;
                    unsigned nFreqBins = fFSFFTWSpectrograms[iComponent].fLastFreqBin - fFSFFTWSpectrograms[iComponent].fFirstFreqBin + 1;
                    double startFreq = fFSFFTWSpectrograms[iComponent].fFirstFreqBin * freqBinWidth;
                    double endFreq = fFSFFTWSpectrograms[iComponent].fLastFreqBin * freqBinWidth;
                    // form the histogram name
                    stringstream conv;
                    conv << iComponent;
                    string histName = string("FSFFTWSpectrogram_") + conv.str();
                    fFSFFTWSpectrograms[iComponent].fSpectrogram = new TH2D(histName.c_str(), "Spectrogram", nSlices, startTime, endTime, nFreqBins, startFreq, endFreq );
                    fFSFFTWSpectrograms[iComponent].fNextTimeBinToFill = 0;
                }
            } // done initializing new spectrograms

            // add this slice's data to the spectrogram
            for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
            {
                const KTFrequencySpectrumFFTW* spectrum = fsData.GetSpectrumFFTW(iComponent);
                unsigned iSpectFreqBin = 0;
                for (unsigned iFreqBin = fFSFFTWSpectrograms[iComponent].fFirstFreqBin; iFreqBin <= fFSFFTWSpectrograms[iComponent].fLastFreqBin; ++iFreqBin)
                {
                    //std::cout << "spectrum bin: " << iFreqBin << "   spectrogram bins (" << fFSFFTWSpectrograms[iComponent].fNextTimeBinToFill << ", " << iSpectFreqBin << "    value: " << spectrum->GetAbs(iFreqBin) << std::endl;
                    fFSFFTWSpectrograms[iComponent].fSpectrogram->SetBinContent(fFSFFTWSpectrograms[iComponent].fNextTimeBinToFill, iSpectFreqBin, spectrum->GetAbs(iFreqBin));
                    ++iSpectFreqBin;
                }
                fFSFFTWSpectrograms[iComponent].fNextTimeBinToFill += 1;
            }
        }

        return;
    }
} /* namespace Katydid */
