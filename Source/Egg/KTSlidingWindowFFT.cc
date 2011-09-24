/*
 * KTSlidingWindowFFT.cc
 *
 *  Created on: Sep 12, 2011
 *      Author: nsoblath
 */

#include "KTSlidingWindowFFT.hh"

#include "KTArrayUC.hh"
#include "KTEvent.hh"
#include "KTPowerSpectrum.hh"
#include "KTWindowFunction.hh"

#include "TArray.h"
#include "TH2D.h"
#include "TMath.h"

#include <iostream>

ClassImp(Katydid::KTSlidingWindowFFT);

namespace Katydid
{

    KTSlidingWindowFFT::KTSlidingWindowFFT() :
            KTFFT(),
            fTransform(NULL),
            fTransformFlag(string("")),
            fIsInitialized(kFALSE),
            fIsDataReady(kFALSE),
            fOverlap(0),
            fWindowFunction(NULL),
            fTimeData(NULL),
            fPowerSpectra()
    {
    }

    KTSlidingWindowFFT::~KTSlidingWindowFFT()
    {
        delete fTransform;
        delete fWindowFunction;
        delete fTimeData;
        while (! fPowerSpectra.empty())
        {
            delete fPowerSpectra.back();
            fPowerSpectra.pop_back();
        }
    }

    void KTSlidingWindowFFT::InitializeFFT()
    {
        fTransform->Init(fTransformFlag.c_str(), 0, NULL);
        fIsInitialized = kTRUE;
        return;
    }

    void KTSlidingWindowFFT::TakeData(const KTEvent* event)
    {
        if (! fIsInitialized)
        {
            std::cerr << "Warning from KTSlidingWindowFFT::TakeData: FFT must be initialized before setting the data" << std::endl;
            std::cerr << "   Please first call InitializeFFT, and then use the TakeData method of your choice to set the data" << std::endl;
            return;
        }

        if (event->GetBinWidth() != fWindowFunction->GetBinWidth())
        {
            std::cerr << "Warning from KTSlidingWindowFFT::TakeData: Bin widths are mismatched between the given event and the window function." << std::endl;
            return;
        }

        fFreqBinWidth = event->GetSampleRate() / (Double_t)fWindowFunction->GetSize();

        this->TakeData((TArray*)(event->GetRecord()));
        return;
    }

    void KTSlidingWindowFFT::TakeData(const TArray* array)
    {
        if (! fIsInitialized)
        {
            std::cerr << "Warning from KTSlidingWindowFFT::TakeData: FFT must be initialized before setting the data" << std::endl;
            std::cerr << "   Please first call InitializeFFT, and then use the TakeData method of your choice to set the data" << std::endl;
            return;
        }

        if (fTimeData == NULL) fTimeData = new TArrayD(array->GetSize());
        else if (fTimeData->GetSize() != array->GetSize())
        {
            delete fTimeData;
            fTimeData = new TArrayD(array->GetSize());
        }

        for (Int_t iPoint=0; iPoint<array->GetSize(); iPoint++)
        {
            (*fTimeData)[iPoint] = (Double_t)(array->GetAt(iPoint));
        }
        fIsDataReady = kTRUE;
        return;
    }

    void KTSlidingWindowFFT::Transform()
    {
        if (! fIsInitialized)
        {
            std::cerr << "Warning from KTSlidingWindowFFT::Transform: FFT must be initialized before the transform is performed" << std::endl;
            std::cerr << "   Please first call InitializeFFT(), then use a TakeData method to set the data, and then finally perform the transform." << std::endl;
            return;
        }

        if (! fIsDataReady)
        {
            std::cerr << "Warning from KTSlidingWindowFFT::Transform: The data for the transform is not ready"<< std::endl;
            std::cerr << "   Please first call TakeData, and then perform the transform" << std::endl;
            return;
        }

        while (! fPowerSpectra.empty())
        {
            delete fPowerSpectra.back();
            fPowerSpectra.pop_back();
        }

        if (fWindowFunction->GetSize() < this->GetFullTimeSize())
        {
            Int_t windowShift = fWindowFunction->GetSize() - fOverlap;
            std::cout << "window shift: " << windowShift << std::endl;
            //Int_t nWindows = 1 + TMath::FloorNint((Double_t)(this->GetFullTimeSize() - fWindowFunction->GetSize()) / (Double_t)windowShift);
            Int_t iWindow = 0;
            for (Int_t windowStart=0; windowStart + fWindowFunction->GetSize() <= this->GetFullTimeSize(); windowStart += windowShift)
            {
                //std::cout << "window: " << iWindow+1 << "; window start: " << windowStart << std::endl;
                for (Int_t iPoint=windowStart; iPoint<windowStart+fWindowFunction->GetSize(); iPoint++)
                {
                    fTransform->SetPoint(iPoint, (*fTimeData)[iPoint] * fWindowFunction->GetWeight(iPoint-windowStart));
                }
                fTransform->Transform();
                fPowerSpectra.push_back(this->CreatePowerSpectrum());
                iWindow++;
            }
            std::cout << "FFTs complete; windows used: " << iWindow << std::endl;
        }

        return;
    }

