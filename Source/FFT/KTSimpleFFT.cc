/*
 * KTSimpleFFT.cc
 *
 *  Created on: Sep 12, 2011
 *      Author: nsoblath
 */

#include "KTSimpleFFT.hh"

#include "KTEvent.hh"
#include "KTPowerSpectrum.hh"
#include "KTPhysicalArray.hh"
#include "KTPStoreNode.hh"

#include "TH1D.h"

#include <iostream>

using std::string;
using std::vector;

ClassImp(Katydid::KTSimpleFFT);

namespace Katydid
{

    KTSimpleFFT::KTSimpleFFT() :
            KTFFT(),
            fTransform(new TFFTRealComplex()),
            fTransformResults(),
            fTransformFlag(string("")),
            fIsInitialized(kFALSE),
            fFreqBinWidth(1.)
    {
    }

    KTSimpleFFT::KTSimpleFFT(UInt_t timeSize) :
            KTFFT(),
            fTransform(new TFFTRealComplex((Int_t)timeSize, kFALSE)),
            fTransformResults(),
            fTransformFlag(string("")),
            fIsInitialized(kFALSE),
            fFreqBinWidth(1.)
    {
    }

    KTSimpleFFT::~KTSimpleFFT()
    {
        delete fTransform;
        ClearTransformResults();
    }

    void KTSimpleFFT::ClearTransformResults()
    {
        while (! fTransformResults.empty())
        {
            delete fTransformResults.back();
            fTransformResults.pop_back();
        }
        return;
    }

    Bool_t KTSimpleFFT::Configure(const KTPStoreNode* node)
    {
        SetTransformFlag(node->GetData<string>("transform_flag", ""));
        return true;
    }

    void KTSimpleFFT::InitializeFFT()
    {
        fTransform->Init(fTransformFlag.c_str(), 0, NULL);
        fIsInitialized = kTRUE;
        return;
    }

    void KTSimpleFFT::InitializeFFT()
    {
        fTransform->Init(fTransformFlag.c_str(), 0, NULL);
        fIsInitialized = kTRUE;
        return;
    }

    Bool_t KTSimpleFFT::TransformEvent(const KTEvent* event)
    {
        if (! fIsInitialized)
        {
            std::cerr << "Warning from KTSimpleFFT::Transform: FFT must be initialized before the transform is performed" << std::endl;
            std::cerr << "   Please first call InitializeFFT(), then use a TakeData method to set the data, and then finally perform the transform." << std::endl;
            return kFALSE;
        }

        ClearTransformResults();

        fFreqBinWidth = event->GetSampleRate() / (Double_t)event->GetRecordSize();

        for (UInt_t iChannel = 0; iChannel < event->GetNRecords(); iChannel++)
        {
            KTComplexVector* nextResult = Transform(event->GetRecord(iChannel));
            if (nextResult == NULL)
            {
                std::cerr << "Warning from KTSimpleFFT::TransformEvent: One of the channels did not transform correctly." << std::endl;
                return kFALSE;
            }
            fTransformResults.push_back(nextResult);
        }

        return kTRUE;
    }

    KTComplexVector* KTSimpleFFT::ExtractTransformResult()
    {
        UInt_t freqSize = this->GetFrequencySize();
        Double_t* freqSpecReal = new Double_t [freqSize];
        Double_t* freqSpecImag = new Double_t [freqSize];
        fTransform->GetPointsComplex(freqSpecReal, freqSpecImag);

        KTComplexVector* transformResult = new KTComplexVector((Int_t)freqSize, freqSpecReal, freqSpecImag, "R");
        delete [] freqSpecReal; delete [] freqSpecImag;
        (*transformResult) *= 1. / (Double_t)this->GetTimeSize();
        return transformResult;
    }

    TH1D* KTSimpleFFT::CreatePowerSpectrumHistogram(const string& name, UInt_t channelNum) const
    {
        KTPowerSpectrum* ps = this->CreatePowerSpectrum(channelNum);
        TH1D* pshist = ps->CreateMagnitudeHistogram(name);
        delete ps;
        return pshist;
    }

    TH1D* KTSimpleFFT::CreatePowerSpectrumHistogram(UInt_t channelNum) const
    {
        return CreatePowerSpectrumHistogram("hPowerSpectrum_SimpleFFT", channelNum);
    }

    KTPhysicalArray< 1, Double_t >* KTSimpleFFT::CreatePowerSpectrumPhysArr(UInt_t channelNum) const
    {
        KTPowerSpectrum* ps = this->CreatePowerSpectrum();
        KTPhysicalArray< 1, Double_t >* psPhysArr = ps->CreateMagnitudePhysArr();
        delete ps;
        return psPhysArr;
    }

    KTPowerSpectrum* KTSimpleFFT::CreatePowerSpectrum(UInt_t channelNum) const
    {
        KTPowerSpectrum* powerSpec = new KTPowerSpectrum();
        powerSpec->TakeFrequencySpectrum(*(fTransformResults[channelNum]));
        powerSpec->SetBinWidth(fFreqBinWidth);
        return powerSpec;
    }


} /* namespace Katydid */
