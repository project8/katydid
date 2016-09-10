/*
 * KTMultiSliceROOTTypeWriterTransform.cc
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#include "KT2ROOT.hh"
#include "KTEggHeader.hh"
#include "KTTIFactory.hh"
#include "KTLogger.hh"
#include "KTFrequencySpectrumPolar.hh"
#include "KTFrequencySpectrumFFTW.hh"
#include "KTFrequencySpectrumDataPolar.hh"
#include "KTFrequencySpectrumDataFFTW.hh"

#include "TCanvas.h"
#include "TH1.h"
#include "TStyle.h"

#include <sstream>
#include "KTMultiSliceROOTTypeWriterTransform.hh"


using std::stringstream;
using std::string;
using std::vector;

namespace Katydid
{
    KTLOGGER(publog, "KTMultiSliceROOTTypeWriterTransform");

    static Nymph::KTTIRegistrar< KTMultiSliceROOTTypeWriter, KTMultiSliceROOTTypeWriterTransform > sMERTWFRegistrar;

    KTMultiSliceROOTTypeWriterTransform::KTMultiSliceROOTTypeWriterTransform() :
            KTMEROOTTypeWriterBase(),
            //KTTypeWriterTransform()
            fFSHists(),
            fFSFFTWHists()
    {
    }

    KTMultiSliceROOTTypeWriterTransform::~KTMultiSliceROOTTypeWriterTransform()
    {
        ClearHistograms();
    }

    void KTMultiSliceROOTTypeWriterTransform::StartNewHistograms()
    {
        ClearHistograms();
        // At this point the vector is size 0
        return;
    }

    void KTMultiSliceROOTTypeWriterTransform::FinishHistograms()
    {
        OutputHistograms();
        ClearHistograms();
        return;
    }

    void KTMultiSliceROOTTypeWriterTransform::OutputHistograms()
    {
        if (! fWriter->OpenAndVerifyFile()) return;

        gStyle->SetOptStat(0);
        for (unsigned iChannel=0; iChannel < fFSHists.size(); iChannel++)
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

        for (unsigned iChannel=0; iChannel < fFSFFTWHists.size(); iChannel++)
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

    void KTMultiSliceROOTTypeWriterTransform::ClearHistograms()
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

    void KTMultiSliceROOTTypeWriterTransform::RegisterSlots()
    {
        fWriter->RegisterSlot("fs-polar", this, &KTMultiSliceROOTTypeWriterTransform::AddFrequencySpectrumDataPolar);
        fWriter->RegisterSlot("fs-fftw", this, &KTMultiSliceROOTTypeWriterTransform::AddFrequencySpectrumDataFFTW);
        return;
    }


    //*****************
    // Time Series Data
    //*****************

    void KTMultiSliceROOTTypeWriterTransform::AddFrequencySpectrumDataPolar(Nymph::KTDataPtr data)
    {
        KTFrequencySpectrumDataPolar& fsData = data->Of< KTFrequencySpectrumDataPolar >();
        if (fFSHists.size() == 0)
        {
            fFSHists.resize(fsData.GetNComponents());

            std::string histNameBase("PowerSpectrum");
            for (unsigned iChannel=0; iChannel < fsData.GetNComponents(); iChannel++)
            {
                std::stringstream conv;
                conv << iChannel;
                std::string histName = histNameBase + conv.str();
                TH1D* newPS = KT2ROOT::CreatePowerHistogram(fsData.GetSpectrumPolar(iChannel), histName);
                fFSHists[iChannel] = newPS;
            }
        }
        else
        {
            for (unsigned iChannel=0; iChannel < fsData.GetNComponents(); iChannel++)
            {
                TH1D* newPS = KT2ROOT::CreatePowerHistogram(fsData.GetSpectrumPolar(iChannel));
                fFSHists[iChannel]->Add(newPS);
                delete newPS;
            }
        }
        return;
    }

    void KTMultiSliceROOTTypeWriterTransform::AddFrequencySpectrumDataFFTW(Nymph::KTDataPtr data)
    {
        KTFrequencySpectrumDataFFTW& fsData = data->Of< KTFrequencySpectrumDataFFTW >();
        if (fFSFFTWHists.size() == 0)
        {
            fFSFFTWHists.resize(fsData.GetNComponents());

            std::string histNameBase("PowerSpectrum");
            for (unsigned iChannel=0; iChannel < fsData.GetNComponents(); iChannel++)
            {
                std::stringstream conv;
                conv << iChannel;
                std::string histName = histNameBase + conv.str();
                TH1D* newPS = KT2ROOT::CreatePowerHistogram(fsData.GetSpectrumFFTW(iChannel), histName);
                fFSFFTWHists[iChannel] = newPS;
            }
        }
        else
        {
            for (unsigned iChannel=0; iChannel < fsData.GetNComponents(); iChannel++)
            {
                TH1D* newPS = KT2ROOT::CreatePowerHistogram(fsData.GetSpectrumFFTW(iChannel));
                fFSFFTWHists[iChannel]->Add(newPS);
                delete newPS;
            }
        }
        return;
    }
} /* namespace Katydid */
