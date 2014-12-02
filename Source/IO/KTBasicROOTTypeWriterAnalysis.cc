/*
 * KTBasicROOTTypeWriterAnalysis.cc
 *
 *  Created on: Jan 3, 2013
 *      Author: nsoblath
 */

#include "KTBasicROOTTypeWriterAnalysis.hh"

#include "KT2ROOT.hh"
#include "KTAnalyticAssociateData.hh"
#include "KTCorrelationData.hh"
#include "KTCorrelationTSData.hh"
#include "KTFrequencySpectrumPolar.hh"
#include "KTGainVariationData.hh"
#include "KTHoughData.hh"
#include "KTLogger.hh"
#include "KTNormalizedFSData.hh"
#include "KTPowerSpectrumData.hh"
#include "KTSliceHeader.hh"
#include "KTTIFactory.hh"
#include "KTTimeFrequencyPolar.hh"
#include "KTWignerVilleData.hh"
#include "KTWV2DData.hh"

#ifdef ENABLE_TUTORIAL
#include "KTLowPassFilteredData.hh"
#endif

#include "TH1.h"
#include "TH2.h"

#include <sstream>



using std::stringstream;
using std::string;

namespace Katydid
{
    KTLOGGER(publog, "KTBasicROOTTypeWRiterAnalysis");


    static KTTIRegistrar< KTBasicROOTTypeWriter, KTBasicROOTTypeWriterAnalysis > sBRTWAnalysisRegistrar;

    KTBasicROOTTypeWriterAnalysis::KTBasicROOTTypeWriterAnalysis() :
            KTBasicROOTTypeWriter()
    {
    }

    KTBasicROOTTypeWriterAnalysis::~KTBasicROOTTypeWriterAnalysis()
    {
    }


    void KTBasicROOTTypeWriterAnalysis::RegisterSlots()
    {
        fWriter->RegisterSlot("snr-power", this, &KTBasicROOTTypeWriterAnalysis::WriteSNRPower);
        fWriter->RegisterSlot("norm-fs-polar", this, &KTBasicROOTTypeWriterAnalysis::WriteNormalizedFSDataPolar);
        fWriter->RegisterSlot("norm-fs-fftw", this, &KTBasicROOTTypeWriterAnalysis::WriteNormalizedFSDataFFTW);
        fWriter->RegisterSlot("norm-fs-polar-phase", this, &KTBasicROOTTypeWriterAnalysis::WriteNormalizedFSDataPolarPhase);
        fWriter->RegisterSlot("norm-fs-fftw-phase", this, &KTBasicROOTTypeWriterAnalysis::WriteNormalizedFSDataFFTWPhase);
        fWriter->RegisterSlot("norm-fs-polar-power", this, &KTBasicROOTTypeWriterAnalysis::WriteNormalizedFSDataPolarPower);
        fWriter->RegisterSlot("norm-fs-fftw-power", this, &KTBasicROOTTypeWriterAnalysis::WriteNormalizedFSDataFFTWPower);
        fWriter->RegisterSlot("norm-ps", this, &KTBasicROOTTypeWriterAnalysis::WriteNormalizedPSData);
        fWriter->RegisterSlot("aa", this, &KTBasicROOTTypeWriterAnalysis::WriteAnalyticAssociateData);
        fWriter->RegisterSlot("aa-dist", this, &KTBasicROOTTypeWriterAnalysis::WriteAnalyticAssociateDataDistribution);
        fWriter->RegisterSlot("corr", this, &KTBasicROOTTypeWriterAnalysis::WriteCorrelationData);
        fWriter->RegisterSlot("corr-dist", this, &KTBasicROOTTypeWriterAnalysis::WriteCorrelationDataDistribution);
        fWriter->RegisterSlot("corr-ts", this, &KTBasicROOTTypeWriterAnalysis::WriteCorrelationTSData);
        fWriter->RegisterSlot("corr-ts-dist", this, &KTBasicROOTTypeWriterAnalysis::WriteCorrelationTSDataDistribution);
        fWriter->RegisterSlot("hough", this, &KTBasicROOTTypeWriterAnalysis::WriteHoughData);
        fWriter->RegisterSlot("gain-var", this, &KTBasicROOTTypeWriterAnalysis::WriteGainVariationData);
        fWriter->RegisterSlot("wv", this, &KTBasicROOTTypeWriterAnalysis::WriteWignerVilleData);
        fWriter->RegisterSlot("wv-dist", this, &KTBasicROOTTypeWriterAnalysis::WriteWignerVilleDataDistribution);
        fWriter->RegisterSlot("wv-2d", this, &KTBasicROOTTypeWriterAnalysis::WriteWV2DData);
#ifdef ENABLE_TUTORIAL
        fWriter->RegisterSlot("lpf-fs-polar", this, &KTBasicROOTTypeWriterAnalysis::WriteLowPassFilteredFSDataPolar);
        fWriter->RegisterSlot("lpf-fs-fftw", this, &KTBasicROOTTypeWriterAnalysis::WriteLowPassFilteredFSDataFFTW);
        fWriter->RegisterSlot("lpf-ps", this, &KTBasicROOTTypeWriterAnalysis::WriteLowPassFilteredPSData);
#endif /* ENABLE_TUTORIAL */
        return;
    }

