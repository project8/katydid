/*
 * KTSimpleFFT.cc
 *
 *  Created on: Sep 12, 2011
 *      Author: nsoblath
 */

#include "KTSimpleFFT.hh"

//#include "KTArrayUC.hh"
#include "KTEvent.hh"
#include "KTPowerSpectrum.hh"

//#include "TArray.h"
#include "TH1D.h"

#include <iostream>

ClassImp(Katydid::KTSimpleFFT);

namespace Katydid
{

    KTSimpleFFT::KTSimpleFFT() :
            KTFFT(),
            fTransform(new TFFTRealComplex()),
            fTransformResult(NULL),
            fTransformFlag(string("")),
            fIsInitialized(kFALSE),
            fIsDataReady(kFALSE),
            fFreqBinWidth(1.)
    {
    }

    KTSimpleFFT::KTSimpleFFT(UInt_t timeSize) :
            KTFFT(),
            fTransform(new TFFTRealComplex((Int_t)timeSize, kFALSE)),
            fTransformResult(NULL),
            fTransformFlag(string("")),
            fIsInitialized(kFALSE),
            fIsDataReady(kFALSE),
            fFreqBinWidth(1.)
    {
    }

    KTSimpleFFT::~KTSimpleFFT()
    {
        delete fTransform;
        delete fTransformResult;
    }

    void KTSimpleFFT::InitializeFFT()
    {
        fTransform->Init(fTransformFlag.c_str(), 0, NULL);
        fIsInitialized = kTRUE;
        return;
    }

    Bool_t KTSimpleFFT::TakeData(const KTEvent* event)
    {
        unsigned int nBins = event->GetRecordSize();
        if (nBins != (unsigned int)fTransform->GetSize())
        {
            std::cerr << "Warning from KTSimpleFFT::TakeData: Number of bins in the data provided does not match the number of bins set for this transform" << std::endl;
            std::cerr << "   Bin expected: " << fTransform->GetSize() << ";   Bins in data: " << nBins << std::endl;
            return kFALSE;
        }
        if (! fIsInitialized)
        {
            std::cerr << "Warning from KTSimpleFFT::TakeData: FFT must be initialized before setting the data" << std::endl;
            std::cerr << "   Please first call InitializeFFT, and then use the TakeData method of your choice to set the data" << std::endl;
            return kFALSE;
        }

        fFreqBinWidth = event->GetSampleRate() / (Double_t)nBins;
        for (unsigned int iPoint=0; iPoint<nBins; iPoint++)
        {
            fTransform->SetPoint(iPoint, event->GetRecordAt< Double_t >(iPoint));
        }
        fIsDataReady = kTRUE;
        return kTRUE;
    }

    Bool_t KTSimpleFFT::TakeData(const vector< Double_t >& data)
    {
        unsigned int nBins = (unsigned int)data.size();
        if (nBins != (unsigned int)fTransform->GetSize())
        {
            std::cerr << "Warning from KTSimpleFFT::TakeData: Number of bins in the data provided does not match the number of bins set for this transform" << std::endl;
            std::cerr << "   Bin expected: " << fTransform->GetSize() << ";   Bins in data: " << nBins << std::endl;
            return kFALSE;
        }
        if (! fIsInitialized)
        {
            std::cerr << "Warning from KTSimpleFFT::TakeData: FFT must be initialized before setting the data" << std::endl;
            std::cerr << "   Please first call InitializeFFT, and then use the TakeData method of your choice to set the data" << std::endl;
            return kFALSE;
        }

        for (unsigned int iPoint=0; iPoint<nBins; iPoint++)
        {
            fTransform->SetPoint(iPoint, data[iPoint]);
        }
        fIsDataReady = kTRUE;
        return kTRUE;
    }
    /*
    void KTSimpleFFT::TakeData(const TArray* array)
    {
        if (array->GetSize() != fTransform->GetSize())
        {
            std::cerr << "Warning from KTSimpleFFT::TakeData: Number of bins in the data provided does not match the number of bins set for this transform" << std::endl;
            return;
        }
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
    */
    Bool_t KTSimpleFFT::Transform()
    {
        if (! fIsInitialized)
        {
            std::cerr << "Warning from KTSimpleFFT::Transform: FFT must be initialized before the transform is performed" << std::endl;
            std::cerr << "   Please first call InitializeFFT(), then use a TakeData method to set the data, and then finally perform the transform." << std::endl;
            return kFALSE;
        }

        if (! fIsDataReady)
        {
            std::cerr << "Warning from KTSimpleFFT::Transform: The data for the transform is not ready"<< std::endl;
            std::cerr << "   Please first call TakeData, and then perform the transform" << std::endl;
            return kFALSE;
        }

        fTransform->Transform();
        ExtractTransformResult();
        return kTRUE;
    }

    void KTSimpleFFT::ExtractTransformResult()
    {
        UInt_t freqSize = this->GetFrequencySize();
        Double_t* freqSpecReal = new Double_t [freqSize];
        Double_t* freqSpecImag = new Double_t [freqSize];
        fTransform->GetPointsComplex(freqSpecReal, freqSpecImag);

        delete fTransformResult;
        fTransformResult = new KTComplexVector((Int_t)freqSize, freqSpecReal, freqSpecImag, "R");
        delete [] freqSpecReal; delete [] freqSpecImag;
        (*fTransformResult) *= 1. / (Double_t)this->GetTimeSize();

    }

    TH1D* KTSimpleFFT::CreatePowerSpectrumHistogram() const
    {
        KTPowerSpectrum* ps = this->CreatePowerSpectrum();
        TH1D* pshist = ps->CreateMagnitudeHistogram();
        delete ps;
        return pshist;
    }

    KTPowerSpectrum* KTSimpleFFT::CreatePowerSpectrum() const
    {
        KTPowerSpectrum* powerSpec = new KTPowerSpectrum();
        powerSpec->TakeFrequencySpectrum(*fTransformResult);
        powerSpec->SetBinWidth(fFreqBinWidth);
        return powerSpec;
    }


} /* namespace Katydid */
