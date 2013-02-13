/*
 * KTMultiBundleROOTTypeWriterAnalysis.cc
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#include "KTMultiBundleROOTTypeWriterAnalysis.hh"

#include "KTEggHeader.hh"
#include "KTBundle.hh"
#include "KTTIFactory.hh"
#include "KTLogger.hh"
#include "KTFrequencySpectrumPolar.hh"
#include "KTCorrelationData.hh"

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

    static KTDerivedTIRegistrar< KTMultiBundleROOTTypeWriter, KTMultiBundleROOTTypeWriterAnalysis > sMERTWARegistrar;

    KTMultiBundleROOTTypeWriterAnalysis::KTMultiBundleROOTTypeWriterAnalysis() :
            KTMEROOTTypeWriterBase(),
            //KTTypeWriterAnalysis()
            fCorrHists()
    {
    }

    KTMultiBundleROOTTypeWriterAnalysis::~KTMultiBundleROOTTypeWriterAnalysis()
    {
        ClearHistograms();
    }

    void KTMultiBundleROOTTypeWriterAnalysis::StartNewHistograms()
    {
        ClearHistograms();
        // At this point the vector is size 0
        return;
    }

    void KTMultiBundleROOTTypeWriterAnalysis::FinishHistograms()
    {
        OutputHistograms();
        ClearHistograms();
        return;
    }

    void KTMultiBundleROOTTypeWriterAnalysis::OutputHistograms()
    {
        if (! fWriter->OpenAndVerifyFile()) return;

        gStyle->SetOptStat(0);
        for (UInt_t iChannel=0; iChannel < fCorrHists.size(); iChannel++)
        {
            // Printing to graphics files
            stringstream conv;
            conv << "_corr_" << iChannel << "." << fWriter->GetGraphicsFileType();
            string fileName = fWriter->GetGraphicsFilenameBase() + conv.str();
            if (! fWriter->GetGraphicsFilePath().empty()) fileName = fWriter->GetGraphicsFilePath() + '/' + fileName;

            TCanvas* cPrint = new TCanvas("cPrint", "cPrint");
            cPrint->SetLogy(1);
            fCorrHists[iChannel]->Draw();

            cPrint->Print(fileName.c_str(), fWriter->GetGraphicsFileType().c_str());
            KTINFO(publog, "Printed file " << fileName);
            delete cPrint;

            // Writing to ROOT file
            fCorrHists[iChannel]->SetDirectory(fWriter->GetFile());
            fCorrHists[iChannel]->Write();
        }
        return;
    }

    void KTMultiBundleROOTTypeWriterAnalysis::ClearHistograms()
    {
        for (vector<TH1D*>::iterator it=fCorrHists.begin(); it != fCorrHists.end(); it++)
        {
            delete *it;
        }
        fCorrHists.clear();
        return;
    }

    void KTMultiBundleROOTTypeWriterAnalysis::RegisterSlots()
    {
        fWriter->RegisterSlot("corr-data", this, &KTMultiBundleROOTTypeWriterAnalysis::AddCorrelationData, "void (const KTCorrelationData*)");
        return;
    }


    //*****************
    // Time Series Data
    //*****************

    void KTMultiBundleROOTTypeWriterAnalysis::AddCorrelationData(const KTCorrelationData* data)
    {
        if (fCorrHists.size() == 0)
        {
            fCorrHists.resize(data->GetNComponents());

            std::string histNameBase("Correlation");
            for (UInt_t iChannel=0; iChannel < data->GetNComponents(); iChannel++)
            {
                std::stringstream conv;
                conv << iChannel;
                std::string histName = histNameBase + conv.str();
                TH1D* newCorr = data->GetCorrelation(iChannel)->CreateMagnitudeHistogram(histName);
                fCorrHists[iChannel] = newCorr;
            }
        }
        else
        {
            for (UInt_t iChannel=0; iChannel < data->GetNComponents(); iChannel++)
            {
                TH1D* newCorr = data->GetCorrelation(iChannel)->CreateMagnitudeHistogram();
                fCorrHists[iChannel]->Add(newCorr);
                delete newCorr;
            }
        }
        return;
    }

} /* namespace Katydid */
