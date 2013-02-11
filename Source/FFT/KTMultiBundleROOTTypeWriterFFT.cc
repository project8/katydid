/*
 * KTMultiBundleROOTTypeWriterFFT.cc
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#include "KTMultiBundleROOTTypeWriterFFT.hh"

#include "KTEggHeader.hh"
#include "KTBundle.hh"
#include "KTTIFactory.hh"
#include "KTLogger.hh"
#include "KTFrequencySpectrumPolar.hh"
#include "KTFrequencySpectrumFFTW.hh"
#include "KTFrequencySpectrumData.hh"
#include "KTFrequencySpectrumDataFFTW.hh"

#include "TCanvas.h"
#include "TH1.h"
#include "TStyle.h"

#include <sstream>

using std::stringstream;
using std::string;
using std::vector;

namespace Katydid
{
    KTLOGGER(publog, "katydid.output");

    static KTDerivedTIRegistrar< KTMultiBundleROOTTypeWriter, KTMultiBundleROOTTypeWriterFFT > sMERTWFRegistrar;

    KTMultiBundleROOTTypeWriterFFT::KTMultiBundleROOTTypeWriterFFT() :
            KTMEROOTTypeWriterBase(),
            //KTTypeWriterFFT()
            fFSHists(),
            fFSFFTWHists()
    {
    }

    KTMultiBundleROOTTypeWriterFFT::~KTMultiBundleROOTTypeWriterFFT()
    {
        ClearHistograms();
    }

    void KTMultiBundleROOTTypeWriterFFT::StartNewHistograms()
    {
        ClearHistograms();
        // At this point the vector is size 0
        return;
    }

    void KTMultiBundleROOTTypeWriterFFT::FinishHistograms()
    {
        OutputHistograms();
        ClearHistograms();
        return;
    }

    void KTMultiBundleROOTTypeWriterFFT::OutputHistograms()
    {
        if (! fWriter->OpenAndVerifyFile()) return;

        gStyle->SetOptStat(0);
        for (UInt_t iChannel=0; iChannel < fFSHists.size(); iChannel++)
        {
            // Printing to graphics files
            stringstream conv;
            conv << "_fs_" << iChannel << "." << fWriter->GetGraphicsFileType();
            string fileName = fWriter->GetGraphicsFilenameBase() + conv.str();
            if (! fWriter->GetGraphicsFilePath().empty()) fileName = fWriter->GetGraphicsFilePath() + '/' + fileName;

            TCanvas* cPrint = new TCanvas("cPrint", "cPrint");
            cPrint->SetLogy(1);
            fFSHists[iChannel]->Draw();

            cPrint->Print(fileName.c_str(), fWriter->GetGraphicsFileType().c_str());
            KTINFO(publog, "Printed file " << fileName);
            delete cPrint;

            // Writing to ROOT file
            fFSHists[iChannel]->SetDirectory(fWriter->GetFile());
            fFSHists[iChannel]->Write();
        }

        for (UInt_t iChannel=0; iChannel < fFSFFTWHists.size(); iChannel++)
        {
            // Printing to graphics files
            stringstream conv;
            conv << "_fsfftw_" << iChannel << "." << fWriter->GetGraphicsFileType();
            string fileName = fWriter->GetGraphicsFilenameBase() + conv.str();
            if (! fWriter->GetGraphicsFilePath().empty()) fileName = fWriter->GetGraphicsFilePath() + '/' + fileName;

            TCanvas* cPrint = new TCanvas("cPrint", "cPrint");
            cPrint->SetLogy(1);
            fFSHists[iChannel]->Draw();

            cPrint->Print(fileName.c_str(), fWriter->GetGraphicsFileType().c_str());
            KTINFO(publog, "Printed file " << fileName);
            delete cPrint;

            fFSFFTWHists[iChannel]->SetDirectory(fWriter->GetFile());
            fFSFFTWHists[iChannel]->Write();
        }

        return;
    }

    void KTMultiBundleROOTTypeWriterFFT::ClearHistograms()
    {
        for (vector<TH1D*>::iterator it=fFSHists.begin(); it != fFSHists.end(); it++)
        {
            delete *it;
        }
        for (vector<TH1D*>::iterator it=fFSFFTWHists.begin(); it != fFSFFTWHists.end(); it++)
        {
            delete *it;
        }
        fFSHists.clear();
        fFSFFTWHists.clear();
        return;
    }

    void KTMultiBundleROOTTypeWriterFFT::RegisterSlots()
    {
        fWriter->RegisterSlot("fs-data", this, &KTMultiBundleROOTTypeWriterFFT::AddFrequencySpectrumData, "void (const KTFrequencySpectrumData*)");
        fWriter->RegisterSlot("fs-fftw-data", this, &KTMultiBundleROOTTypeWriterFFT::AddFrequencySpectrumDataFFTW, "void (const KTFrequencySpectrumDataFFTW*)");
        return;
    }


    //*****************
    // Time Series Data
    //*****************

    void KTMultiBundleROOTTypeWriterFFT::AddFrequencySpectrumData(const KTFrequencySpectrumData* data)
    {
        if (fFSHists.size() == 0)
        {
            fFSHists.resize(data->GetNChannels());

            std::string histNameBase("PowerSpectrum");
            for (UInt_t iChannel=0; iChannel < data->GetNChannels(); iChannel++)
            {
                std::stringstream conv;
                conv << iChannel;
                std::string histName = histNameBase + conv.str();
                TH1D* newPS = data->GetSpectrumPolar(iChannel)->CreatePowerHistogram(histName);
                fFSHists[iChannel] = newPS;
            }
        }
        else
        {
            for (UInt_t iChannel=0; iChannel < data->GetNChannels(); iChannel++)
            {
                TH1D* newPS = data->GetSpectrumPolar(iChannel)->CreatePowerHistogram();
                fFSHists[iChannel]->Add(newPS);
                delete newPS;
            }
        }
        return;
    }

    void KTMultiBundleROOTTypeWriterFFT::AddFrequencySpectrumDataFFTW(const KTFrequencySpectrumDataFFTW* data)
    {
        if (fFSFFTWHists.size() == 0)
        {
            fFSFFTWHists.resize(data->GetNChannels());

            std::string histNameBase("PowerSpectrum");
            for (UInt_t iChannel=0; iChannel < data->GetNChannels(); iChannel++)
            {
                std::stringstream conv;
                conv << iChannel;
                std::string histName = histNameBase + conv.str();
                TH1D* newPS = data->GetSpectrumFFTW(iChannel)->CreatePowerHistogram(histName);
                fFSFFTWHists[iChannel] = newPS;
            }
        }
        else
        {
            for (UInt_t iChannel=0; iChannel <data->GetNChannels(); iChannel++)
            {
                TH1D* newPS = data->GetSpectrumFFTW(iChannel)->CreatePowerHistogram();
                fFSFFTWHists[iChannel]->Add(newPS);
                delete newPS;
            }
        }
        return;
    }
} /* namespace Katydid */
