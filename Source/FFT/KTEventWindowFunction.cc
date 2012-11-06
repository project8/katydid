/*
 * KTEventWindowFunction.cc
 *
 *  Created on: Sep 18, 2011
 *      Author: nsoblath
 */

#include "KTEventWindowFunction.hh"

#include "KTFrequencySpectrum.hh"
#include "KTLogger.hh"
#include "KTMath.hh"
#include "KTPowerSpectrum.hh"
#include "KTPStoreNode.hh"
#include "KTSimpleFFT.hh"
#include "KTTimeSeriesChannelData.hh"
#include "KTTimeSeriesReal.hh"

#ifdef ROOT_FOUND
#include "TH1.h"
#endif

#include <cmath>

using std::string;

namespace Katydid
{
    KTLOGGER(fftlog, "katydid.fft");

    KTEventWindowFunction::KTEventWindowFunction() :
            KTWindowFunction(),
            fWindowFunction(1),
            fLength(1.),
            fBinWidth(1.),
            fSize(1)
    {
    }

    KTEventWindowFunction::KTEventWindowFunction(const KTTimeSeriesChannelData* tsData) :
            KTWindowFunction(),
            fWindowFunction(1),
            fLength(1.),
            fBinWidth(tsData->GetBinWidth()),
            fSize(1)
    {
        fSize = (UInt_t)KTMath::Nint(fLength / fBinWidth);
        fLength = (Double_t)fSize * fBinWidth;
    }

    KTEventWindowFunction::~KTEventWindowFunction()
    {
    }

    Bool_t KTEventWindowFunction::ConfigureWindowFunctionSubclass(const KTPStoreNode* node)
    {
        return ConfigureEventWindowFunctionSubclass(node);
    }

    Double_t KTEventWindowFunction::AdaptTo(const KTTimeSeriesChannelData* tsData)
    {
        return this->SetBinWidth(tsData->GetBinWidth());
    }

    Double_t KTEventWindowFunction::AdaptTo(const KTTimeSeriesChannelData* tsData, Double_t length)
    {
        return this->SetBinWidthAndLength(tsData->GetBinWidth(), length);
    }

#ifdef ROOT_FOUND
    TH1D* KTEventWindowFunction::CreateHistogram(const string& name) const
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

    TH1D* KTEventWindowFunction::CreateHistogram() const
    {
        return CreateHistogram("hWindowFunction");
    }

    TH1D* KTEventWindowFunction::CreateFrequencyResponseHistogram(const string& name) const
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
        KTSimpleFFT fft(totalSize);
        fft.SetTransformFlag("ESTIMATE");
        fft.InitializeFFT();
        KTFrequencySpectrum* freqSpect = fft.Transform(&timeData);
        TH1D* hist = freqSpect->CreatePowerHistogram(name);
        hist->SetYTitle("Weight");
        delete freqSpect;
        return hist;
    }

    TH1D* KTEventWindowFunction::CreateFrequencyResponseHistogram() const
    {
        return CreateFrequencyResponseHistogram("hFrequencyResponse");
    }
#endif

    Double_t KTEventWindowFunction::GetLength() const
    {
        return fLength;
    }

    UInt_t KTEventWindowFunction::GetSize() const
    {
        return fSize;
    }

    Double_t KTEventWindowFunction::GetBinWidth() const
    {
        return fBinWidth;
    }

    Double_t KTEventWindowFunction::SetLength(Double_t length)
    {
        fLength = fabs(length);
        Double_t prelimNBins = fLength / fBinWidth;
        fSize = (UInt_t)KTMath::Nint(prelimNBins);
        fBinWidth = fLength / (Double_t)fSize;
        this->RebuildWindowFunction();
        return fBinWidth;
    }

    Double_t KTEventWindowFunction::SetBinWidth(Double_t bw)
    {
        fBinWidth = fabs(bw);
        Double_t prelimNBins = fLength / fBinWidth;
        fSize = (UInt_t)KTMath::Nint(prelimNBins);
        fLength = (Double_t)fSize * fBinWidth;
        this->RebuildWindowFunction();
        KTDEBUG(fftlog, "setting the bin width: " << fSize << "  " << fBinWidth << "  " << fLength);
        return fLength;
    }

    Double_t KTEventWindowFunction::SetBinWidthAndLength(Double_t bw, Double_t length)
    {
        fBinWidth = fabs(bw);
        Double_t prelimNBins = fabs(length) / fBinWidth;
        fSize = (UInt_t)KTMath::Nint(prelimNBins);
        fLength = (Double_t)fSize * fBinWidth;
        this->RebuildWindowFunction();
        return fLength;
    }

    Double_t KTEventWindowFunction::SetLengthAndBinWidth(Double_t length, Double_t bw)
    {
        fLength = fabs(length);
        Double_t prelimNBins = fLength / fabs(bw);
        fSize = (UInt_t)KTMath::Nint(prelimNBins);
        fBinWidth = fLength / (Double_t)fSize;
        this->RebuildWindowFunction();
        return fBinWidth;
    }

    Double_t KTEventWindowFunction::SetSize(UInt_t wib)
    {
        fSize = wib;
        fLength = (Double_t)fSize * fBinWidth;
        this->RebuildWindowFunction();
        return fLength;
    }


} /* namespace Katydid */
