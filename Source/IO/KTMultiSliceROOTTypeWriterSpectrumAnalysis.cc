/*
 * KTMultiSliceROOTTypeWriterSpectrumAnalysis.cc
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#include "KT2ROOT.hh"
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
#include "KTMultiSliceROOTTypeWriterSpectrumAnalysis.hh"



using std::stringstream;
using std::string;
using std::vector;

namespace Katydid
{
    KTLOGGER(publog, "KTMultiSliceROOTTypeWriterSpectrumAnalysis");

    static KTTIRegistrar< KTMultiSliceROOTTypeWriter, KTMultiSliceROOTTypeWriterSpectrumAnalysis > sMERTWARegistrar;

    KTMultiSliceROOTTypeWriterSpectrumAnalysis::KTMultiSliceROOTTypeWriterSpectrumAnalysis() :
            KTMEROOTTypeWriterBase(),
            //KTTypeWriterSpectrumAnalysis()
            fCorrHists()
    {
    }

    KTMultiSliceROOTTypeWriterSpectrumAnalysis::~KTMultiSliceROOTTypeWriterSpectrumAnalysis()
    {
        ClearHistograms();
    }

    void KTMultiSliceROOTTypeWriterSpectrumAnalysis::StartNewHistograms()
    {
        ClearHistograms();
        // At this point the vector is size 0
        return;
    }

    void KTMultiSliceROOTTypeWriterSpectrumAnalysis::FinishHistograms()
    {
        OutputHistograms();
        ClearHistograms();
        return;
    }

    void KTMultiSliceROOTTypeWriterSpectrumAnalysis::OutputHistograms()
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

    void KTMultiSliceROOTTypeWriterSpectrumAnalysis::ClearHistograms()
    {
        for (vector<TH1D*>::iterator it=fCorrHists.begin(); it != fCorrHists.end(); it++)
        {
            delete *it;
        }
        fCorrHists.clear();
        return;
    }

    void KTMultiSliceROOTTypeWriterSpectrumAnalysis::RegisterSlots()
    {
        fWriter->RegisterSlot("corr", this, &KTMultiSliceROOTTypeWriterSpectrumAnalysis::AddCorrelationData);
        return;
    }


    //*****************
    // Time Series Data
    //*****************

    void KTMultiSliceROOTTypeWriterSpectrumAnalysis::AddCorrelationData(KTDataPtr data)
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
                TH1D* newCorr = KT2ROOT::CreateMagnitudeHistogram(corrData.GetSpectrumPolar(iComponent), histName);
                fCorrHists[iComponent] = newCorr;
            }
        }
        else
        {
            for (unsigned iComponent=0; iComponent < nComponents; ++iComponent)
            {
                TH1D* newCorr = KT2ROOT::CreateMagnitudeHistogram(corrData.GetSpectrumPolar(iComponent));
                fCorrHists[iComponent]->Add(newCorr);
                delete newCorr;
            }
        }
        return;
    }

} /* namespace Katydid */
