/*
 * KTSimpleFFT.cc
 *
 *  Created on: Sep 12, 2011
 *      Author: nsoblath
 */

#include "KTSimpleFFT.hh"

#include "KTEggHeader.hh"
#include "KTTimeSeriesData.hh"
#include "KTPowerSpectrum.hh"
#include "KTPhysicalArray.hh"
#include "KTPStoreNode.hh"

#include "TH1D.h"

using std::string;
using std::vector;

namespace Katydid
{

    KTSimpleFFT::KTSimpleFFT() :
            KTFFT(),
            KTProcessor(),
            KTConfigurable(),
            fTransform(new TFFTRealComplex()),
            fTransformResults(),
            fTransformFlag(string("")),
            fIsInitialized(kFALSE),
            fFreqBinWidth(1.),
            fFFTSignal()
    {
        fConfigName = "simple-fft";

        RegisterSignal("fft", &fFFTSignal);

        RegisterSlot("header", this, &KTSimpleFFT::ProcessHeader);
        RegisterSlot("event", this, &KTSimpleFFT::ProcessEvent);
    }

    KTSimpleFFT::KTSimpleFFT(UInt_t timeSize) :
            KTFFT(),
            KTProcessor(),
            KTConfigurable(),
            fTransform(new TFFTRealComplex((Int_t)timeSize, kFALSE)),
            fTransformResults(),
            fTransformFlag(string("")),
            fIsInitialized(kFALSE),
            fFreqBinWidth(1.),
            fFFTSignal()
    {
        fConfigName = "simple-fft";

        RegisterSignal("fft", &fFFTSignal);

        RegisterSlot("header", this, &KTSimpleFFT::ProcessHeader);
        RegisterSlot("event", this, &KTSimpleFFT::ProcessEvent);
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
        // Config-file settings
        if (node != NULL)
        {
            SetTransformFlag(node->GetData<string>("transform-flag", fTransformFlag));
        }

        // Command-line settings
        //SetTransformFlag(fCLHandler->GetCommandLineValue< string >("transform-flag", fTransformFlag));

        return true;
    }

    void KTSimpleFFT::InitializeFFT()
    {
        fTransform->Init(fTransformFlag.c_str(), 0, NULL);
        fIsInitialized = kTRUE;
        return;
    }

    Bool_t KTSimpleFFT::TransformData(const KTTimeSeriesData* tsData)
    {
        if (! fIsInitialized)
        {
            KTERROR(fftlog_simp, "FFT must be initialized before the transform is performed\n"
                    << "   Please first call InitializeFFT(), then use a TakeData method to set the data, and then finally perform the transform.");
            return kFALSE;
        }

        ClearTransformResults();

        fFreqBinWidth = tsData->GetSampleRate() / (Double_t)tsData->GetRecordSize();

        for (UInt_t iChannel = 0; iChannel < tsData->GetNRecords(); iChannel++)
        {
            KTComplexVector* nextResult = Transform(tsData->GetRecord(iChannel));
            if (nextResult == NULL)
            {
                KTERROR(fftlog_simp, "One of the channels did not transform correctly.");
                return kFALSE;
            }
            AddTransformResult(nextResult);
        }

        return kTRUE;
    }

    void KTSimpleFFT::AddTransformResult(KTComplexVector* result)
    {
        fTransformResults.push_back(result);
        return;
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

    void KTSimpleFFT::ProcessHeader(const KTEggHeader* header)
    {
        SetTimeSize(header->GetRecordSize());
        InitializeFFT();
        return;
    }

    void KTSimpleFFT::ProcessEvent(UInt_t iEvent, const KTEvent* event)
    {
        TransformEvent(event);
        fFFTSignal(iEvent, this);
        return;
    }



} /* namespace Katydid */
