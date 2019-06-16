/*
 * KTBasicROOTTypeWriterSpectrumAnalysis.cc
 *
 *  Created on: Jan 3, 2013
 *      Author: nsoblath
 */

#include "KTBasicROOTTypeWriterSpectrumAnalysis.hh"

#include "KT2ROOT.hh"
#include "KTAnalyticAssociateData.hh"
#include "KTCorrelationData.hh"
#include "KTCorrelationTSData.hh"
#include "KTFrequencySpectrumPolar.hh"
#include "KTGainVariationData.hh"
#include "KTHoughData.hh"
#include "KTKDTreeData.hh"
#include "KTLogger.hh"
#include "KTNormalizedFSData.hh"
#include "KTPowerSpectrumData.hh"
#include "KTSliceHeader.hh"
#include "KTTIFactory.hh"
#include "KTTimeFrequencyPolar.hh"
#include "KTWignerVilleData.hh"
#include "KTWV2DData.hh"
#include "KTChannelAggregatedData.hh"

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
    
    
    static Nymph::KTTIRegistrar< KTBasicROOTTypeWriter, KTBasicROOTTypeWriterSpectrumAnalysis > sBRTWAnalysisRegistrar;
    
    KTBasicROOTTypeWriterSpectrumAnalysis::KTBasicROOTTypeWriterSpectrumAnalysis() :
    KTBasicROOTTypeWriter()
    {
    }
    
    KTBasicROOTTypeWriterSpectrumAnalysis::~KTBasicROOTTypeWriterSpectrumAnalysis()
    {
    }
    
    
    void KTBasicROOTTypeWriterSpectrumAnalysis::RegisterSlots()
    {
        fWriter->RegisterSlot("snr-power", this, &KTBasicROOTTypeWriterSpectrumAnalysis::WriteSNRPower);
        fWriter->RegisterSlot("norm-fs-polar", this, &KTBasicROOTTypeWriterSpectrumAnalysis::WriteNormalizedFSDataPolar);
        fWriter->RegisterSlot("norm-fs-fftw", this, &KTBasicROOTTypeWriterSpectrumAnalysis::WriteNormalizedFSDataFFTW);
        fWriter->RegisterSlot("norm-fs-polar-phase", this, &KTBasicROOTTypeWriterSpectrumAnalysis::WriteNormalizedFSDataPolarPhase);
        fWriter->RegisterSlot("norm-fs-fftw-phase", this, &KTBasicROOTTypeWriterSpectrumAnalysis::WriteNormalizedFSDataFFTWPhase);
        fWriter->RegisterSlot("norm-fs-polar-power", this, &KTBasicROOTTypeWriterSpectrumAnalysis::WriteNormalizedFSDataPolarPower);
        fWriter->RegisterSlot("norm-fs-fftw-power", this, &KTBasicROOTTypeWriterSpectrumAnalysis::WriteNormalizedFSDataFFTWPower);
        fWriter->RegisterSlot("norm-ps", this, &KTBasicROOTTypeWriterSpectrumAnalysis::WriteNormalizedPSData);
        fWriter->RegisterSlot("aa", this, &KTBasicROOTTypeWriterSpectrumAnalysis::WriteAnalyticAssociateData);
        fWriter->RegisterSlot("aa-dist", this, &KTBasicROOTTypeWriterSpectrumAnalysis::WriteAnalyticAssociateDataDistribution);
        fWriter->RegisterSlot("corr", this, &KTBasicROOTTypeWriterSpectrumAnalysis::WriteCorrelationData);
        fWriter->RegisterSlot("corr-dist", this, &KTBasicROOTTypeWriterSpectrumAnalysis::WriteCorrelationDataDistribution);
        //fWriter->RegisterSlot("corr-ts", this, &KTBasicROOTTypeWriterSpectrumAnalysis::WriteCorrelationTSData);
        //fWriter->RegisterSlot("corr-ts-dist", this, &KTBasicROOTTypeWriterSpectrumAnalysis::WriteCorrelationTSDataDistribution);
        fWriter->RegisterSlot("hough", this, &KTBasicROOTTypeWriterSpectrumAnalysis::WriteHoughData);
        fWriter->RegisterSlot("gain-var", this, &KTBasicROOTTypeWriterSpectrumAnalysis::WriteGainVariationData);
        fWriter->RegisterSlot("wv", this, &KTBasicROOTTypeWriterSpectrumAnalysis::WriteWignerVilleData);
        fWriter->RegisterSlot("wv-dist", this, &KTBasicROOTTypeWriterSpectrumAnalysis::WriteWignerVilleDataDistribution);
        fWriter->RegisterSlot("wv-2d", this, &KTBasicROOTTypeWriterSpectrumAnalysis::WriteWV2DData);
        fWriter->RegisterSlot("kd-tree-ss", this, &KTBasicROOTTypeWriterSpectrumAnalysis::WriteKDTreeSparseSpectrogram);
        fWriter->RegisterSlot("agg-fs-fftw", this, &KTBasicROOTTypeWriterSpectrumAnalysis::WriteAggregatedFrequencySpectrumFFTWData);
        fWriter->RegisterSlot("agg-grid-fftw", this, &KTBasicROOTTypeWriterSpectrumAnalysis::WriteAggregatedFrequencySpectrumGrid);
        fWriter->RegisterSlot("agg-ps", this, &KTBasicROOTTypeWriterSpectrumAnalysis::WriteChannelAggregatedPowerSpectrumData);
        fWriter->RegisterSlot("agg-grid-ps", this, &KTBasicROOTTypeWriterSpectrumAnalysis::WriteChannelAggregatedPowerSpectrumGrid);
        fWriter->RegisterSlot("agg-psd", this, &KTBasicROOTTypeWriterSpectrumAnalysis::WriteChannelAggregatedPSDSpectrumData);
        fWriter->RegisterSlot("agg-grid-psd", this, &KTBasicROOTTypeWriterSpectrumAnalysis::WriteChannelAggregatedPSDSpectrumGrid);
#ifdef ENABLE_TUTORIAL
        fWriter->RegisterSlot("lpf-fs-polar", this, &KTBasicROOTTypeWriterSpectrumAnalysis::WriteLowPassFilteredFSDataPolar);
        fWriter->RegisterSlot("lpf-fs-fftw", this, &KTBasicROOTTypeWriterSpectrumAnalysis::WriteLowPassFilteredFSDataFFTW);
        fWriter->RegisterSlot("lpf-ps", this, &KTBasicROOTTypeWriterSpectrumAnalysis::WriteLowPassFilteredPSData);
#endif
        return;
    }
    
    //************************
    // SNR
    //************************
    
    void KTBasicROOTTypeWriterSpectrumAnalysis::WriteSNRPower(Nymph::KTDataPtr data)
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
    
    void KTBasicROOTTypeWriterSpectrumAnalysis::WriteNormalizedFSDataPolar(Nymph::KTDataPtr data)
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
                TH1D* powerSpectrum = KT2ROOT::CreateMagnitudeHistogram(spectrum, histName);
                powerSpectrum->SetDirectory(fWriter->GetFile());
                powerSpectrum->Write();
                KTDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
            }
        }
        return;
    }
    
    void KTBasicROOTTypeWriterSpectrumAnalysis::WriteNormalizedFSDataFFTW(Nymph::KTDataPtr data)
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
                TH1D* powerSpectrum = KT2ROOT::CreateMagnitudeHistogram(spectrum, histName);
                powerSpectrum->SetDirectory(fWriter->GetFile());
                powerSpectrum->Write();
                KTDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
            }
        }
        return;
    }
    
    void KTBasicROOTTypeWriterSpectrumAnalysis::WriteNormalizedFSDataPolarPhase(Nymph::KTDataPtr data)
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
                TH1D* powerSpectrum = KT2ROOT::CreatePhaseHistogram(spectrum, histName);
                powerSpectrum->SetDirectory(fWriter->GetFile());
                powerSpectrum->Write();
                KTDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
            }
        }
        return;
    }
    
    void KTBasicROOTTypeWriterSpectrumAnalysis::WriteNormalizedFSDataFFTWPhase(Nymph::KTDataPtr data)
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
                TH1D* powerSpectrum = KT2ROOT::CreatePhaseHistogram(spectrum, histName);
                powerSpectrum->SetDirectory(fWriter->GetFile());
                powerSpectrum->Write();
                KTDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
            }
        }
        return;
    }
    
    void KTBasicROOTTypeWriterSpectrumAnalysis::WriteNormalizedFSDataPolarPower(Nymph::KTDataPtr data)
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
                TH1D* powerSpectrum = KT2ROOT::CreatePowerHistogram(spectrum, histName);
                powerSpectrum->SetDirectory(fWriter->GetFile());
                powerSpectrum->Write();
                KTDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
            }
        }
        return;
    }
    
    void KTBasicROOTTypeWriterSpectrumAnalysis::WriteNormalizedFSDataFFTWPower(Nymph::KTDataPtr data)
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
                TH1D* powerSpectrum = KT2ROOT::CreatePowerHistogram(spectrum, histName);
                powerSpectrum->SetDirectory(fWriter->GetFile());
                powerSpectrum->Write();
                KTDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
            }
        }
        return;
    }
    
    void KTBasicROOTTypeWriterSpectrumAnalysis::WriteNormalizedPSData(Nymph::KTDataPtr data)
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
    
    void KTBasicROOTTypeWriterSpectrumAnalysis::WriteAnalyticAssociateData(Nymph::KTDataPtr data)
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
    
    void KTBasicROOTTypeWriterSpectrumAnalysis::WriteAnalyticAssociateDataDistribution(Nymph::KTDataPtr data)
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
    
    void KTBasicROOTTypeWriterSpectrumAnalysis::WriteCorrelationData(Nymph::KTDataPtr data)
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
                TH1D* corrHist = KT2ROOT::CreateMagnitudeHistogram(spectrum, histName);
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
    
    void KTBasicROOTTypeWriterSpectrumAnalysis::WriteCorrelationDataDistribution(Nymph::KTDataPtr data)
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
                TH1D* corrHist = KT2ROOT::CreateMagnitudeDistributionHistogram(spectrum, histName);
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
    /*
     void KTBasicROOTTypeWriterSpectrumAnalysis::WriteCorrelationTSData(Nymph::KTDataPtr data)
     {
     if (! data) return;
     
     uint64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();
     
     KTCorrelationTSData& tsData = data->Of<KTCorrelationTSData>();
     unsigned nComponents = tsData.GetNComponents();
     
     if (! fWriter->OpenAndVerifyFile()) return;
     
     for (unsigned iComponent=0; iComponent<nComponents; ++iComponent)
     {
     const KTTimeSeries* timeSeries = tsData.GetTimeSeries(iComponent);
     if (timeSeries != NULL)
     {
     stringstream conv;
     conv << "histCorrTS_" << sliceNumber << "_" << iComponent;
     string histName;
     conv >> histName;
     TH1D* corrHist = KT2ROOT::CreateHistogram(timeSeries, histName);
     corrHist->SetDirectory(fWriter->GetFile());
     corrHist->Write();
     KTDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
     }
     }
     return;
     }
     
     void KTBasicROOTTypeWriterSpectrumAnalysis::WriteCorrelationTSDataDistribution(Nymph::KTDataPtr data)
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
     TH1D* powerSpectrum = KT2ROOT::CreateAmplitudeDistributionHistogram(spectrum, histName);
     powerSpectrum->SetDirectory(fWriter->GetFile());
     powerSpectrum->Write();
     KTDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
     }
     }
     return;
     }
     */
    //************************
    // Hough Transform Data
    //************************
    
    void KTBasicROOTTypeWriterSpectrumAnalysis::WriteHoughData(Nymph::KTDataPtr data)
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
    
    void KTBasicROOTTypeWriterSpectrumAnalysis::WriteGainVariationData(Nymph::KTDataPtr data)
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
            TH1D* gvHist = gvData.CreateGainVariationHistogram(300, iPlot, histName);
            gvHist->SetDirectory(fWriter->GetFile());
            gvHist->Write();
            histName.clear();
            stringstream convVar;
            convVar << "histGVVar_" << sliceNumber << "_" << iPlot;
            convVar >> histName;
            TH1D* gvVarHist = gvData.CreateGainVariationVarianceHistogram(300, iPlot, histName);
            gvVarHist->SetDirectory(fWriter->GetFile());
            gvVarHist->Write();
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
    
    void KTBasicROOTTypeWriterSpectrumAnalysis::WriteWignerVilleData(Nymph::KTDataPtr data)
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
                TH1D* corrHist = KT2ROOT::CreateMagnitudeHistogram(spectrum, histName);
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
    
    void KTBasicROOTTypeWriterSpectrumAnalysis::WriteWignerVilleDataDistribution(Nymph::KTDataPtr data)
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
                TH1D* corrHist = KT2ROOT::CreateMagnitudeDistributionHistogram(spectrum, histName);
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
    
    void KTBasicROOTTypeWriterSpectrumAnalysis::WriteWV2DData(Nymph::KTDataPtr data)
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
    
    //************************
    // KDTree Data
    //************************
    void KTBasicROOTTypeWriterSpectrumAnalysis::WriteKDTreeSparseSpectrogram(Nymph::KTDataPtr data)
    {
        if (! data) return;
        
        static unsigned kdTreeSpectNum = 0;
        
        KTKDTreeData& kdtData = data->Of< KTKDTreeData >();
        
        if (! fWriter->OpenAndVerifyFile()) return;
        
        double xScaling = kdtData.GetXScaling();
        double yScaling = kdtData.GetYScaling();
        for (unsigned iComponent = 0; iComponent < kdtData.GetNComponents(); iComponent++)
        {
            const KTKDTreeData::SetOfPoints& points = kdtData.GetSetOfPoints(iComponent);
            const KTKDTreeData::TreeIndex* index = kdtData.GetTreeIndex(iComponent);
            unsigned pid = 0;
            
            KTDEBUG(publog, "Creating sparse spectrogram graph from KDTree with " << points.size() << " points");
            
            TGraph* grSpectrogram = new TGraph(points.size());
            stringstream conv;
            conv << "grKDTreeSSpect_" << kdTreeSpectNum << "_" << iComponent;
            string grName;
            conv >> grName;
            grSpectrogram->SetName(grName.c_str());
            grSpectrogram->SetTitle("Sparse Spectrogram (from KDTree)");
            
            for (KTKDTreeData::SetOfPoints::const_iterator it = points.begin(); it != points.end(); ++it)
            {
                grSpectrogram->SetPoint(pid, it->fCoords[0] * xScaling, it->fCoords[1] * yScaling);
                ++pid;
            }
            
            fWriter->GetFile()->cd();
            grSpectrogram->Write();
            KTDEBUG(publog, "Graph <" << grName << "> written to ROOT file");
        }
        
        return;
    }
    
    //************************
    // Channel Aggregated Data
    //************************
    
    void KTBasicROOTTypeWriterSpectrumAnalysis::WriteAggregatedFrequencySpectrumFFTWData(Nymph::KTDataPtr data)
    {
        if (! data) return;
        
        uint64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();
        
        KTAggregatedFrequencySpectrumDataFFTW& sumData = data->Of<KTAggregatedFrequencySpectrumDataFFTW>();
        unsigned nComponents = sumData.GetNComponents();
        
        if (! fWriter->OpenAndVerifyFile()) return;
        for (unsigned iChannel=0; iChannel<nComponents; iChannel++)
        {
            KTFrequencySpectrumFFTW* spectrum = sumData.GetSpectrumFFTW(iChannel);
            if (spectrum != NULL) {
                stringstream conv;
                conv << "histAggFFTW_" << sliceNumber<<"_" << iChannel;
                string histName;
                conv >> histName;
                TH1D* aggregatedFrequencySpectrum = KT2ROOT::CreateMagnitudeHistogram(spectrum, histName);
                aggregatedFrequencySpectrum->SetDirectory(fWriter->GetFile());
                
                aggregatedFrequencySpectrum->Write(); //Redundant
                KTDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
            }
        }
        return;
    }
    
    void KTBasicROOTTypeWriterSpectrumAnalysis::WriteAggregatedFrequencySpectrumGrid(Nymph::KTDataPtr data)
    {
        if (! data) return;
        
        uint64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();
        
        KTAggregatedFrequencySpectrumDataFFTW& sumData = data->Of<KTAggregatedFrequencySpectrumDataFFTW>();
        unsigned nComponents = sumData.GetNComponents();
        
        if (! fWriter->OpenAndVerifyFile()) return;
        
        stringstream conv;
        conv << "histAggGridFFTW_" << sliceNumber;
        string histName;
        conv >> histName;
        TH2D* aggregatedGridHistogram = KT2ROOT::CreateGridHistogram(sumData, histName);
        aggregatedGridHistogram->SetDirectory(fWriter->GetFile());
        
        aggregatedGridHistogram->Write(); //Redundant
        KTDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
        return;
    }
    
    
    void KTBasicROOTTypeWriterSpectrumAnalysis::WriteChannelAggregatedPowerSpectrumData(Nymph::KTDataPtr data)
    {
        if (! data) return;
        
        uint64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();
        
        KTAggregatedPowerSpectrumData& sumData = data->Of<KTAggregatedPowerSpectrumData>();
        unsigned nComponents = sumData.GetNComponents();
        
        if (! fWriter->OpenAndVerifyFile()) return;
        
        for (unsigned iChannel=0; iChannel<nComponents; iChannel++)
        {
            KTPowerSpectrum* spectrum = sumData.GetSpectrum(iChannel);
            if (spectrum != NULL)
            {
                spectrum->ConvertToPowerSpectrum();
                stringstream conv;
                conv << "histAggChPS_" << sliceNumber << "_" << iChannel;
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
    
    void KTBasicROOTTypeWriterSpectrumAnalysis::WriteChannelAggregatedPowerSpectrumGrid(Nymph::KTDataPtr data)
    {
        if (! data) return;
        
        uint64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();
        
        KTAggregatedPowerSpectrumData& sumData = data->Of<KTAggregatedPowerSpectrumData>();
        unsigned nComponents = sumData.GetNComponents();
        
        if (! fWriter->OpenAndVerifyFile()) return;
        
        stringstream conv;
        conv << "histAggGridPower_" << sliceNumber;
        string histName;
        conv >> histName;
        TH2D* aggregatedGridHistogram = KT2ROOT::CreateGridHistogram(sumData, histName);
        aggregatedGridHistogram->SetDirectory(fWriter->GetFile());
        
        aggregatedGridHistogram->Write(); //Redundant
        KTDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
        return;
    }
    
    void KTBasicROOTTypeWriterSpectrumAnalysis::WriteChannelAggregatedPSDSpectrumData(Nymph::KTDataPtr data)
    {
        if (! data) return;
        
        uint64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();
        
        KTAggregatedPowerSpectrumData& sumData = data->Of<KTAggregatedPowerSpectrumData>();
        unsigned nComponents = sumData.GetNComponents();
        
        if (! fWriter->OpenAndVerifyFile()) return;
        
        for (unsigned iChannel=0; iChannel<nComponents; iChannel++)
        {
            KTPowerSpectrum* spectrum = sumData.GetSpectrum(iChannel);
            if (spectrum != NULL)
            {
                stringstream conv;
                conv << "histAggChPSD_" << sliceNumber<< "_" << iChannel;
                string histName;
                conv >> histName;
                TH1D* powerSpectrum = KT2ROOT::CreatePowerHistogram(spectrum, histName);
                powerSpectrum->SetDirectory(fWriter->GetFile());
                //        powerSpectrum->Write(); //Redundant
                KTDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
            }
        }
        return;
    }
    
    void KTBasicROOTTypeWriterSpectrumAnalysis::WriteChannelAggregatedPSDSpectrumGrid(Nymph::KTDataPtr data)
    {
        if (! data) return;
        
        uint64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();
        
        KTAggregatedPowerSpectrumData& sumData = data->Of<KTAggregatedPowerSpectrumData>();
        unsigned nComponents = sumData.GetNComponents();
        
        if (! fWriter->OpenAndVerifyFile()) return;
        
        stringstream conv;
        conv << "histAggGridPSD_" << sliceNumber;
        string histName;
        conv >> histName;
        TH2D* aggregatedGridHistogram = KT2ROOT::CreateGridHistogram(sumData, histName);
        aggregatedGridHistogram->SetDirectory(fWriter->GetFile());
        
        aggregatedGridHistogram->Write(); //Redundant
        KTDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
        return;
    }
    
#ifdef ENABLE_TUTORIAL
    void KTBasicROOTTypeWriterSpectrumAnalysis::WriteLowPassFilteredFSDataPolar(Nymph::KTDataPtr data)
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
                TH1D* powerSpectrum = KT2ROOT::CreateMagnitudeHistogram(spectrum, histName);
                powerSpectrum->SetDirectory(fWriter->GetFile());
                powerSpectrum->Write();
                KTDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
            }
        }
        return;
    }
    
    void KTBasicROOTTypeWriterSpectrumAnalysis::WriteLowPassFilteredFSDataFFTW(Nymph::KTDataPtr data)
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
                TH1D* powerSpectrum = KT2ROOT::CreateMagnitudeHistogram(spectrum, histName);
                powerSpectrum->SetDirectory(fWriter->GetFile());
                powerSpectrum->Write();
                KTDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
            }
        }
        return;
    }
    
    void KTBasicROOTTypeWriterSpectrumAnalysis::WriteLowPassFilteredPSData(Nymph::KTDataPtr data)
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
