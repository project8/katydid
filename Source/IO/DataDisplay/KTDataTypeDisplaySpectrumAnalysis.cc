/*
 * KTDataTypeDisplaySpectrumAnalysis.cc
 *
 *  Created on: Dec 13, 2013
 *      Author: nsoblath
 */

#include "KT2ROOT.hh"
#include "KTAnalyticAssociateData.hh"
#include "KTCorrelationData.hh"
#include "KTCorrelationTSData.hh"
#include "KTFrequencySpectrumPolar.hh"
#include "KTGainVariationData.hh"
#include "KTHoughData.hh"
#include "KTLogger.hh"
#include "KTNormalizedFSData.hh"
#include "KTSliceHeader.hh"
#include "KTTIFactory.hh"
#include "KTTimeFrequencyPolar.hh"
#include "KTWignerVilleData.hh"
#include "KTWV2DData.hh"

#include "TH1.h"
#include "TH2.h"

#include <sstream>
#include "KTDataTypeDisplaySpectrumAnalysis.hh"



using std::stringstream;
using std::string;

namespace Katydid
{
    KTLOGGER(publog, "katydid.output");


    static Nymph::KTTIRegistrar< KTDataTypeDisplay, KTDataTypeDisplaySpectrumAnalysis > sBRTWAnalysisRegistrar;

    KTDataTypeDisplaySpectrumAnalysis::KTDataTypeDisplaySpectrumAnalysis() :
            KTDataTypeDisplay()
    {
    }

    KTDataTypeDisplaySpectrumAnalysis::~KTDataTypeDisplaySpectrumAnalysis()
    {
    }


    void KTDataTypeDisplaySpectrumAnalysis::RegisterSlots()
    {
        fWriter->RegisterSlot("norm-fs-polar", this, &KTDataTypeDisplaySpectrumAnalysis::DrawNormalizedFSDataPolar);
        fWriter->RegisterSlot("norm-fs-fftw", this, &KTDataTypeDisplaySpectrumAnalysis::DrawNormalizedFSDataFFTW);
        fWriter->RegisterSlot("norm-fs-polar-phase", this, &KTDataTypeDisplaySpectrumAnalysis::DrawNormalizedFSDataPolarPhase);
        fWriter->RegisterSlot("norm-fs-fftw-phase", this, &KTDataTypeDisplaySpectrumAnalysis::DrawNormalizedFSDataFFTWPhase);
        fWriter->RegisterSlot("norm-fs-polar-power", this, &KTDataTypeDisplaySpectrumAnalysis::DrawNormalizedFSDataPolarPower);
        fWriter->RegisterSlot("norm-fs-fftw-power", this, &KTDataTypeDisplaySpectrumAnalysis::DrawNormalizedFSDataFFTWPower);
        fWriter->RegisterSlot("aa", this, &KTDataTypeDisplaySpectrumAnalysis::DrawAnalyticAssociateData);
        fWriter->RegisterSlot("aa-dist", this, &KTDataTypeDisplaySpectrumAnalysis::DrawAnalyticAssociateDataDistribution);
        fWriter->RegisterSlot("corr", this, &KTDataTypeDisplaySpectrumAnalysis::DrawCorrelationData);
        fWriter->RegisterSlot("corr-dist", this, &KTDataTypeDisplaySpectrumAnalysis::DrawCorrelationDataDistribution);
        fWriter->RegisterSlot("corr-ts", this, &KTDataTypeDisplaySpectrumAnalysis::DrawCorrelationTSData);
        fWriter->RegisterSlot("corr-ts-dist", this, &KTDataTypeDisplaySpectrumAnalysis::DrawCorrelationTSDataDistribution);
        fWriter->RegisterSlot("hough", this, &KTDataTypeDisplaySpectrumAnalysis::DrawHoughData);
        fWriter->RegisterSlot("gain-var", this, &KTDataTypeDisplaySpectrumAnalysis::DrawGainVariationData);
        fWriter->RegisterSlot("wv", this, &KTDataTypeDisplaySpectrumAnalysis::DrawWignerVilleData);
        fWriter->RegisterSlot("wv-dist", this, &KTDataTypeDisplaySpectrumAnalysis::DrawWignerVilleDataDistribution);
        fWriter->RegisterSlot("wv-2d", this, &KTDataTypeDisplaySpectrumAnalysis::DrawWV2DData);
        return;
    }


    //************************
    // Frequency Spectrum Data
    //************************

