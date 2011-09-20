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
            fWidth(1.),
            fBinWidth(1.),
            fWidthInBins(1)
    {
    }

    KTEventWindowFunction::KTEventWindowFunction(const KTEvent* event) :
                    KTWindowFunction(),
                    fWindowFunction(new TArrayD(1)),
                    fWidth(1.),
                    fBinWidth(event->GetBinWidth()),
                    fWidthInBins(1)
    {
        fWidthInBins = TMath::Nint(fWidth / fBinWidth);
    }

    KTEventWindowFunction::~KTEventWindowFunction()
    {
        delete fWindowFunction;
    }

    Double_t KTEventWindowFunction::AdaptTo(const KTEvent* event)
    {
        return this->SetBinWidth(event->GetBinWidth());
    }

    Double_t KTEventWindowFunction::AdaptTo(const KTEvent* event, Double_t width)
    {
        return this->SetBinWidthAndWidth(event->GetBinWidth(), width);
    }

    TH1D* KTEventWindowFunction::CreateHistogram() const
    {
        Int_t sideBands = TMath::Nint(0.2 * fWidthInBins);
        Int_t totalWidthInBins = fWidthInBins + 2 * sideBands;
        Double_t histEdges = fWidth / 2. + sideBands * fBinWidth;
        TH1D* hist = new TH1D("hWindowFunction", "Window Function", totalWidthInBins, -histEdges, histEdges);
        for (Int_t iHistBin=1; iHistBin<=sideBands; iHistBin++)
        {
            hist->SetBinContent(iHistBin, 0.);
            hist->SetBinContent(totalWidthInBins-iHistBin+1, 0.);
        }
        for (Int_t iHistBin=sideBands+1; iHistBin<=fWidthInBins+sideBands; iHistBin++)
        {
            hist->SetBinContent(iHistBin, this->GetWeight(iHistBin-sideBands-1));
        }
        hist->SetYTitle("Weight");
        return hist;
    }

    TH1D* KTEventWindowFunction::CreateFrequencyResponseHistogram() const
    {
        Int_t sideBands = TMath::Nint(0.2 * fWidthInBins);
        Int_t totalWidthInBins = fWidthInBins + 2 * sideBands;
        TArrayD* timeData = new TArrayD(totalWidthInBins);
        for (Int_t iBin=0; iBin<sideBands; iBin++)
        {
            (*timeData)[iBin] = 0.;
            (*timeData)[totalWidthInBins-iBin-1] = 0.;
        }
        for (Int_t iBin=sideBands; iBin<fWidthInBins+sideBands; iBin++)
        {
            (*timeData)[iBin] = this->GetWeight(iBin-sideBands);
        }
        KTSimpleFFT* fft = new KTSimpleFFT(totalWidthInBins);
        fft->SetBinWidth(fBinWidth);
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

    Double_t KTEventWindowFunction::GetWidth() const
    {
        return fWidth;
    }

    Int_t KTEventWindowFunction::GetWidthInBins() const
    {
        return fWidthInBins;
    }

    Double_t KTEventWindowFunction::GetBinWidth() const
    {
        return fBinWidth;
    }

    Double_t KTEventWindowFunction::SetWidth(Double_t width)
    {
        Double_t prelimNBins = width / fBinWidth;
        fWidthInBins = TMath::Nint(prelimNBins);
        fWidth = (Double_t)fWidthInBins * fBinWidth;
        this->RebuildWindowFunction();
        return fWidth;
    }

    Double_t KTEventWindowFunction::SetBinWidth(Double_t bw)
    {
        fBinWidth = bw;
        Double_t prelimNBins = fWidth / fBinWidth;
        fWidthInBins = TMath::Nint(prelimNBins);
        fWidth = (Double_t)fWidthInBins * fBinWidth;
        this->RebuildWindowFunction();
        return fWidth;
    }

    Double_t KTEventWindowFunction::SetBinWidthAndWidth(Double_t bw, Double_t width)
    {
        fBinWidth = bw;
        Double_t prelimNBins = width / fBinWidth;
        fWidthInBins = TMath::Nint(prelimNBins);
        fWidth = (Double_t)fWidthInBins * fBinWidth;
        this->RebuildWindowFunction();
        return fWidth;
    }


} /* namespace Katydid */
