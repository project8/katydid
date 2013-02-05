/*
 * KTBundleWindowFunction.cc
 *
 *  Created on: Sep 18, 2011
 *      Author: nsoblath
 */

#include "KTBundleWindowFunction.hh"

#include "KTFrequencySpectrum.hh"
#include "KTLogger.hh"
#include "KTMath.hh"
#include "KTPowerSpectrum.hh"
#include "KTPStoreNode.hh"
#include "KTSimpleFFT.hh"
#include "KTTimeSeriesData.hh"
#include "KTTimeSeriesReal.hh"

#ifdef ROOT_FOUND
#include "TH1.h"
#endif

#include <cmath>

using std::string;

namespace Katydid
{
    KTLOGGER(fftlog, "katydid.fft");

    KTBundleWindowFunction::KTBundleWindowFunction() :
            KTWindowFunction(),
            fWindowFunction(1),
            fLength(1.),
            fBinWidth(1.),
            fSize(1),
            fLastSetParameter(kSize)
    {
    }

    KTBundleWindowFunction::KTBundleWindowFunction(const KTTimeSeriesData* tsData) :
            KTWindowFunction(),
            fWindowFunction(1),
            fLength(1.),
            fBinWidth(tsData->GetTimeSeries(0)->GetTimeBinWidth()),
            fSize(1),
            fLastSetParameter(kSize)
    {
        fSize = (UInt_t)KTMath::Nint(fLength / fBinWidth);
        fLength = (Double_t)fSize * fBinWidth;
    }

    KTBundleWindowFunction::~KTBundleWindowFunction()
    {
    }

    Bool_t KTBundleWindowFunction::ConfigureWindowFunctionSubclass(const KTPStoreNode* node)
    {
        return ConfigureEventWindowFunctionSubclass(node);
    }

    Double_t KTBundleWindowFunction::AdaptTo(const KTTimeSeriesData* tsData)
    {
        return this->SetBinWidth(tsData->GetTimeSeries(0)->GetTimeBinWidth());
    }

    Double_t KTBundleWindowFunction::AdaptTo(const KTTimeSeriesData* tsData, Double_t length)
    {
        return this->SetBinWidthAndLength(tsData->GetTimeSeries(0)->GetTimeBinWidth(), length);
    }

#ifdef ROOT_FOUND
    TH1D* KTBundleWindowFunction::CreateHistogram(const string& name) const
    {
        Int_t sideBands = KTMath::Nint(0.2 * fSize);
        Int_t totalSize = fSize + 2 * sideBands;
        Double_t histEdges = fLength / 2. + sideBands * fBinWidth;
        TH1D* hist = new TH1D(name.c_str(), "Window Function", totalSize, -histEdges, histEdges);
        for (UInt_t iHistBin=1; iHistBin<=sideBands; iHistBin++)
        {
            hist->SetBinContent(iHistBin, 0.);
            hist->SetBinContent(totalSize-iHistBin+1, 0.);
        }
        for (UInt_t iHistBin=sideBands+1; iHistBin<=fSize+sideBands; iHistBin++)
        {
            hist->SetBinContent(iHistBin, this->GetWeight(iHistBin-sideBands-1));
        }
        hist->SetYTitle("Weight");
        return hist;
    }

    TH1D* KTBundleWindowFunction::CreateHistogram() const
    {
        return CreateHistogram("hWindowFunction");
    }