    void KTDataTypeDisplaySpectrumAnalysis::DrawNormalizedFSDataPolar(Nymph::KTDataPtr data)
    {
        if (! data) return;

        ULong64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTNormalizedFSDataPolar& fsData = data->Of<KTNormalizedFSDataPolar>();
        UInt_t nComponents = fsData.GetNComponents();

        if (! fWriter->OpenWindow()) return;

        for (unsigned iChannel=0; iChannel<nComponents; iChannel++)
        {
            const KTFrequencySpectrumPolar* spectrum = fsData.GetSpectrumPolar(iChannel);
            if (spectrum != NULL)
            {
                stringstream conv;
                conv << "histNFSpolar_" << sliceNumber << "_" << iChannel;
                string histName;
                conv >> histName;
                TH1D* magHist = KT2ROOT::CreateMagnitudeHistogram(spectrum, histName);
                fWriter->Draw(magHist);
            }
        }
        return;
    }

    void KTDataTypeDisplaySpectrumAnalysis::DrawNormalizedFSDataFFTW(Nymph::KTDataPtr data)
    {
        if (! data) return;

        ULong64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTNormalizedFSDataFFTW& fsData = data->Of<KTNormalizedFSDataFFTW>();
        UInt_t nComponents = fsData.GetNComponents();

        if (! fWriter->OpenWindow()) return;

        for (unsigned iChannel=0; iChannel<nComponents; iChannel++)
        {
            const KTFrequencySpectrumFFTW* spectrum = fsData.GetSpectrumFFTW(iChannel);
            if (spectrum != NULL)
            {
                stringstream conv;
                conv << "histNFSfftw_" << sliceNumber << "_" << iChannel;
                string histName;
                conv >> histName;
                TH1D* magHist = KT2ROOT::CreateMagnitudeHistogram(spectrum, histName);
                fWriter->Draw(magHist);
            }
        }
        return;
    }

    void KTDataTypeDisplaySpectrumAnalysis::DrawNormalizedFSDataPolarPhase(Nymph::KTDataPtr data)
    {
        if (! data) return;

        ULong64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTNormalizedFSDataPolar& fsData = data->Of<KTNormalizedFSDataPolar>();
        UInt_t nComponents = fsData.GetNComponents();

        if (! fWriter->OpenWindow()) return;

        for (unsigned iChannel=0; iChannel<nComponents; iChannel++)
        {
            const KTFrequencySpectrumPolar* spectrum = fsData.GetSpectrumPolar(iChannel);
            if (spectrum != NULL)
            {
                stringstream conv;
                conv << "histNFSpolarPhase_" << sliceNumber << "_" << iChannel;
                string histName;
                conv >> histName;
                TH1D* phaseHist = KT2ROOT::CreatePhaseHistogram(spectrum, histName);
                fWriter->Draw(phaseHist);
            }
        }
        return;
    }

    void KTDataTypeDisplaySpectrumAnalysis::DrawNormalizedFSDataFFTWPhase(Nymph::KTDataPtr data)
    {
        if (! data) return;

        ULong64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTNormalizedFSDataFFTW& fsData = data->Of<KTNormalizedFSDataFFTW>();
        UInt_t nComponents = fsData.GetNComponents();

        if (! fWriter->OpenWindow()) return;

        for (unsigned iChannel=0; iChannel<nComponents; iChannel++)
        {
            const KTFrequencySpectrumFFTW* spectrum = fsData.GetSpectrumFFTW(iChannel);
            if (spectrum != NULL)
            {
                stringstream conv;
                conv << "histNFSfftwPhase_" << sliceNumber << "_" << iChannel;
                string histName;
                conv >> histName;
                TH1D* phaseHist = KT2ROOT::CreatePhaseHistogram(spectrum, histName);
                fWriter->Draw(phaseHist);
            }
        }
        return;
    }

