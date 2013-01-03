/*
 * KTBasicROOTTypeWriterAnalysis.cc
 *
 *  Created on: Jan 3, 2013
 *      Author: nsoblath
 */

#include "KTBasicROOTTypeWriterAnalysis.hh"

#include "KTEvent.hh"
#include "KTTIFactory.hh"
#include "KTLogger.hh"
#include "KTCorrelationData.hh"
#include "KTFrequencySpectrum.hh"
#include "KTHoughData.hh"
#include "KTGainVariationData.hh"

#include "TH1.h"
#include "TH2.h"

#include <sstream>

using std::stringstream;
using std::string;

namespace Katydid
{
    KTLOGGER(publog, "katydid.output");


    static KTDerivedTIRegistrar< KTBasicROOTTypeWriter, KTBasicROOTTypeWriterAnalysis > sBRTWAnalysisRegistrar;

    KTBasicROOTTypeWriterAnalysis::KTBasicROOTTypeWriterAnalysis() :
            KTBasicROOTTypeWriter()
    {
    }

    KTBasicROOTTypeWriterAnalysis::~KTBasicROOTTypeWriterAnalysis()
    {
    }


    void KTBasicROOTTypeWriterAnalysis::RegisterSlots()
    {
        fWriter->RegisterSlot("corr-data", this, &KTBasicROOTTypeWriterAnalysis::WriteCorrelationData, "void (const KTCorrelationData*)");
        fWriter->RegisterSlot("hough-data", this, &KTBasicROOTTypeWriterAnalysis::WriteHoughData, "void (const KTHoughData*)");
        fWriter->RegisterSlot("gain-var-data", this, &KTBasicROOTTypeWriterAnalysis::WriteGainVariationData, "void (const KTGainVariationData*)");
        return;
    }


    //************************
    // Correlation Data
    //************************

    void KTBasicROOTTypeWriterAnalysis::WriteCorrelationData(const KTCorrelationData* data)
    {
        KTEvent* event = data->GetEvent();
        UInt_t eventNumber = 0;
        if (event != NULL) eventNumber = event->GetEventNumber();
        UInt_t nPairs = data->GetNPairs();

        if (! fWriter->OpenAndVerifyFile()) return;

        for (unsigned iPair=0; iPair<nPairs; iPair++)
        {
            const KTFrequencySpectrum* spectrum = data->GetCorrelation(iPair);
            if (spectrum != NULL)
            {
                stringstream conv;
                conv << "histCorr_" << eventNumber << "_" << iPair;
                string histName;
                conv >> histName;
                TH1D* corrHist = spectrum->CreateMagnitudeHistogram(histName);
                stringstream titleStream;
                titleStream << "Event " << eventNumber << ", Correlation " << iPair << ", "
                        "Channels (" << data->GetFirstChannel(iPair) << ", " << data->GetSecondChannel(iPair) << ")";
                corrHist->SetTitle(titleStream.str().c_str());
                corrHist->SetDirectory(fWriter->GetFile());
                corrHist->Write();
                KTDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
            }
        }
        return;
    }

    //************************
    // Hough Transform Data
    //************************

    void KTBasicROOTTypeWriterAnalysis::WriteHoughData(const KTHoughData* data)
    {
        KTEvent* event = data->GetEvent();
        UInt_t eventNumber = 0;
        if (event != NULL) eventNumber = event->GetEventNumber();
        UInt_t nPlots = data->GetNTransforms();

        if (! fWriter->OpenAndVerifyFile()) return;

        for (unsigned iPlot=0; iPlot<nPlots; iPlot++)
        {
            stringstream conv;
            conv << "histHT_" << eventNumber << "_" << iPlot;
            string histName;
            conv >> histName;
            TH2D* swHist = data->CreateHistogram(iPlot, histName);
            swHist->SetDirectory(fWriter->GetFile());
            swHist->Write();
            KTDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
        }
        return;
    }

    //************************
    // Gain Variation Data
    //************************

    void KTBasicROOTTypeWriterAnalysis::WriteGainVariationData(const KTGainVariationData* data)
    {
        KTEvent* event = data->GetEvent();
        UInt_t eventNumber = 0;
        if (event != NULL) eventNumber = event->GetEventNumber();
        UInt_t nPlots = data->GetNChannels();

        if (! fWriter->OpenAndVerifyFile()) return;

        for (unsigned iPlot=0; iPlot<nPlots; iPlot++)
        {
            stringstream conv;
            conv << "histGV_" << eventNumber << "_" << iPlot;
            string histName;
            conv >> histName;
            TH1D* gvHist = data->CreateGainVariationHistogram(100, iPlot, histName);
            gvHist->SetDirectory(fWriter->GetFile());
            gvHist->Write();
            KTDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");

            /*
            stringstream conv2;
            string splineName;
            conv2 << "splineGV_" << eventNumber << "_" << iPlot;
            conv2 >> splineName;
            const TSpline* spline = data->GetSpline(iPlot);
            if (spline == NULL)
            {
                KTDEBUG(publog, "No spline in the data");
                continue;
            }
            TSpline* splineClone = (TSpline*)spline->Clone();
            splineClone->SetName(splineName.c_str());
            splineClone->Write();
            KTDEBUG(publog, "Spline <" << splineName << "> written to ROOT file");
            */
        }
        return;
    }


} /* namespace Katydid */
