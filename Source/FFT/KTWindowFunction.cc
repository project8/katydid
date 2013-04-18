/*
 * KTWindowFunction.cc
 *
 *  Created on: Sep 18, 2011
 *      Author: nsoblath
 */

#include "KTWindowFunction.hh"

#include "KTFrequencySpectrumPolar.hh"
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
using std::vector;

namespace Katydid
{
    KTLOGGER(fftlog, "katydid.fft");

    KTWindowFunction::KTWindowFunction(const string& name) :
            KTConfigurable(name),
            fWindowFunction(1),
            fLength(1.),
            fBinWidth(1.),
            fSize(1),
            fLastSetParameter(kSize)
    {
    }

    KTWindowFunction::~KTWindowFunction()
    {
    }

    Bool_t KTWindowFunction::Configure(const KTPStoreNode* node)
    {
        return ConfigureWFSubclass(node);
    }

    Double_t KTWindowFunction::AdaptTo(const KTTimeSeriesData* tsData)
    {
        return SetBinWidth(tsData->GetTimeSeries(0)->GetTimeBinWidth());
    }

    Double_t KTWindowFunction::AdaptTo(const KTTimeSeriesData* tsData, Double_t length)
    {
        return SetBinWidthAndLength(tsData->GetTimeSeries(0)->GetTimeBinWidth(), length);
    }

#ifdef ROOT_FOUND
    TH1D* KTWindowFunction::CreateHistogram(const string& name) const
    {
        //Int_t sideBands = KTMath::Nint(0.2 * fSize);
        //Int_t totalSize = fSize + 2 * sideBands;
        Int_t sideBands = 0;
        Int_t totalSize = fSize;
        Double_t histEdges = fLength / 2. + sideBands * fBinWidth;
        Double_t histMin = 0.;
        Double_t histMax = fLength;
        TH1D* hist = new TH1D(name.c_str(), "Window Function", totalSize, histMin, histMax);
        //for (UInt_t iHistBin=1; iHistBin<=sideBands; iHistBin++)
        //{
        //    hist->SetBinContent(iHistBin, 0.);
        //    hist->SetBinContent(totalSize-iHistBin+1, 0.);
        //}
        for (UInt_t iHistBin=sideBands+1; iHistBin<=fSize+sideBands; iHistBin++)
        {
            hist->SetBinContent(iHistBin, this->GetWeight(iHistBin-sideBands-1));
        }
        hist->SetYTitle("Weight");
        return hist;
    }

    TH1D* KTWindowFunction::CreateFrequencyResponseHistogram(const string& name) const
    {
        //Int_t sideBands = KTMath::Nint(0.2 * fSize);
        //Int_t totalSize = fSize + 2 * sideBands;
        Int_t sideBands = 0;
        Int_t totalSize = fSize;
        KTTimeSeriesReal timeData(totalSize, 0., Double_t(totalSize) * fBinWidth);
        //for (UInt_t iBin=0; iBin<sideBands; iBin++)
        //{
        //    timeData(iBin) = 0.;
        //    timeData(totalSize-iBin-1) = 0.;
        //}
        for (UInt_t iBin=sideBands; iBin<fSize+sideBands; iBin++)
        {
            timeData(iBin) = this->GetWeight(iBin-sideBands);
        }
        KTSimpleFFT fft;
        fft.SetTimeSize(totalSize);
        fft.SetTransformFlag("ESTIMATE");
        fft.InitializeFFT();
        KTFrequencySpectrumPolar* freqSpect = fft.Transform(&timeData);
        TH1D* hist = freqSpect->CreatePowerHistogram(name);
        hist->SetYTitle("Weight");
        delete freqSpect;
        return hist;
    }
#endif

    Double_t KTWindowFunction::SetLength(Double_t length)
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
        RebuildWindowFunction();
        fLastSetParameter = kLength;
        return fBinWidth;
    }

    Double_t KTWindowFunction::SetBinWidth(Double_t bw)
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
        RebuildWindowFunction();
        fLastSetParameter = kBinWidth;
        return fLength;
    }

    Double_t KTWindowFunction::SetBinWidthAndLength(Double_t bw, Double_t length)
    {
        fBinWidth = fabs(bw);
        Double_t prelimNBins = fabs(length) / fBinWidth;
        fSize = (UInt_t)KTMath::Nint(prelimNBins);
        fLength = (Double_t)fSize * fBinWidth;
        RebuildWindowFunction();
        fLastSetParameter = kBinWidth;
        return fLength;
    }

    Double_t KTWindowFunction::SetLengthAndBinWidth(Double_t length, Double_t bw)
    {
        fLength = fabs(length);
        Double_t prelimNBins = fLength / fabs(bw);
        fSize = (UInt_t)KTMath::Nint(prelimNBins);
        fBinWidth = fLength / (Double_t)fSize;
        RebuildWindowFunction();
        fLastSetParameter = kLength;
        return fBinWidth;
    }

    Double_t KTWindowFunction::SetSize(UInt_t wib)
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
        RebuildWindowFunction();
        fLastSetParameter = kSize;
        return fLength;
    }


} /* namespace Katydid */