    void KTDataTypeDisplaySpectrumAnalysis::DrawNormalizedFSDataPolarPower(Nymph::KTDataPtr data)
    {
        if (! data) return;

        ULong64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTNormalizedFSDataPolar& fsData = data->Of<KTNormalizedFSDataPolar>();
        UInt_t nComponents = fsData.GetNComponents();

        if (! fWriter->OpenWindow()) return;

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
                fWriter->Draw(powerSpectrum);
            }
        }
        return;
    }

    void KTDataTypeDisplaySpectrumAnalysis::DrawNormalizedFSDataFFTWPower(Nymph::KTDataPtr data)
    {
        if (! data) return;

        ULong64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTNormalizedFSDataFFTW& fsData = data->Of<KTNormalizedFSDataFFTW>();
        UInt_t nComponents = fsData.GetNComponents();

        if (! fWriter->OpenWindow()) return;

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
                fWriter->Draw(powerSpectrum);
            }
        }
        return;
    }

    //************************
    // Analytic Associate Data
    //************************

    void KTDataTypeDisplaySpectrumAnalysis::DrawAnalyticAssociateData(Nymph::KTDataPtr data)
    {
        if (! data) return;

        ULong64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTAnalyticAssociateData& aaData = data->Of<KTAnalyticAssociateData>();
        UInt_t nComponents = aaData.GetNComponents();

        if (! fWriter->OpenWindow()) return;

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
                fWriter->Draw(tsHist);
            }
        }
        return;
    }

    void KTDataTypeDisplaySpectrumAnalysis::DrawAnalyticAssociateDataDistribution(Nymph::KTDataPtr data)
    {
        if (! data) return;

        ULong64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTAnalyticAssociateData& aaData = data->Of<KTAnalyticAssociateData>();
        UInt_t nComponents = aaData.GetNComponents();

        if (! fWriter->OpenWindow()) return;

        for (UInt_t iPair=0; iPair<nComponents; iPair++)
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
                fWriter->Draw(tsHist);
            }
        }
        return;
    }

    //************************
    // Correlation Data
    //************************

    void KTDataTypeDisplaySpectrumAnalysis::DrawCorrelationData(Nymph::KTDataPtr data)
    {
        if (! data) return;

        ULong64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTCorrelationData& corrData = data->Of<KTCorrelationData>();
        UInt_t nComponents = corrData.GetNComponents();

        if (! fWriter->OpenWindow()) return;

        for (UInt_t iPair=0; iPair<nComponents; iPair++)
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
                fWriter->Draw(corrHist);
            }
        }
        return;
    }

    void KTDataTypeDisplaySpectrumAnalysis::DrawCorrelationDataDistribution(Nymph::KTDataPtr data)
    {
        if (! data) return;

        ULong64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTCorrelationData& corrData = data->Of<KTCorrelationData>();
        UInt_t nComponents = corrData.GetNComponents();

        if (! fWriter->OpenWindow()) return;

        for (UInt_t iPair=0; iPair<nComponents; iPair++)
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
                fWriter->Draw(corrHist);
            }
        }
        return;
    }

    //************************
    // Correlation Data
    //************************

    void KTDataTypeDisplaySpectrumAnalysis::DrawCorrelationTSData(Nymph::KTDataPtr data)
    {
        if (! data) return;

        ULong64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTCorrelationTSData& tsData = data->Of<KTCorrelationTSData>();
        UInt_t nComponents = tsData.GetNComponents();

        if (! fWriter->OpenWindow()) return;

        for (UInt_t iComponent=0; iComponent<nComponents; ++iComponent)
        {
            const KTTimeSeries* spectrum = tsData.GetTimeSeries(iComponent);
            if (spectrum != NULL)
            {
                stringstream conv;
                conv << "histCorrTS_" << sliceNumber << "_" << iComponent;
                string histName;
                conv >> histName;
                TH1D* tsHist = spectrum->CreateHistogram(histName);
                fWriter->Draw(tsHist);
            }
        }
        return;
    }

    void KTDataTypeDisplaySpectrumAnalysis::DrawCorrelationTSDataDistribution(Nymph::KTDataPtr data)
    {
        if (! data) return;

        ULong64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTCorrelationTSData& tsData = data->Of<KTCorrelationTSData>();
        UInt_t nComponents = tsData.GetNComponents();

        if (! fWriter->OpenWindow()) return;

        for (UInt_t iComponent=0; iComponent<nComponents; ++iComponent)
        {
            const KTTimeSeries* spectrum = tsData.GetTimeSeries(iComponent);
            if (spectrum != NULL)
            {
                stringstream conv;
                conv << "histTSDist_" << sliceNumber << "_" << iComponent;
                string histName;
                conv >> histName;
                TH1D* tsDistHist = spectrum->CreateAmplitudeDistributionHistogram(histName);
                fWriter->Draw(tsDistHist);
            }
        }
        return;
    }

    //************************
    // Hough Transform Data
    //************************

    void KTDataTypeDisplaySpectrumAnalysis::DrawHoughData(Nymph::KTDataPtr data)
    {
        if (! data) return;

        ULong64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTHoughData& houghData = data->Of<KTHoughData>();
        UInt_t nComponents = houghData.GetNComponents();

        if (! fWriter->OpenWindow()) return;

        for (UInt_t iPlot=0; iPlot<nComponents; iPlot++)
        {
            stringstream conv;
            conv << "histHT_" << sliceNumber << "_" << iPlot;
            string histName;
            conv >> histName;
            TH2D* swHist = KT2ROOT::CreateHistogram(houghData.GetTransform(iPlot), histName);
            swHist->SetDirectory(NULL);
            swHist->SetTitle("Hough Space");
            swHist->SetXTitle("Angle");
            swHist->SetYTitle("Radius");
            fWriter->Draw(swHist);
        }
        return;
    }

    //************************
    // Gain Variation Data
    //************************

    void KTDataTypeDisplaySpectrumAnalysis::DrawGainVariationData(Nymph::KTDataPtr data)
    {
        if (! data) return;

        ULong64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTGainVariationData& gvData = data->Of<KTGainVariationData>();
        UInt_t nComponents = gvData.GetNComponents();

        if (! fWriter->OpenWindow()) return;

        for (UInt_t iPlot=0; iPlot<nComponents; iPlot++)
        {
            stringstream conv;
            conv << "histGV_" << sliceNumber << "_" << iPlot;
            string histName;
            conv >> histName;
            TH1D* gvHist = gvData.CreateGainVariationHistogram(100, iPlot, histName);
            fWriter->Draw(gvHist);

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

    void KTDataTypeDisplaySpectrumAnalysis::DrawWignerVilleData(Nymph::KTDataPtr data)
    {
        if (! data) return;

        ULong64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTWignerVilleData& wvData = data->Of<KTWignerVilleData>();
        UInt_t nComponents = wvData.GetNComponents();

        if (! fWriter->OpenWindow()) return;

        for (UInt_t iPair=0; iPair<nComponents; iPair++)
        {
            const KTFrequencySpectrumFFTW* spectrum = wvData.GetSpectrumFFTW(iPair);
            if (spectrum != NULL)
            {
                stringstream conv;
                conv << "histWV_" << sliceNumber << "_" << iPair;
                string histName;
                conv >> histName;
                TH1D* wvHist = KT2ROOT::CreateMagnitudeHistogram(spectrum, histName);
                stringstream titleStream;
                titleStream << "Slice " << sliceNumber << ", WignerVille Distribution " << iPair << ", "
                        "Channels (" << wvData.GetInputPair(iPair).first << ", " << wvData.GetInputPair(iPair).second << ")";
                wvHist->SetTitle(titleStream.str().c_str());
                fWriter->Draw(wvHist);
            }
        }
        return;
    }

    void KTDataTypeDisplaySpectrumAnalysis::DrawWignerVilleDataDistribution(Nymph::KTDataPtr data)
    {
        if (! data) return;

        ULong64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTWignerVilleData& wvData = data->Of<KTWignerVilleData>();
        UInt_t nComponents = wvData.GetNComponents();

        if (! fWriter->OpenWindow()) return;

        for (UInt_t iPair=0; iPair<nComponents; iPair++)
        {
            const KTFrequencySpectrumFFTW* spectrum = wvData.GetSpectrumFFTW(iPair);
            if (spectrum != NULL)
            {
                stringstream conv;
                conv << "histWVDist_" << sliceNumber << "_" << iPair;
                string histName;
                conv >> histName;
                TH1D* wvHist = KT2ROOT::CreateMagnitudeDistributionHistogram(spectrum, histName);
                stringstream titleStream;
                titleStream << "Slice " << sliceNumber << ", WignerVille Distribution " << iPair << ", "
                        "Channels (" << wvData.GetInputPair(iPair).first << ", " << wvData.GetInputPair(iPair).second << ")";
                wvHist->SetTitle(titleStream.str().c_str());
                fWriter->Draw(wvHist);
            }
        }
        return;
    }

    void KTDataTypeDisplaySpectrumAnalysis::DrawWV2DData(Nymph::KTDataPtr data)
    {
        if (! data) return;

        ULong64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTWV2DData& fsData = data->Of<KTWV2DData>();
        UInt_t nComponents = fsData.GetNComponents();

        if (! fWriter->OpenWindow()) return;

        for (unsigned iPlot = 0; iPlot < nComponents; iPlot++)
        {
            stringstream conv;
            conv << "histWV2D_" << sliceNumber << "_" << iPlot;
            string histName;
            conv >> histName;
            TH2D* mfsHist = fsData.CreateMagnitudeHistogram(iPlot, histName);
            fWriter->Draw(mfsHist);
        }
        return;
    }


} /* namespace Katydid */
