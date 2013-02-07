/*
 * KTBasicROOTTypeWriterFFT.cc
 *
 *  Created on: Jan 3, 2013
 *      Author: nsoblath
 */

#include "KTBasicROOTTypeWriterFFT.hh"

#include "KTBundle.hh"
#include "KTTIFactory.hh"
#include "KTLogger.hh"
#include "KTFrequencySpectrumPolar.hh"
#include "KTFrequencySpectrumData.hh"
#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTSlidingWindowFSData.hh"
#include "KTSlidingWindowFSDataFFTW.hh"

#include "TH1.h"
#include "TH2.h"

#include <sstream>

using std::stringstream;
using std::string;

namespace Katydid
{
    KTLOGGER(publog, "katydid.output");


    static KTDerivedTIRegistrar< KTBasicROOTTypeWriter, KTBasicROOTTypeWriterFFT > sBRTWFFTRegistrar;

    KTBasicROOTTypeWriterFFT::KTBasicROOTTypeWriterFFT() :
            KTBasicROOTTypeWriter()
    {
    }

    KTBasicROOTTypeWriterFFT::~KTBasicROOTTypeWriterFFT()
    {
    }


    void KTBasicROOTTypeWriterFFT::RegisterSlots()
    {
        fWriter->RegisterSlot("fs-data", this, &KTBasicROOTTypeWriterFFT::WriteFrequencySpectrumData, "void (const KTFrequencySpectrumData*)");
        fWriter->RegisterSlot("fs-data-fftw", this, &KTBasicROOTTypeWriterFFT::WriteFrequencySpectrumDataFFTW, "void (const KTFrequencySpectrumDataFFTW*)");
        fWriter->RegisterSlot("swfs-data", this, &KTBasicROOTTypeWriterFFT::WriteSlidingWindowFSData, "void (const WriteSlidingWindowFSData*)");
        fWriter->RegisterSlot("swfs-data-fftw", this, &KTBasicROOTTypeWriterFFT::WriteSlidingWindowFSDataFFTW, "void (const WriteSlidingWindowFSDataFFTW*)");
        return;
    }


    //************************
    // Frequency Spectrum Data
    //************************

    void KTBasicROOTTypeWriterFFT::WriteFrequencySpectrumData(const KTFrequencySpectrumData* data)
    {
        KTBundle* bundle = data->GetBundle();
        UInt_t bundleNumber = 0;
        if (bundle != NULL) bundleNumber = bundle->GetBundleNumber();
        UInt_t nChannels = data->GetNChannels();

        if (! fWriter->OpenAndVerifyFile()) return;

        for (unsigned iChannel=0; iChannel<nChannels; iChannel++)
        {
            const KTFrequencySpectrumPolar* spectrum = data->GetSpectrum(iChannel);
            if (spectrum != NULL)
            {
                stringstream conv;
                conv << "histPS_" << bundleNumber << "_" << iChannel;
                string histName;
                conv >> histName;
                TH1D* powerSpectrum = spectrum->CreatePowerHistogram(histName);
                powerSpectrum->SetDirectory(fWriter->GetFile());
                powerSpectrum->Write();
                KTDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
            }
        }
        return;
    }

    void KTBasicROOTTypeWriterFFT::WriteFrequencySpectrumDataFFTW(const KTFrequencySpectrumDataFFTW* data)
    {
        KTBundle* bundle = data->GetBundle();
        UInt_t bundleNumber = 0;
        if (bundle != NULL) bundleNumber = bundle->GetBundleNumber();
        UInt_t nChannels = data->GetNChannels();

        if (! fWriter->OpenAndVerifyFile()) return;

        for (unsigned iChannel=0; iChannel<nChannels; iChannel++)
        {
            const KTFrequencySpectrumFFTW* spectrum = data->GetSpectrum(iChannel);
            if (spectrum != NULL)
            {
                stringstream conv;
                conv << "histPS_" << bundleNumber << "_" << iChannel;
                string histName;
                conv >> histName;
                TH1D* powerSpectrum = spectrum->CreatePowerHistogram(histName);
                powerSpectrum->SetDirectory(fWriter->GetFile());
                powerSpectrum->Write();
                KTDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
            }
        }
        return;
    }

    //************************
    // Sliding Window Data
    //************************

    void KTBasicROOTTypeWriterFFT::WriteSlidingWindowFSData(const KTSlidingWindowFSData* data)
    {
        KTBundle* bundle = data->GetBundle();
        UInt_t bundleNumber = 0;
        if (bundle != NULL) bundleNumber = bundle->GetBundleNumber();
        UInt_t nPlots = data->GetNChannels();

        if (! fWriter->OpenAndVerifyFile()) return;

        for (unsigned iPlot=0; iPlot<nPlots; iPlot++)
        {
            stringstream conv;
            conv << "histSW_" << bundleNumber << "_" << iPlot;
            string histName;
            conv >> histName;
            TH2D* swHist = data->CreateMagnitudeHistogram(iPlot, histName);
            swHist->SetDirectory(fWriter->GetFile());
            swHist->Write();
            KTDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
        }
        return;
    }

    void KTBasicROOTTypeWriterFFT::WriteSlidingWindowFSDataFFTW(const KTSlidingWindowFSDataFFTW* data)
    {
        KTBundle* bundle = data->GetBundle();
        UInt_t bundleNumber = 0;
        if (bundle != NULL) bundleNumber = bundle->GetBundleNumber();
        UInt_t nPlots = data->GetNChannels();

        if (! fWriter->OpenAndVerifyFile()) return;

        for (unsigned iPlot=0; iPlot<nPlots; iPlot++)
        {
            stringstream conv;
            conv << "histSW_" << bundleNumber << "_" << iPlot;
            string histName;
            conv >> histName;
            TH2D* swHist = data->CreateMagnitudeHistogram(iPlot, histName);
            swHist->SetDirectory(fWriter->GetFile());
            swHist->Write();
            KTDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
        }
        return;
    }


} /* namespace Katydid */