    //************************
    // SNR
    //************************

    void KTBasicROOTTypeWriterAnalysis::WriteSNRPower(KTDataPtr data)
    {
        if (! data) return;

        uint64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTPowerSpectrumData& psData = data->Of< KTPowerSpectrumData >();
        KTGainVariationData& gvData = data->Of< KTGainVariationData >();
        unsigned nComponents = psData.GetNComponents();

        if (! fWriter->OpenAndVerifyFile()) return;

        for (unsigned iComponent=0; iComponent<nComponents; ++iComponent)
        {
            const KTPowerSpectrum* spectrum = psData.GetSpectrum(iComponent);
            if (spectrum != NULL)
            {
                stringstream conv;
                conv << "histSNRPower_" << sliceNumber << "_" << iComponent;
                string histName;
                conv >> histName;
                TH1D* powerSpectrum = KT2ROOT::CreatePowerHistogram(spectrum, histName);
                TH1D* gvHist = gvData.CreateGainVariationHistogram(powerSpectrum->GetNbinsX(), iComponent, "htemp");
                powerSpectrum->Divide(gvHist);
                delete gvHist;
                powerSpectrum->SetDirectory(fWriter->GetFile());
                powerSpectrum->Write();
                KTDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
            }
        }
        return;
    }

    //************************
    // Frequency Spectrum Data
    //************************