    TH2D* KTSlidingWindowFFT::CreatePowerSpectrumHistogram() const
    {
        if (fPowerSpectra.empty()) return NULL;

        // plot in MHz, instead of Hz
        Double_t freqMult = 1.e-6;

        Double_t effTimeWidth = (Double_t)(fPowerSpectra.size() * fWindowFunction->GetSize() - (fPowerSpectra.size()-1) * fOverlap);
        effTimeWidth *= fWindowFunction->GetBinWidth();
        TH2D* hist = new TH2D("hPowerSpectra", "Power Spectra",
                fPowerSpectra.size(), 0., effTimeWidth,
                this->GetFrequencySize(), -0.5 * fFreqBinWidth * freqMult, fFreqBinWidth * ((Double_t)this->GetFrequencySize()-0.5) * freqMult);
        std::cout << "Frequency axis: " << this->GetFrequencySize() << " bins; range: " << hist->GetYaxis()->GetXmin() << " - " << hist->GetYaxis()->GetXmax() << " MHz" << std::endl;
        std::cout << "Time axis: " << fPowerSpectra.size() << " bins; range: 0 - " << effTimeWidth << " s" << std::endl;

        for (Int_t iBinX=1; iBinX<=(Int_t)fPowerSpectra.size(); iBinX++)
        {
            KTPowerSpectrum* ps = fPowerSpectra[iBinX-1];
            for (Int_t iBinY=1; iBinY<=this->GetFrequencySize(); iBinY++)
            {
                hist->SetBinContent(iBinX, iBinY, ps->GetMagnitudeAt(iBinY-1));
            }
        }

        hist->SetXTitle("Time (s)");
        hist->SetYTitle("Frequency (MHz)");
        return hist;
    }

    KTPowerSpectrum* KTSlidingWindowFFT::CreatePowerSpectrum() const
    {
        // Extract the transformed data
        Int_t freqSize = this->GetFrequencySize();
        Double_t* freqSpecReal = new Double_t [freqSize];
        Double_t* freqSpecImag = new Double_t [freqSize];
        fTransform->GetPointsComplex(freqSpecReal, freqSpecImag);

        KTComplexVector* freqSpec = new KTComplexVector(freqSize, freqSpecReal, freqSpecImag, "R");
        delete [] freqSpecReal; delete [] freqSpecImag;
        (*freqSpec) *= 1. / (Double_t)this->GetTimeSize();

        KTPowerSpectrum* powerSpec = new KTPowerSpectrum();
        powerSpec->TakeFrequencySpectrum(*freqSpec);
        powerSpec->SetBinWidth(fWindowFunction->GetBinWidth());
        delete freqSpec;
        return powerSpec;
    }

    void KTSlidingWindowFFT::SetWindowSize(Int_t nBins)
    {
        fWindowFunction->SetSize(nBins);
        delete fTransform;
        fTransform = new TFFTRealComplex(nBins, kFALSE);
        fIsInitialized = kFALSE;
        return;
    }

    void KTSlidingWindowFFT::SetWindowLength(Double_t wlTime)
    {
        fWindowFunction->SetLength(wlTime);
        delete fTransform;
        fTransform = new TFFTRealComplex(fWindowFunction->GetSize(), kFALSE);
        fIsInitialized = kFALSE;
        return;
    }

    void KTSlidingWindowFFT::SetWindowFunction(KTWindowFunction* wf)
    {
        delete fWindowFunction;
        fWindowFunction = wf;
        fTransform = new TFFTRealComplex(fWindowFunction->GetSize(), kFALSE);
        fIsInitialized = kFALSE;
        return;
    }

    void KTSlidingWindowFFT::SetOverlap(Double_t overlapTime)
    {
        this->SetOverlap(TMath::Nint(overlapTime / fWindowFunction->GetBinWidth()));
        return;
    }

    Int_t KTSlidingWindowFFT::GetWindowSize() const
    {
        return fWindowFunction->GetSize();
    }


} /* namespace Katydid */
