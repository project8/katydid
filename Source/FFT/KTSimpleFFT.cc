/*
 * KTSimpleFFT.cc
 *
 *  Created on: Sep 12, 2011
 *      Author: nsoblath
 */

#include "KTSimpleFFT.hh"

#include "KTEggHeader.hh"
#include "KTEvent.hh"
#include "KTFrequencySpectrumData.hh"
#include "KTTimeSeriesData.hh"
//#include "KTPowerSpectrum.hh"
//#include "KTPhysicalArray.hh"
#include "KTPStoreNode.hh"

//#include "TH1D.h"

using std::string;
using std::vector;

namespace Katydid
{

    KTSimpleFFT::KTSimpleFFT() :
            //KTFFT(),
            KTProcessor(),
            KTConfigurable(),
            fTransform(new TFFTRealComplex()),
            //fTransformResults(),
            fTransformFlag(string("")),
            fIsInitialized(kFALSE),
            fFreqBinWidth(1.),
            fFreqMin(-0.5),
            fFreqMax(0.5),
            fFFTSignal()
    {
        fConfigName = "simple-fft";

        RegisterSignal("fft", &fFFTSignal);

        RegisterSlot("header", this, &KTSimpleFFT::ProcessHeader);
        RegisterSlot("event", this, &KTSimpleFFT::ProcessEvent);
    }

    KTSimpleFFT::KTSimpleFFT(UInt_t timeSize) :
            //KTFFT(),
            KTProcessor(),
            KTConfigurable(),
            fTransform(new TFFTRealComplex((Int_t)timeSize, kFALSE)),
            //fTransformResults(),
            fTransformFlag(string("")),
            fIsInitialized(kFALSE),
            fFreqBinWidth(1.),
            fFreqMin(-0.5),
            fFreqMax(0.5),
            fFFTSignal()
    {
        fConfigName = "simple-fft";

        RegisterSignal("fft", &fFFTSignal);

        RegisterSlot("header", this, &KTSimpleFFT::ProcessHeader);
        RegisterSlot("ts-data", this, &KTSimpleFFT::ProcessTimeSeriesData);
        RegisterSlot("event", this, &KTSimpleFFT::ProcessEvent);
    }

    KTSimpleFFT::~KTSimpleFFT()
    {
        delete fTransform;
        //ClearTransformResults();
    }
    /*
    void KTSimpleFFT::ClearTransformResults()
    {
        while (! fTransformResults.empty())
        {
            delete fTransformResults.back();
            fTransformResults.pop_back();
        }
        return;
    }
    */
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

    KTFrequencySpectrumData* KTSimpleFFT::TransformData(const KTTimeSeriesData* tsData)
    {
        if (tsData->GetRecordSize() != GetTimeSize())
        {
            SetTimeSize(tsData->GetRecordSize());
            InitializeFFT();
        }

        if (! fIsInitialized)
        {
            KTERROR(fftlog_simp, "FFT must be initialized before the transform is performed\n"
                    << "   Please first call InitializeFFT(), then use a TakeData method to set the data, and then finally perform the transform.");
            return NULL;
        }

        fFreqBinWidth = tsData->GetSampleRate() / (Double_t)tsData->GetRecordSize();
        fFreqMin = -0.5 * fFreqBinWidth;
        fFreqMax = fFreqBinWidth * ((Double_t)tsData->GetRecordSize()-0.5);

        KTFrequencySpectrumData* newData = new KTFrequencySpectrumData(tsData->GetNChannels());

        for (UInt_t iChannel = 0; iChannel < tsData->GetNChannels(); iChannel++)
        {
            KTFrequencySpectrum* nextResult = Transform(tsData->GetRecord(iChannel));
            if (nextResult == NULL)
            {
                KTERROR(fftlog_simp, "One of the channels did not transform correctly.");
                delete newData;
                return NULL;
            }
            newData->SetSpectrum(nextResult, iChannel);
        }

        KTDEBUG(fftlog_simp, "FFT complete; " << newData->GetNChannels() << " channel(s) transformed");

        newData->SetEvent(tsData->GetEvent());

        //tsData->GetEvent()->AddData(newData);
        fFFTSignal(newData);

        return newData;
    }

    KTFrequencySpectrum* KTSimpleFFT::Transform(const KTTimeSeries* data) const
    {
        unsigned int nBins = (unsigned int)data->GetNBins();
        if (nBins != (unsigned int)fTransform->GetSize())
        {
            KTWARN(fftlog_simp, "Number of bins in the data provided does not match the number of bins set for this transform\n"
                    << "   Bin expected: " << fTransform->GetSize() << ";   Bins in data: " << nBins);
            return NULL;
        }

        for (unsigned int iPoint=0; iPoint<nBins; iPoint++)
        {
            fTransform->SetPoint(iPoint, Double_t((*data)[iPoint]));
        }

        fTransform->Transform();

        return ExtractTransformResult();
    }

    /*
    void KTSimpleFFT::AddTransformResult(KTComplexVector* result)
    {
        fTransformResults.push_back(result);
        return;
    }
    */
    KTFrequencySpectrum* KTSimpleFFT::ExtractTransformResult() const
    {
        UInt_t freqSize = this->GetFrequencySize();
        Double_t normalization = 1. / (Double_t)GetTimeSize();

        Double_t tempReal, tempImag;
        KTFrequencySpectrum* newSpect = new KTFrequencySpectrum(freqSize, fFreqMin, fFreqMax);
        for (Int_t iPoint = 0; iPoint<freqSize; iPoint++)
        {
            fTransform->GetPointComplex(iPoint, tempReal, tempImag);
            (*newSpect)[iPoint].set_rect(tempReal, tempImag);
            (*newSpect)[iPoint] *= normalization;
        }

        return newSpect;
    }
    /*
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
    */

    void KTSimpleFFT::ProcessHeader(const KTEggHeader* header)
    {
        SetTimeSize(header->GetRecordSize());
        InitializeFFT();
        fFreqBinWidth = header->GetAcquisitionRate() / (Double_t)header->GetRecordSize();
        fFreqMin = -0.5 * fFreqBinWidth;
        fFreqMax = fFreqBinWidth * ((Double_t)header->GetRecordSize()-0.5);
        return;
    }

    void KTSimpleFFT::ProcessTimeSeriesData(const KTTimeSeriesData* tsData)
    {
        KTFrequencySpectrumData* newData = TransformData(tsData);
        tsData->GetEvent()->AddData(newData);
        return;
    }

    void KTSimpleFFT::ProcessEvent(KTEvent* event)
    {
        KTDEBUG(fftlog_simp, "Performing FFT of event " << event->GetEventNumber());
        const KTTimeSeriesData* tsData = dynamic_cast< KTTimeSeriesData* >(event->GetData(KTTimeSeriesData::StaticGetName()));
        if (tsData == NULL)
        {
            KTWARN(fftlog_simp, "No time series data was available in the event");
            return;
        }
        KTFrequencySpectrumData* newData = TransformData(tsData);
        event->AddData(newData);
        return;
    }


} /* namespace Katydid */
