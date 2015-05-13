/*
 * KTMultiSliceROOTTypeWriterTime.cc
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#include "KTEggHeader.hh"
#include "KTTIFactory.hh"
#include "KTLogger.hh"
#include "KTTimeSeries.hh"
#include "KTTimeSeriesData.hh"

#include "TCanvas.h"
#include "TH1.h"
#include "TStyle.h"

#include <sstream>
#include "KTMultiSliceROOTTypeWriterTime.hh"



using std::stringstream;
using std::string;
using std::vector;

namespace Katydid
{
    KTLOGGER(publog, "KTMultiSliceROOTTypeWriterTime");

    static KTTIRegistrar< KTMultiSliceROOTTypeWriter, KTMultiSliceROOTTypeWriterTime > sMERTWERegistrar;

    KTMultiSliceROOTTypeWriterTime::KTMultiSliceROOTTypeWriterTime() :
            KTMEROOTTypeWriterBase(),
            //KTTypeWriterTime()
            fTSHists()
    {
    }

    KTMultiSliceROOTTypeWriterTime::~KTMultiSliceROOTTypeWriterTime()
    {
        ClearHistograms();
    }

    void KTMultiSliceROOTTypeWriterTime::StartNewHistograms()
    {
        ClearHistograms();
        // At this point the vector is size 0
        return;
    }

    void KTMultiSliceROOTTypeWriterTime::FinishHistograms()
    {
        OutputHistograms();
        ClearHistograms();
        return;
    }

    void KTMultiSliceROOTTypeWriterTime::OutputHistograms()
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

    void KTMultiSliceROOTTypeWriterTime::ClearHistograms()
    {
        for (vector<TH1D*>::iterator it=fTSHists.begin(); it != fTSHists.end(); it++)
        {
            delete *it;
        }
        fTSHists.clear();
        return;
    }

    void KTMultiSliceROOTTypeWriterTime::RegisterSlots()
    {
        fWriter->RegisterSlot("start-by-header", this, &KTMultiSliceROOTTypeWriterTime::StartByHeader);

        fWriter->RegisterSlot("ts", this, &KTMultiSliceROOTTypeWriterTime::AddTimeSeriesData);
        return;
    }


    void KTMultiSliceROOTTypeWriterTime::StartByHeader(KTDataPtr)
    {
        fWriter->Start();
        return;
    }


    //*****************
    // Time Series Data
    //*****************

    void KTMultiSliceROOTTypeWriterTime::AddTimeSeriesData(KTDataPtr data)
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