    void KTBasicROOTTypeWriterAnalysis::WriteNormalizedFSDataPolar(KTDataPtr data)
    {
        if (! data) return;

        uint64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTNormalizedFSDataPolar& fsData = data->Of<KTNormalizedFSDataPolar>();
        unsigned nComponents = fsData.GetNComponents();

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

    void KTBasicROOTTypeWriterAnalysis::WriteNormalizedFSDataFFTW(KTDataPtr data)
    {
        if (! data) return;

        uint64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTNormalizedFSDataFFTW& fsData = data->Of<KTNormalizedFSDataFFTW>();
        unsigned nComponents = fsData.GetNComponents();

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

    void KTBasicROOTTypeWriterAnalysis::WriteNormalizedFSDataPolarPhase(KTDataPtr data)
    {
        if (! data) return;

        uint64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTNormalizedFSDataPolar& fsData = data->Of<KTNormalizedFSDataPolar>();
        unsigned nComponents = fsData.GetNComponents();

        if (! fWriter->OpenAndVerifyFile()) return;

        for (unsigned iChannel=0; iChannel<nComponents; iChannel++)
        {
            const KTFrequencySpectrumPolar* spectrum = fsData.GetSpectrumPolar(iChannel);
            if (spectrum != NULL)
            {
                stringstream conv;
                conv << "histNFSpolarPhase_" << sliceNumber << "_" << iChannel;
                string histName;
                conv >> histName;
                TH1D* powerSpectrum = spectrum->CreatePhaseHistogram(histName);
                powerSpectrum->SetDirectory(fWriter->GetFile());
                powerSpectrum->Write();
                KTDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
            }
        }
        return;
    }

    void KTBasicROOTTypeWriterAnalysis::WriteNormalizedFSDataFFTWPhase(KTDataPtr data)
    {
        if (! data) return;

        uint64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTNormalizedFSDataFFTW& fsData = data->Of<KTNormalizedFSDataFFTW>();
        unsigned nComponents = fsData.GetNComponents();

        if (! fWriter->OpenAndVerifyFile()) return;

        for (unsigned iChannel=0; iChannel<nComponents; iChannel++)
        {
            const KTFrequencySpectrumFFTW* spectrum = fsData.GetSpectrumFFTW(iChannel);
            if (spectrum != NULL)
            {
                stringstream conv;
                conv << "histNFSfftwPhase_" << sliceNumber << "_" << iChannel;
                string histName;
                conv >> histName;
                TH1D* powerSpectrum = spectrum->CreatePhaseHistogram(histName);
                powerSpectrum->SetDirectory(fWriter->GetFile());
                powerSpectrum->Write();
                KTDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
            }
        }
        return;
    }

    void KTBasicROOTTypeWriterAnalysis::WriteNormalizedFSDataPolarPower(KTDataPtr data)
    {
        if (! data) return;

        uint64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTNormalizedFSDataPolar& fsData = data->Of<KTNormalizedFSDataPolar>();
        unsigned nComponents = fsData.GetNComponents();

        if (! fWriter->OpenAndVerifyFile()) return;

        for (unsigned iChannel=0; iChannel<nComponents; iChannel++)
        {
            const KTFrequencySpectrumPolar* spectrum = fsData.GetSpectrumPolar(iChannel);
            if (spectrum != NULL)
            {
                stringstream conv;
                conv << "histNPSpolar_" << sliceNumber << "_" << iChannel;
                string histName;
                conv >> histName;
                TH1D* powerSpectrum = spectrum->CreatePowerHistogram(histName);
                powerSpectrum->SetDirectory(fWriter->GetFile());
                powerSpectrum->Write();
                KTDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
            }
        }
        return;
    }

    void KTBasicROOTTypeWriterAnalysis::WriteNormalizedFSDataFFTWPower(KTDataPtr data)
    {
        if (! data) return;

        uint64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTNormalizedFSDataFFTW& fsData = data->Of<KTNormalizedFSDataFFTW>();
        unsigned nComponents = fsData.GetNComponents();

        if (! fWriter->OpenAndVerifyFile()) return;

        for (unsigned iChannel=0; iChannel<nComponents; iChannel++)
        {
            const KTFrequencySpectrumFFTW* spectrum = fsData.GetSpectrumFFTW(iChannel);
            if (spectrum != NULL)
            {
                stringstream conv;
                conv << "histNPSfftw_" << sliceNumber << "_" << iChannel;
                string histName;
                conv >> histName;
                TH1D* powerSpectrum = spectrum->CreatePowerHistogram(histName);
                powerSpectrum->SetDirectory(fWriter->GetFile());
                powerSpectrum->Write();
                KTDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
            }
        }
        return;
    }

    void KTBasicROOTTypeWriterAnalysis::WriteNormalizedPSData(KTDataPtr data)
    {
        if (! data) return;

        uint64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTNormalizedPSData& psData = data->Of<KTNormalizedPSData>();
        unsigned nComponents = psData.GetNComponents();

        if (! fWriter->OpenAndVerifyFile()) return;

        for (unsigned iChannel=0; iChannel<nComponents; iChannel++)
        {
            KTPowerSpectrum* spectrum = psData.GetSpectrum(iChannel);
            if (spectrum != NULL)
            {
                spectrum->ConvertToPowerSpectrum();
                stringstream conv;
                conv << "histNPS_" << sliceNumber << "_" << iChannel;
                string histName;
                conv >> histName;
                TH1D* powerSpectrum = KT2ROOT::CreatePowerHistogram(spectrum, histName);
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

    void KTBasicROOTTypeWriterAnalysis::WriteAnalyticAssociateData(KTDataPtr data)
    {
        if (! data) return;

        uint64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTAnalyticAssociateData& aaData = data->Of<KTAnalyticAssociateData>();
        unsigned nComponents = aaData.GetNComponents();

        if (! fWriter->OpenAndVerifyFile()) return;

        for (unsigned iPair=0; iPair<nComponents; iPair++)
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

    void KTBasicROOTTypeWriterAnalysis::WriteAnalyticAssociateDataDistribution(KTDataPtr data)
    {
        if (! data) return;

        uint64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTAnalyticAssociateData& aaData = data->Of<KTAnalyticAssociateData>();
        unsigned nComponents = aaData.GetNComponents();

        if (! fWriter->OpenAndVerifyFile()) return;

        for (unsigned iPair=0; iPair<nComponents; iPair++)
        {
            const KTTimeSeries* timeSeries = aaData.GetTimeSeries(iPair);
            if (timeSeries != NULL)
            {
                stringstream conv;
                conv << "histAADist_" << sliceNumber << "_" << iPair;
                string histName;
                conv >> histName;
                TH1D* tsHist = timeSeries->CreateAmplitudeDistributionHistogram(histName);
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

    void KTBasicROOTTypeWriterAnalysis::WriteCorrelationData(KTDataPtr data)
    {
        if (! data) return;

        uint64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTCorrelationData& corrData = data->Of<KTCorrelationData>();
        unsigned nComponents = corrData.GetNComponents();

        if (! fWriter->OpenAndVerifyFile()) return;

        for (unsigned iPair=0; iPair<nComponents; iPair++)
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

    void KTBasicROOTTypeWriterAnalysis::WriteCorrelationDataDistribution(KTDataPtr data)
    {
        if (! data) return;

        uint64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTCorrelationData& corrData = data->Of<KTCorrelationData>();
        unsigned nComponents = corrData.GetNComponents();

        if (! fWriter->OpenAndVerifyFile()) return;

        for (unsigned iPair=0; iPair<nComponents; iPair++)
        {
            const KTFrequencySpectrumPolar* spectrum = corrData.GetSpectrumPolar(iPair);
            if (spectrum != NULL)
            {
                stringstream conv;
                conv << "histCorrDist_" << sliceNumber << "_" << iPair;
                string histName;
                conv >> histName;
                TH1D* corrHist = spectrum->CreateMagnitudeDistributionHistogram(histName);
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
    // Correlation Data
    //************************

    void KTBasicROOTTypeWriterAnalysis::WriteCorrelationTSData(KTDataPtr data)
    {
        if (! data) return;

        uint64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTCorrelationTSData& tsData = data->Of<KTCorrelationTSData>();
        unsigned nComponents = tsData.GetNComponents();

        if (! fWriter->OpenAndVerifyFile()) return;

        for (unsigned iComponent=0; iComponent<nComponents; ++iComponent)
        {
            const KTTimeSeries* spectrum = tsData.GetTimeSeries(iComponent);
            if (spectrum != NULL)
            {
                stringstream conv;
                conv << "histCorrTS_" << sliceNumber << "_" << iComponent;
                string histName;
                conv >> histName;
                TH1D* powerSpectrum = spectrum->CreateHistogram(histName);
                powerSpectrum->SetDirectory(fWriter->GetFile());
                powerSpectrum->Write();
                KTDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
            }
        }
        return;
    }

    void KTBasicROOTTypeWriterAnalysis::WriteCorrelationTSDataDistribution(KTDataPtr data)
    {
        if (! data) return;

        uint64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTCorrelationTSData& tsData = data->Of<KTCorrelationTSData>();
        unsigned nComponents = tsData.GetNComponents();

        if (! fWriter->OpenAndVerifyFile()) return;

        for (unsigned iComponent=0; iComponent<nComponents; ++iComponent)
        {
            const KTTimeSeries* spectrum = tsData.GetTimeSeries(iComponent);
            if (spectrum != NULL)
            {
                stringstream conv;
                conv << "histTSDist_" << sliceNumber << "_" << iComponent;
                string histName;
                conv >> histName;
                TH1D* powerSpectrum = spectrum->CreateAmplitudeDistributionHistogram(histName);
                powerSpectrum->SetDirectory(fWriter->GetFile());
                powerSpectrum->Write();
                KTDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
            }
        }
        return;
    }

    //************************
    // Hough Transform Data
    //************************

    void KTBasicROOTTypeWriterAnalysis::WriteHoughData(KTDataPtr data)
    {
        if (! data) return;

        uint64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTHoughData& houghData = data->Of<KTHoughData>();
        unsigned nComponents = houghData.GetNComponents();

        if (! fWriter->OpenAndVerifyFile()) return;

        for (unsigned iPlot=0; iPlot<nComponents; iPlot++)
        {
            stringstream conv;
            conv << "histHT_" << sliceNumber << "_" << iPlot;
            string histName;
            conv >> histName;
            TH2D* swHist = KT2ROOT::CreateHistogram(houghData.GetTransform(iPlot), histName);
            swHist->SetDirectory(fWriter->GetFile());
            swHist->SetTitle("Hough Space");
            swHist->SetXTitle("Angle");
            swHist->SetYTitle("Radius");
            swHist->Write();
            KTDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
        }
        return;
    }

    //************************
    // Gain Variation Data
    //************************

    void KTBasicROOTTypeWriterAnalysis::WriteGainVariationData(KTDataPtr data)
    {
        if (! data) return;

        uint64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTGainVariationData& gvData = data->Of<KTGainVariationData>();
        unsigned nComponents = gvData.GetNComponents();

        if (! fWriter->OpenAndVerifyFile()) return;

        for (unsigned iPlot=0; iPlot<nComponents; iPlot++)
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

    void KTBasicROOTTypeWriterAnalysis::WriteWignerVilleData(KTDataPtr data)
    {
        if (! data) return;

        uint64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTWignerVilleData& wvData = data->Of<KTWignerVilleData>();
        unsigned nComponents = wvData.GetNComponents();

        if (! fWriter->OpenAndVerifyFile()) return;

        for (unsigned iPair=0; iPair<nComponents; iPair++)
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

    void KTBasicROOTTypeWriterAnalysis::WriteWignerVilleDataDistribution(KTDataPtr data)
    {
        if (! data) return;

        uint64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTWignerVilleData& wvData = data->Of<KTWignerVilleData>();
        unsigned nComponents = wvData.GetNComponents();

        if (! fWriter->OpenAndVerifyFile()) return;

        for (unsigned iPair=0; iPair<nComponents; iPair++)
        {
            const KTFrequencySpectrumFFTW* spectrum = wvData.GetSpectrumFFTW(iPair);
            if (spectrum != NULL)
            {
                stringstream conv;
                conv << "histWVDist_" << sliceNumber << "_" << iPair;
                string histName;
                conv >> histName;
                TH1D* corrHist = spectrum->CreateMagnitudeDistributionHistogram(histName);
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

    void KTBasicROOTTypeWriterAnalysis::WriteWV2DData(KTDataPtr data)
    {
        if (! data) return;

        uint64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTWV2DData& fsData = data->Of<KTWV2DData>();
        unsigned nComponents = fsData.GetNComponents();

        if (! fWriter->OpenAndVerifyFile()) return;

        for (unsigned iPlot = 0; iPlot < nComponents; iPlot++)
        {
            stringstream conv;
            conv << "histWV2D_" << sliceNumber << "_" << iPlot;
            string histName;
            conv >> histName;
            TH2D* mfsHist = fsData.CreateMagnitudeHistogram(iPlot, histName);
            mfsHist->SetDirectory(fWriter->GetFile());
            mfsHist->Write();
            KTDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
        }
        return;
    }

#ifdef ENABLE_TUTORIAL
    void KTBasicROOTTypeWriterAnalysis::WriteLowPassFilteredFSDataPolar(KTDataPtr data)
    {
        if (! data) return;

        uint64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTLowPassFilteredFSDataPolar& fsData = data->Of<KTLowPassFilteredFSDataPolar>();
        unsigned nComponents = fsData.GetNComponents();

        if (! fWriter->OpenAndVerifyFile()) return;

        for (unsigned iChannel=0; iChannel<nComponents; iChannel++)
        {
            const KTFrequencySpectrumPolar* spectrum = fsData.GetSpectrumPolar(iChannel);
            if (spectrum != NULL)
            {
                stringstream conv;
                conv << "histLPFFSpolar_" << sliceNumber << "_" << iChannel;
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

    void KTBasicROOTTypeWriterAnalysis::WriteLowPassFilteredFSDataFFTW(KTDataPtr data)
    {
        if (! data) return;

        uint64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTLowPassFilteredFSDataFFTW& fsData = data->Of<KTLowPassFilteredFSDataFFTW>();
        unsigned nComponents = fsData.GetNComponents();

        if (! fWriter->OpenAndVerifyFile()) return;

        for (unsigned iChannel=0; iChannel<nComponents; iChannel++)
        {
            const KTFrequencySpectrumFFTW* spectrum = fsData.GetSpectrumFFTW(iChannel);
            if (spectrum != NULL)
            {
                stringstream conv;
                conv << "histLPFFSfftw_" << sliceNumber << "_" << iChannel;
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

    void KTBasicROOTTypeWriterAnalysis::WriteLowPassFilteredPSData(KTDataPtr data)
    {
        if (! data) return;

        uint64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTLowPassFilteredPSData& psData = data->Of<KTLowPassFilteredPSData>();
        unsigned nComponents = psData.GetNComponents();

        if (! fWriter->OpenAndVerifyFile()) return;

        for (unsigned iChannel=0; iChannel<nComponents; iChannel++)
        {
            KTPowerSpectrum* spectrum = psData.GetSpectrum(iChannel);
            if (spectrum != NULL)
            {
                spectrum->ConvertToPowerSpectrum();
                stringstream conv;
                conv << "histLPFPS_" << sliceNumber << "_" << iChannel;
                string histName;
                conv >> histName;
                TH1D* powerSpectrum = KT2ROOT::CreatePowerHistogram(spectrum, histName);
                powerSpectrum->SetDirectory(fWriter->GetFile());
                powerSpectrum->Write();
                KTDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
            }
        }
        return;
    }
#endif /* ENABLE_TUTORIAL */

} /* namespace Katydid */
