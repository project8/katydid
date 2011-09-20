/*
 * KTSimpleFFT.cc
 *
 *  Created on: Sep 12, 2011
 *      Author: nsoblath
 */

#include "KTSimpleFFT.hh"

#include "KTArrayUC.hh"
#include "KTEvent.hh"
#include "KTPowerSpectrum.hh"

#include "TArray.h"

#include <iostream>

ClassImp(Katydid::KTSimpleFFT);

namespace Katydid
{

    KTSimpleFFT::KTSimpleFFT() :
            KTFFT(),
            fTransform(NULL),
            fTransformFlag(string("")),
            fIsInitialized(kFALSE),
            fIsDataReady(kFALSE),
            fBinWidth(1.)
    {
    }

    KTSimpleFFT::KTSimpleFFT(Int_t timeSize) :
            KTFFT(),
            fTransform(new TFFTRealComplex(timeSize, kFALSE)),
            fTransformFlag(string("")),
            fIsInitialized(kFALSE),
            fIsDataReady(kFALSE),
            fBinWidth(1.)
    {
    }

    KTSimpleFFT::~KTSimpleFFT()
    {
        delete fTransform;
    }

    void KTSimpleFFT::InitializeFFT()
    {
        fTransform->Init(fTransformFlag.c_str(), 0, NULL);
        fIsInitialized = kTRUE;
        return;
    }

    void KTSimpleFFT::TakeData(const KTEvent* event)
    {
        if (! fIsInitialized)
        {
            std::cerr << "Warning from KTSimpleFFT::TakeData: FFT must be initialized before setting the data" << std::endl;
            std::cerr << "   Please first call InitializeFFT, and then use the TakeData method of your choice to set the data" << std::endl;
            return;
        }

        fBinWidth = event->GetSampleRate() / (Double_t)event->GetRecordSize();
        this->TakeData((TArray*)(event->GetRecord()));
        return;
    }

    void KTSimpleFFT::TakeData(const TArray* array)
    {
        if (! fIsInitialized)
        {
            std::cerr << "Warning from KTSimpleFFT::TakeData: FFT must be initialized before setting the data" << std::endl;
            std::cerr << "   Please first call InitializeFFT, and then use the TakeData method of your choice to set the data" << std::endl;
            return;
        }

        for (Int_t iPoint=0; iPoint<array->GetSize(); iPoint++)
        {
            fTransform->SetPoint(iPoint, (Double_t)(array->GetAt(iPoint)));
        }
        fIsDataReady = kTRUE;
        return;
    }

    void KTSimpleFFT::Transform()
    {
        if (! fIsInitialized)
        {
            std::cerr << "Warning from KTSimpleFFT::Transform: FFT must be initialized before the transform is performed" << std::endl;
            std::cerr << "   Please first call InitializeFFT(), then use a TakeData method to set the data, and then finally perform the transform." << std::endl;
            return;
        }

        fTransform->Transform();
        return;
    }

    KTPowerSpectrum* KTSimpleFFT::CreatePowerSpectrum() const
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
        powerSpec->SetBinWidth(fBinWidth);
        delete freqSpec;
        return powerSpec;
    }


} /* namespace Katydid */
