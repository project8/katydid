/*
 * KTMultiBundleROOTTypeWriterAnalysis.cc
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#include "KTMultiBundleROOTTypeWriterAnalysis.hh"

#include "KTEggHeader.hh"
#include "KTTIFactory.hh"
#include "KTLogger.hh"
#include "KTFrequencySpectrumPolar.hh"
#include "KTSliceHeader.hh"
#include "KTCorrelator.hh"

#include "TCanvas.h"
#include "TH1.h"
#include "TStyle.h"

#include <sstream>

using boost::shared_ptr;

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
        for (UInt_t iComponent=0; iComponent < fCorrHists.size(); iComponent++)
        {
            // Printing to graphics files
            stringstream conv;
            conv << "_corr_" << iComponent << "." << fWriter->GetGraphicsFileType();
            string fileName = fWriter->GetGraphicsFilenameBase() + conv.str();
            if (! fWriter->GetGraphicsFilePath().empty()) fileName = fWriter->GetGraphicsFilePath() + '/' + fileName;

            TCanvas* cPrint = new TCanvas("cPrint", "cPrint");
            cPrint->SetLogy(1);
            fCorrHists[iComponent]->Draw();

            cPrint->Print(fileName.c_str(), fWriter->GetGraphicsFileType().c_str());
            KTINFO(publog, "Printed file " << fileName);
            delete cPrint;

            // Writing to ROOT file
            fCorrHists[iComponent]->SetDirectory(fWriter->GetFile());
            fCorrHists[iComponent]->Write();
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
        fWriter->RegisterSlot("corr-data", this, &KTMultiBundleROOTTypeWriterAnalysis::AddCorrelationData, "void (shared_ptr< KTData >)");
        return;
    }


    //*****************
    // Time Series Data
    //*****************

    void KTMultiBundleROOTTypeWriterAnalysis::AddCorrelationData(shared_ptr< KTData > data)
    {
        if (! data) return;

        ULong64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTCorrelationData& corrData = data->Of<KTCorrelationData>();
        UInt_t nComponents = corrData.GetNComponents();

        if (fCorrHists.size() == 0)
        {
            fCorrHists.resize(nComponents);

            std::string histNameBase("Correlation");
            for (UInt_t iComponent=0; iComponent < nComponents; iComponent++)
            {
                std::stringstream conv;
                conv << iComponent;
                std::string histName = histNameBase + conv.str();
                TH1D* newCorr = corrData.GetSpectrum(iComponent)->CreateMagnitudeHistogram(histName);
                fCorrHists[iComponent] = newCorr;
            }
        }
        else
        {
            for (UInt_t iComponent=0; iComponent < nComponents; iComponent++)
            {
                TH1D* newCorr = corrData.GetSpectrum(iComponent)->CreateMagnitudeHistogram();
                fCorrHists[iComponent]->Add(newCorr);
                delete newCorr;
            }
        }
        return;
    }

} /* namespace Katydid */
