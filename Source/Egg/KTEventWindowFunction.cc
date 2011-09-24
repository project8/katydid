/*
 * KTEventWindowFunction.cc
 *
 *  Created on: Sep 18, 2011
 *      Author: nsoblath
 */

#include "KTEventWindowFunction.hh"

#include "KTEvent.hh"
#include "KTPowerSpectrum.hh"
#include "KTSimpleFFT.hh"

#include "TArrayD.h"
#include "TH1.h"
#include "TMath.h"

#include <iostream>

namespace Katydid
{

    KTEventWindowFunction::KTEventWindowFunction() :
            KTWindowFunction(),
            fWindowFunction(new TArrayD(1)),
            fLength(1.),
            fBinWidth(1.),
            fSize(1)
    {
    }

    KTEventWindowFunction::KTEventWindowFunction(const KTEvent* event) :
                    KTWindowFunction(),
                    fWindowFunction(new TArrayD(1)),
                    fLength(1.),
                    fBinWidth(event->GetBinWidth()),
                    fSize(1)
    {
        fSize = TMath::Nint(fLength / fBinWidth);
        fLength = (Double_t)fSize * fBinWidth;
    }

    KTEventWindowFunction::~KTEventWindowFunction()
    {
        delete fWindowFunction;
    }

    Double_t KTEventWindowFunction::AdaptTo(const KTEvent* event)
    {
        return this->SetBinWidth(event->GetBinWidth());
    }

    Double_t KTEventWindowFunction::AdaptTo(const KTEvent* event, Double_t length)
    {
        return this->SetBinWidthAndLength(event->GetBinWidth(), length);
    }

    TH1D* KTEventWindowFunction::CreateHistogram() const
    {
        Int_t sideBands = TMath::Nint(0.2 * fSize);
        Int_t totalSize = fSize + 2 * sideBands;
        Double_t histEdges = fLength / 2. + sideBands * fBinWidth;
        TH1D* hist = new TH1D("hWindowFunction", "Window Function", totalSize, -histEdges, histEdges);
        for (Int_t iHistBin=1; iHistBin<=sideBands; iHistBin++)
        {
            hist->SetBinContent(iHistBin, 0.);
            hist->SetBinContent(totalSize-iHistBin+1, 0.);
        }
        for (Int_t iHistBin=sideBands+1; iHistBin<=fSize+sideBands; iHistBin++)
        {
            hist->SetBinContent(iHistBin, this->GetWeight(iHistBin-sideBands-1));
        }
        hist->SetYTitle("Weight");
        return hist;
    }

    TH1D* KTEventWindowFunction::CreateFrequencyResponseHistogram() const
    {
        Int_t sideBands = TMath::Nint(0.2 * fSize);
        Int_t totalSize = fSize + 2 * sideBands;
        TArrayD* timeData = new TArrayD(totalSize);
        for (Int_t iBin=0; iBin<sideBands; iBin++)
        {
            (*timeData)[iBin] = 0.;
            (*timeData)[totalSize-iBin-1] = 0.;
        }
        for (Int_t iBin=sideBands; iBin<fSize+sideBands; iBin++)
        {
            (*timeData)[iBin] = this->GetWeight(iBin-sideBands);
        }
        KTSimpleFFT* fft = new KTSimpleFFT(totalSize);
        fft->SetFreqBinWidth(fBinWidth);
        fft->SetTransformFlag("ES");
        fft->InitializeFFT();
        fft->TakeData((TArray*)timeData);
        fft->Transform();
        KTPowerSpectrum* ps = fft->CreatePowerSpectrum();
        delete timeData;
        delete fft;
        TH1D* hist = ps->CreateMagnitudeHistogram();
        hist->SetYTitle("Weight");
        delete ps;
        return hist;
    }

    Double_t KTEventWindowFunction::GetLength() const
    {
        return fLength;
    }

    Int_t KTEventWindowFunction::GetSize() const
    {
        return fSize;
    }

    Double_t KTEventWindowFunction::GetBinWidth() const
    {
        return fBinWidth;
    }

    Double_t KTEventWindowFunction::SetLength(Double_t length)
    {
        Double_t prelimNBins = length / fBinWidth;
        fSize = TMath::Nint(prelimNBins);
        fLength = (Double_t)fSize * fBinWidth;
        this->RebuildWindowFunction();
        return fLength;
    }

    Double_t KTEventWindowFunction::SetBinWidth(Double_t bw)
    {
        fBinWidth = bw;
        Double_t prelimNBins = fLength / fBinWidth;
        fSize = TMath::Nint(prelimNBins);
        fLength = (Double_t)fSize * fBinWidth;
        this->RebuildWindowFunction();
        return fLength;
    }

    Double_t KTEventWindowFunction::SetBinWidthAndLength(Double_t bw, Double_t length)
    {
        fBinWidth = bw;
        Double_t prelimNBins = length / fBinWidth;
        fSize = TMath::Nint(prelimNBins);
        fLength = (Double_t)fSize * fBinWidth;
        this->RebuildWindowFunction();
        return fLength;
    }

    Double_t KTEventWindowFunction::SetSize(Int_t wib)
    {
        fSize = wib;
        fLength = (Double_t)fSize * fBinWidth;
        this->RebuildWindowFunction();
        return fLength;
    }


} /* namespace Katydid */
