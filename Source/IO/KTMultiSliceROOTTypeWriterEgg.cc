/*
 * KTMultiSliceROOTTypeWriterEgg.cc
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#include "KTMultiSliceROOTTypeWriterEgg.hh"

#include "KTEggHeader.hh"
#include "KTTIFactory.hh"
#include "KTLogger.hh"
#include "KTTimeSeries.hh"
#include "KTTimeSeriesData.hh"

#include "TCanvas.h"
#include "TH1.h"
#include "TStyle.h"

#include <sstream>



using std::stringstream;
using std::string;
using std::vector;

namespace Katydid
{
    KTLOGGER(publog, "KTMultiSliceROOTTypeWriterEgg");

    static KTTIRegistrar< KTMultiSliceROOTTypeWriter, KTMultiSliceROOTTypeWriterEgg > sMERTWERegistrar;

    KTMultiSliceROOTTypeWriterEgg::KTMultiSliceROOTTypeWriterEgg() :
            KTMEROOTTypeWriterBase(),
            //KTTypeWriterEgg()
            fTSHists()
    {
    }

    KTMultiSliceROOTTypeWriterEgg::~KTMultiSliceROOTTypeWriterEgg()
    {
        ClearHistograms();
    }

    void KTMultiSliceROOTTypeWriterEgg::StartNewHistograms()
    {
        ClearHistograms();
        // At this point the vector is size 0
        return;
    }

    void KTMultiSliceROOTTypeWriterEgg::FinishHistograms()
    {
        OutputHistograms();
        ClearHistograms();
        return;
    }

    void KTMultiSliceROOTTypeWriterEgg::OutputHistograms()
    {
        if (! fWriter->OpenAndVerifyFile()) return;

        gStyle->SetOptStat(0);
        for (unsigned iComponent=0; iComponent < fTSHists.size(); ++iComponent)
        {
            // Printing to graphics files
            stringstream conv;
            conv << "_ts_" << iComponent << "." << fWriter->GetGraphicsFileType();
            string fileName = fWriter->GetGraphicsFilenameBase() + conv.str();
            if (! fWriter->GetGraphicsFilePath().empty()) fileName = fWriter->GetGraphicsFilePath() + '/' + fileName;

            TCanvas* cPrint = new TCanvas("cPrint", "cPrint");
            cPrint->SetLogy(1);
            fTSHists[iComponent]->Draw();

            cPrint->Print(fileName.c_str(), fWriter->GetGraphicsFileType().c_str());
            KTINFO(publog, "Printed file " << fileName);
            delete cPrint;

            // Writing to ROOT file
            fTSHists[iComponent]->SetDirectory(fWriter->GetFile());
            fTSHists[iComponent]->Write();
        }

        return;
    }

    void KTMultiSliceROOTTypeWriterEgg::ClearHistograms()
    {
        for (vector<TH1D*>::iterator it=fTSHists.begin(); it != fTSHists.end(); it++)
        {
            delete *it;
        }
        fTSHists.clear();
        return;
    }

    void KTMultiSliceROOTTypeWriterEgg::RegisterSlots()
    {
        fWriter->RegisterSlot("start-by-header", this, &KTMultiSliceROOTTypeWriterEgg::StartByHeader);

        fWriter->RegisterSlot("ts", this, &KTMultiSliceROOTTypeWriterEgg::AddTimeSeriesData);
        return;
    }


    void KTMultiSliceROOTTypeWriterEgg::StartByHeader(KTDataPtr)
    {
        fWriter->Start();
        return;
    }


    //*****************
    // Time Series Data
    //*****************

    void KTMultiSliceROOTTypeWriterEgg::AddTimeSeriesData(KTDataPtr data)
    {
        KTTimeSeriesData& tsData = data->Of<KTTimeSeriesData>();
        if (fTSHists.size() == 0)
        {
            fTSHists.resize(tsData.GetNComponents());

            std::string histNameBase("PowerSpectrum");
            for (unsigned iComponent=0; iComponent < tsData.GetNComponents(); ++iComponent)
            {
                std::stringstream conv;
                conv << iComponent;
                std::string histName = histNameBase + conv.str();
                TH1D* newPS = tsData.GetTimeSeries(iComponent)->CreateHistogram(histName);
                fTSHists[iComponent] = newPS;
            }
        }
        else
        {
            for (unsigned iComponent=0; iComponent < tsData.GetNComponents(); ++iComponent)
            {
                TH1D* newTS = tsData.GetTimeSeries(iComponent)->CreateHistogram();
                fTSHists[iComponent]->Add(newTS);
                delete newTS;
            }
        }
        return;
    }

} /* namespace Katydid */
