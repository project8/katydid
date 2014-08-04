/*
 * KTMultiSliceROOTTypeWriterAnalysis.cc
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#include "KTMultiSliceROOTTypeWriterAnalysis.hh"

#include "KTCorrelationData.hh"
#include "KTEggHeader.hh"
#include "KTTIFactory.hh"
#include "KTLogger.hh"
#include "KTFrequencySpectrumPolar.hh"
#include "KTSliceHeader.hh"

#include "TCanvas.h"
#include "TH1.h"
#include "TStyle.h"

#include <sstream>



using std::stringstream;
using std::string;
using std::vector;

namespace Katydid
{
    KTLOGGER(publog, "KTMultiSliceROOTTypeWriterAnalysis");

    static KTTIRegistrar< KTMultiSliceROOTTypeWriter, KTMultiSliceROOTTypeWriterAnalysis > sMERTWARegistrar;

    KTMultiSliceROOTTypeWriterAnalysis::KTMultiSliceROOTTypeWriterAnalysis() :
            KTMEROOTTypeWriterBase(),
            //KTTypeWriterAnalysis()
            fCorrHists()
    {
    }

    KTMultiSliceROOTTypeWriterAnalysis::~KTMultiSliceROOTTypeWriterAnalysis()
    {
        ClearHistograms();
    }

    void KTMultiSliceROOTTypeWriterAnalysis::StartNewHistograms()
    {
        ClearHistograms();
        // At this point the vector is size 0
        return;
    }

    void KTMultiSliceROOTTypeWriterAnalysis::FinishHistograms()
    {
        OutputHistograms();
        ClearHistograms();
        return;
    }

    void KTMultiSliceROOTTypeWriterAnalysis::OutputHistograms()
    {
        if (! fWriter->OpenAndVerifyFile()) return;

        gStyle->SetOptStat(0);
        for (unsigned iComponent=0; iComponent < fCorrHists.size(); ++iComponent)
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

    void KTMultiSliceROOTTypeWriterAnalysis::ClearHistograms()
    {
        for (vector<TH1D*>::iterator it=fCorrHists.begin(); it != fCorrHists.end(); it++)
        {
            delete *it;
        }
        fCorrHists.clear();
        return;
    }

    void KTMultiSliceROOTTypeWriterAnalysis::RegisterSlots()
    {
        fWriter->RegisterSlot("corr", this, &KTMultiSliceROOTTypeWriterAnalysis::AddCorrelationData);
        return;
    }


    //*****************
    // Time Series Data
    //*****************

    void KTMultiSliceROOTTypeWriterAnalysis::AddCorrelationData(KTDataPtr data)
    {
        if (! data) return;

        //uint64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTCorrelationData& corrData = data->Of<KTCorrelationData>();
        unsigned nComponents = corrData.GetNComponents();

        if (fCorrHists.size() == 0)
        {
            fCorrHists.resize(nComponents);

            std::string histNameBase("Correlation");
            for (unsigned iComponent=0; iComponent < nComponents; ++iComponent)
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
            for (unsigned iComponent=0; iComponent < nComponents; ++iComponent)
            {
                TH1D* newCorr = corrData.GetSpectrum(iComponent)->CreateMagnitudeHistogram();
                fCorrHists[iComponent]->Add(newCorr);
                delete newCorr;
            }
        }
        return;
    }

} /* namespace Katydid */
