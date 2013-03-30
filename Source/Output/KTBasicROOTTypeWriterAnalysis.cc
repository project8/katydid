/*
 * KTBasicROOTTypeWriterAnalysis.cc
 *
 *  Created on: Jan 3, 2013
 *      Author: nsoblath
 */

#include "KTBasicROOTTypeWriterAnalysis.hh"

#include "KTAnalyticAssociateData.hh"
#include "KTCorrelationData.hh"
#include "KTFrequencySpectrumPolar.hh"
#include "KTGainVariationData.hh"
#include "KTHoughData.hh"
#include "KTLogger.hh"
#include "KTNormalizedFSData.hh"
#include "KTSliceHeader.hh"
#include "KTTIFactory.hh"
#include "KTWignerVilleData.hh"

#include "TH1.h"
#include "TH2.h"

#include <sstream>

using boost::shared_ptr;

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
        fWriter->RegisterSlot("norm-fs-polar", this, &KTBasicROOTTypeWriterAnalysis::WriteNormalizedFSDataPolar);
        fWriter->RegisterSlot("norm-fs-fftw", this, &KTBasicROOTTypeWriterAnalysis::WriteNormalizedFSDataFFTW);
        fWriter->RegisterSlot("aa", this, &KTBasicROOTTypeWriterAnalysis::WriteAnalyticAssociateData);
        fWriter->RegisterSlot("corr", this, &KTBasicROOTTypeWriterAnalysis::WriteCorrelationData);
        fWriter->RegisterSlot("hough", this, &KTBasicROOTTypeWriterAnalysis::WriteHoughData);
        fWriter->RegisterSlot("gain-var", this, &KTBasicROOTTypeWriterAnalysis::WriteGainVariationData);
        fWriter->RegisterSlot("wv", this, &KTBasicROOTTypeWriterAnalysis::WriteWignerVilleData);
        return;
    }


    //************************
    // Frequency Spectrum Data
    //************************

    void KTBasicROOTTypeWriterAnalysis::WriteNormalizedFSDataPolar(boost::shared_ptr<KTData> data)
    {
        if (! data) return;

        ULong64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTNormalizedFSDataPolar& fsData = data->Of<KTNormalizedFSDataPolar>();
        UInt_t nComponents = fsData.GetNComponents();

        if (! fWriter->OpenAndVerifyFile()) return;

        for (unsigned iChannel=0; iChannel<nComponents; iChannel++)
        {
            const KTFrequencySpectrumPolar* spectrum = fsData.GetSpectrumPolar(iChannel);
            if (spectrum != NULL)
            {
                stringstream conv;
                conv << "histNFSpolar_" << sliceNumber << "_" << iChannel;
                string histName;
                conv >> histName;
                TH1D* powerSpectrum = spectrum->CreateMagnitudeHistogram(histName);
                powerSpectrum->SetDirectory(fWriter->GetFile());
                powerSpectrum->Write();
                KTDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
            }
        }
        return;
    }

    void KTBasicROOTTypeWriterAnalysis::WriteNormalizedFSDataFFTW(boost::shared_ptr<KTData> data)
    {
        if (! data) return;

        ULong64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTNormalizedFSDataFFTW& fsData = data->Of<KTNormalizedFSDataFFTW>();
        UInt_t nComponents = fsData.GetNComponents();

        if (! fWriter->OpenAndVerifyFile()) return;

        for (unsigned iChannel=0; iChannel<nComponents; iChannel++)
        {
            const KTFrequencySpectrumFFTW* spectrum = fsData.GetSpectrumFFTW(iChannel);
            if (spectrum != NULL)
            {
                stringstream conv;
                conv << "histNFSfftw_" << sliceNumber << "_" << iChannel;
                string histName;
                conv >> histName;
                TH1D* powerSpectrum = spectrum->CreateMagnitudeHistogram(histName);
                powerSpectrum->SetDirectory(fWriter->GetFile());
                powerSpectrum->Write();
                KTDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
            }
        }
        return;
    }


    //************************
    // Analytic Associate Data
    //************************

    void KTBasicROOTTypeWriterAnalysis::WriteAnalyticAssociateData(shared_ptr< KTData > data)
    {
        if (! data) return;

        ULong64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTAnalyticAssociateData& aaData = data->Of<KTAnalyticAssociateData>();
        UInt_t nComponents = aaData.GetNComponents();

        if (! fWriter->OpenAndVerifyFile()) return;

        for (UInt_t iPair=0; iPair<nComponents; iPair++)
        {
            const KTTimeSeries* timeSeries = aaData.GetTimeSeries(iPair);
            if (timeSeries != NULL)
            {
                stringstream conv;
                conv << "histAA_" << sliceNumber << "_" << iPair;
                string histName;
                conv >> histName;
                TH1D* tsHist = timeSeries->CreateHistogram(histName);
                stringstream titleStream;
                titleStream << "Analytic Associate " << iPair << " of Slice " << sliceNumber;
                tsHist->SetTitle(titleStream.str().c_str());
                tsHist->SetDirectory(fWriter->GetFile());
                tsHist->Write();
                KTDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
            }
        }
        return;
    }

    //************************
    // Correlation Data
    //************************

    void KTBasicROOTTypeWriterAnalysis::WriteCorrelationData(shared_ptr< KTData > data)
    {
        if (! data) return;

        ULong64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTCorrelationData& corrData = data->Of<KTCorrelationData>();
        UInt_t nComponents = corrData.GetNComponents();

        if (! fWriter->OpenAndVerifyFile()) return;

        for (UInt_t iPair=0; iPair<nComponents; iPair++)
        {
            const KTFrequencySpectrumPolar* spectrum = corrData.GetSpectrumPolar(iPair);
            if (spectrum != NULL)
            {
                stringstream conv;
                conv << "histCorr_" << sliceNumber << "_" << iPair;
                string histName;
                conv >> histName;
                TH1D* corrHist = spectrum->CreateMagnitudeHistogram(histName);
                stringstream titleStream;
                titleStream << "Slice " << sliceNumber << ", Correlation " << iPair << ", "
                        "Channels (" << corrData.GetInputPair(iPair).first << ", " << corrData.GetInputPair(iPair).second << ")";
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

    void KTBasicROOTTypeWriterAnalysis::WriteHoughData(shared_ptr< KTData > data)
    {
        if (! data) return;

        ULong64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTHoughData& houghData = data->Of<KTHoughData>();
        UInt_t nComponents = houghData.GetNComponents();

        if (! fWriter->OpenAndVerifyFile()) return;

        for (UInt_t iPlot=0; iPlot<nComponents; iPlot++)
        {
            stringstream conv;
            conv << "histHT_" << sliceNumber << "_" << iPlot;
            string histName;
            conv >> histName;
            TH2D* swHist = houghData.CreateHistogram(iPlot, histName);
            swHist->SetDirectory(fWriter->GetFile());
            swHist->Write();
            KTDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
        }
        return;
    }

    //************************
    // Gain Variation Data
    //************************

    void KTBasicROOTTypeWriterAnalysis::WriteGainVariationData(shared_ptr< KTData > data)
    {
        if (! data) return;

        ULong64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTGainVariationData& gvData = data->Of<KTGainVariationData>();
        UInt_t nComponents = gvData.GetNComponents();

        if (! fWriter->OpenAndVerifyFile()) return;

        for (UInt_t iPlot=0; iPlot<nComponents; iPlot++)
        {
            stringstream conv;
            conv << "histGV_" << sliceNumber << "_" << iPlot;
            string histName;
            conv >> histName;
            TH1D* gvHist = gvData.CreateGainVariationHistogram(100, iPlot, histName);
            gvHist->SetDirectory(fWriter->GetFile());
            gvHist->Write();
            KTDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");

            /*
            stringstream conv2;
            string splineName;
            conv2 << "splineGV_" << sliceNumber << "_" << iPlot;
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

    //************************
    // WignerVille Data
    //************************

    void KTBasicROOTTypeWriterAnalysis::WriteWignerVilleData(shared_ptr< KTData > data)
    {
        if (! data) return;

        ULong64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTWignerVilleData& wvData = data->Of<KTWignerVilleData>();
        UInt_t nComponents = wvData.GetNComponents();

        if (! fWriter->OpenAndVerifyFile()) return;

        for (UInt_t iPair=0; iPair<nComponents; iPair++)
        {
            const KTFrequencySpectrumFFTW* spectrum = wvData.GetSpectrumFFTW(iPair);
            if (spectrum != NULL)
            {
                stringstream conv;
                conv << "histWV_" << sliceNumber << "_" << iPair;
                string histName;
                conv >> histName;
                TH1D* corrHist = spectrum->CreateMagnitudeHistogram(histName);
                stringstream titleStream;
                titleStream << "Slice " << sliceNumber << ", WignerVille Distribution " << iPair << ", "
                        "Channels (" << wvData.GetInputPair(iPair).first << ", " << wvData.GetInputPair(iPair).second << ")";
                corrHist->SetTitle(titleStream.str().c_str());
                corrHist->SetDirectory(fWriter->GetFile());
                corrHist->Write();
                KTDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
            }
        }
        return;
    }


} /* namespace Katydid */
