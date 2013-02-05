/*
 * KTMultiEventROOTTypeWriterEgg.cc
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#include "KTMultiEventROOTTypeWriterEgg.hh"

#include "KTEggHeader.hh"
#include "KTBundle.hh"
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
    KTLOGGER(publog, "katydid.output");

    static KTDerivedTIRegistrar< KTMultiEventROOTTypeWriter, KTMultiEventROOTTypeWriterEgg > sMERTWERegistrar;

    KTMultiEventROOTTypeWriterEgg::KTMultiEventROOTTypeWriterEgg() :
            KTMEROOTTypeWriterBase(),
            //KTTypeWriterEgg()
            fTSHists()
    {
    }

    KTMultiEventROOTTypeWriterEgg::~KTMultiEventROOTTypeWriterEgg()
    {
        ClearHistograms();
    }

    void KTMultiEventROOTTypeWriterEgg::StartNewHistograms()
    {
        ClearHistograms();
        // At this point the vector is size 0
        return;
    }

    void KTMultiEventROOTTypeWriterEgg::FinishHistograms()
    {
        OutputHistograms();
        ClearHistograms();
        return;
    }

    void KTMultiEventROOTTypeWriterEgg::OutputHistograms()
    {
        if (! fWriter->OpenAndVerifyFile()) return;

        gStyle->SetOptStat(0);
        for (UInt_t iChannel=0; iChannel < fTSHists.size(); iChannel++)
        {
            // Printing to graphics files
            stringstream conv;
            conv << "_ts_" << iChannel << "." << fWriter->GetGraphicsFileType();
            string fileName = fWriter->GetGraphicsFilenameBase() + conv.str();
            if (! fWriter->GetGraphicsFilePath().empty()) fileName = fWriter->GetGraphicsFilePath() + '/' + fileName;

            TCanvas* cPrint = new TCanvas("cPrint", "cPrint");
            cPrint->SetLogy(1);
            fTSHists[iChannel]->Draw();

            cPrint->Print(fileName.c_str(), fWriter->GetGraphicsFileType().c_str());
            KTINFO(publog, "Printed file " << fileName);
            delete cPrint;

            // Writing to ROOT file
            fTSHists[iChannel]->SetDirectory(fWriter->GetFile());
            fTSHists[iChannel]->Write();
        }

        return;
    }

    void KTMultiEventROOTTypeWriterEgg::ClearHistograms()
    {
        for (vector<TH1D*>::iterator it=fTSHists.begin(); it != fTSHists.end(); it++)
        {
            delete *it;
        }
        fTSHists.clear();
        return;
    }

    void KTMultiEventROOTTypeWriterEgg::RegisterSlots()
    {
        fWriter->RegisterSlot("start-by-header", this, &KTMultiEventROOTTypeWriterEgg::StartByHeader, "void (const KTEggHeader*)");

        fWriter->RegisterSlot("ts-data", this, &KTMultiEventROOTTypeWriterEgg::AddTimeSeriesData, "void (const KTTimeSeriesData*)");
        return;
    }


    void KTMultiEventROOTTypeWriterEgg::StartByHeader(const KTEggHeader* header)
    {
        fWriter->Start();
        return;
    }


    //*****************
    // Time Series Data
    //*****************

    void KTMultiEventROOTTypeWriterEgg::AddTimeSeriesData(const KTTimeSeriesData* data)
    {
        if (fTSHists.size() == 0)
        {
            fTSHists.resize(data->GetNTimeSeries());

            std::string histNameBase("PowerSpectrum");
            for (UInt_t iChannel=0; iChannel < data->GetNTimeSeries(); iChannel++)
            {
                std::stringstream conv;
                conv << iChannel;
                std::string histName = histNameBase + conv.str();
                TH1D* newPS = data->GetTimeSeries(iChannel)->CreateHistogram(histName);
                fTSHists[iChannel] = newPS;
            }
        }
        else
        {
            for (UInt_t iChannel=0; iChannel < data->GetNTimeSeries(); iChannel++)
            {
                TH1D* newTS = data->GetTimeSeries(iChannel)->CreateHistogram();
                fTSHists[iChannel]->Add(newTS);
                delete newTS;
            }
        }
        return;
    }

} /* namespace Katydid */
