/*
 * KTWindowFunction.cc
 *
 *  Created on: Sep 18, 2011
 *      Author: nsoblath
 */

#include "KTWindowFunction.hh"

#ifdef FFTW_FOUND
#include "KTComplexFFTW.hh"
#endif

#include "KTFrequencySpectrumFFTW.hh"
#include "KTLogger.hh"
#include "KTMath.hh"
#include "KTPStoreNode.hh"
#include "KTTimeSeriesData.hh"
#include "KTTimeSeriesFFTW.hh"

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
        SetBinWidth(tsData->GetTimeSeries(0)->GetTimeBinWidth());
        return SetSize(tsData->GetTimeSeries(0)->GetNTimeBins());
    }

#ifdef ROOT_FOUND
    TH1D* KTWindowFunction::CreateHistogram(const string& name) const
    {
        TH1D* hist = new TH1D(name.c_str(), "Window Function", fSize, 0, fLength);
        for (UInt_t iHistBin=1; iHistBin<=fSize; iHistBin++)
        {
            hist->SetBinContent(iHistBin, this->GetWeight(iHistBin-1));
        }
        hist->SetYTitle("Weight");
        return hist;
    }

#ifdef FFTW_FOUND
    TH1D* KTWindowFunction::CreateFrequencyResponseHistogram(const string& name) const
    {
        KTTimeSeriesFFTW timeData(fSize, 0., Double_t(fSize) * fBinWidth);
        for (UInt_t iBin=0; iBin<fSize+0; iBin++)
        {
            timeData.SetValue(iBin, GetWeight(iBin));
        }
        KTComplexFFTW fft;
        fft.SetSize(fSize);
        fft.SetTransformFlag("ESTIMATE");
        fft.InitializeFFT();
        KTFrequencySpectrumFFTW* freqSpect = fft.Transform(&timeData);
        TH1D* hist = freqSpect->CreateMagnitudeHistogram(name);
        hist->SetYTitle("Weight");
        delete freqSpect;
        return hist;
    }
#endif
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