    TH1D* KTBundleWindowFunction::CreateFrequencyResponseHistogram(const string& name) const
    {
        Int_t sideBands = KTMath::Nint(0.2 * fSize);
        Int_t totalSize = fSize + 2 * sideBands;
        KTTimeSeriesReal timeData(totalSize, 0., totalSize * fBinWidth);
        for (UInt_t iBin=0; iBin<sideBands; iBin++)
        {
            timeData(iBin) = 0.;
            timeData(totalSize-iBin-1) = 0.;
        }
        for (UInt_t iBin=sideBands; iBin<fSize+sideBands; iBin++)
        {
            timeData(iBin) = this->GetWeight(iBin-sideBands);
        }
        KTSimpleFFT fft;
        fft.SetTimeSize(totalSize);
        fft.SetTransformFlag("ESTIMATE");
        fft.InitializeFFT();
        KTFrequencySpectrum* freqSpect = fft.Transform(&timeData);
        TH1D* hist = freqSpect->CreatePowerHistogram(name);
        hist->SetYTitle("Weight");
        delete freqSpect;
        return hist;
    }

    TH1D* KTBundleWindowFunction::CreateFrequencyResponseHistogram() const
    {
        return CreateFrequencyResponseHistogram("hFrequencyResponse");
    }
#endif

    Double_t KTBundleWindowFunction::GetLength() const
    {
        return fLength;
    }

    UInt_t KTBundleWindowFunction::GetSize() const
    {
        return fSize;
    }

    Double_t KTBundleWindowFunction::GetBinWidth() const
    {
        return fBinWidth;
    }

    Double_t KTBundleWindowFunction::SetLength(Double_t length)
    {
        fLength = fabs(length);
        if (fLastSetParameter == kBinWidth)
        {
            // Priority is to preserve the bin width, but it might not be an even divisor of the new length
            Double_t prelimSize = fLength / fBinWidth;
            fSize = (UInt_t)KTMath::Nint(prelimSize);
            fBinWidth = fLength / (Double_t)fSize;
        }
        else if (fLastSetParameter == kSize || fLastSetParameter == kLength)
        {
            fBinWidth = fLength / fSize;
        }
        this->RebuildWindowFunction();
        fLastSetParameter = kLength;
        return fBinWidth;
    }

    Double_t KTBundleWindowFunction::SetBinWidth(Double_t bw)
    {
        fBinWidth = fabs(bw);
        if (fLastSetParameter == kSize || fLastSetParameter == kBinWidth)
        {
            fLength = fSize * fBinWidth;
        }
        else if (fLastSetParameter == kLength)
        {
            // Priority is to preserve the length, but it might not be an even multiple of the new bin width
            Double_t prelimNBins = fLength / fBinWidth;
            fSize = (UInt_t)KTMath::Nint(prelimNBins);
            fLength = (Double_t)fSize * fBinWidth;
        }
        this->RebuildWindowFunction();
        fLastSetParameter = kBinWidth;
        return fLength;
    }

    Double_t KTBundleWindowFunction::SetBinWidthAndLength(Double_t bw, Double_t length)
    {
        fBinWidth = fabs(bw);
        Double_t prelimNBins = fabs(length) / fBinWidth;
        fSize = (UInt_t)KTMath::Nint(prelimNBins);
        fLength = (Double_t)fSize * fBinWidth;
        this->RebuildWindowFunction();
        fLastSetParameter = kBinWidth;
        return fLength;
    }

    Double_t KTBundleWindowFunction::SetLengthAndBinWidth(Double_t length, Double_t bw)
    {
        fLength = fabs(length);
        Double_t prelimNBins = fLength / fabs(bw);
        fSize = (UInt_t)KTMath::Nint(prelimNBins);
        fBinWidth = fLength / (Double_t)fSize;
        this->RebuildWindowFunction();
        fLastSetParameter = kLength;
        return fBinWidth;
    }

    Double_t KTBundleWindowFunction::SetSize(UInt_t wib)
    {
        fSize = wib;
        if (fLastSetParameter == kBinWidth || fLastSetParameter == kSize)
        {
            fLength = (Double_t)fSize * fBinWidth;
        }
        else if (fLastSetParameter == kLength)
        {
            fBinWidth = fLength / (Double_t)fSize;
        }
        this->RebuildWindowFunction();
        fLastSetParameter = kSize;
        return fLength;
    }


} /* namespace Katydid */
