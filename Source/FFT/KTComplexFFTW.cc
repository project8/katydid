/*
 * KTComplexFFTW.cc
 *
 *  Created on: Sep 12, 2011
 *      Author: nsoblath
 */

#include "KTComplexFFTW.hh"

#include "KTEggHeader.hh"
#include "KTEvent.hh"
#include "KTFactory.hh"
#include "KTFrequencySpectrumData.hh"
#include "KTTimeSeriesDataFFTW.hh"
#include "KTPStoreNode.hh"

#include <algorithm>
#include <cmath>

using std::copy;
using std::string;
using std::vector;

namespace Katydid
{

    static KTDerivedRegistrar< KTProcessor, KTComplexFFTW > sSimpleFFTRegistrar("simple-fft");

    KTComplexFFTW::KTComplexFFTW() :
            KTFFT(),
            KTProcessor(),
            fFTPlan(),
            fTimeSize(0),
            fInputArray(NULL),
            fOutputArray(NULL),
            fTransformFlag("MEASURE"),
            fIsInitialized(false),
            fFFTSignal()
    {
        fConfigName = "simple-fft";

        RegisterSignal("fft", &fFFTSignal, "void (const KTWriteableData*)");

        RegisterSlot("header", this, &KTComplexFFTW::ProcessHeader, "void (const KTEggHeader*)");
        RegisterSlot("ts-data", this, &KTComplexFFTW::ProcessTimeSeriesData, "void (const KTTimeSeriesDataFFTW*)");
        RegisterSlot("event", this, &KTComplexFFTW::ProcessEvent, "void (KTEvent*)");

        SetupTransformFlagMap();
    }

    KTComplexFFTW::KTComplexFFTW(UInt_t timeSize) :
            KTFFT(),
            KTProcessor(),
            fFTPlan(),
            fTimeSize(timeSize),
            fInputArray((double*) fftw_malloc(sizeof(double) * timeSize)),
            fOutputArray((fftw_complex*) fftw_malloc(sizeof(fftw_complex) * CalculateNFrequencyBins(timeSize))),
            fTransformFlag("MEASURE"),
            fIsInitialized(false),
            fFFTSignal()
    {
        fConfigName = "simple-fft";

        RegisterSignal("fft", &fFFTSignal, "void (const KTWriteableData*)");

        RegisterSlot("header", this, &KTComplexFFTW::ProcessHeader, "void (const KTEggHeader*)");
        RegisterSlot("ts-data", this, &KTComplexFFTW::ProcessTimeSeriesData, "void (const KTTimeSeriesDataFFTW*)");
        RegisterSlot("event", this, &KTComplexFFTW::ProcessEvent, "void (KTEvent*)");

        SetupTransformFlagMap();
    }

    KTComplexFFTW::~KTComplexFFTW()
    {
        if (fInputArray != NULL) fftw_free(fInputArray);
        if (fOutputArray != NULL) fftw_free(fOutputArray);
    }

    Bool_t KTComplexFFTW::Configure(const KTPStoreNode* node)
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

    void KTComplexFFTW::InitializeFFT()
    {
        // fTransformFlag is guaranteed to be valid in the Set method.
        TransformFlagMap::const_iterator iter = fTransformFlagMap.find(fTransformFlag);
        Int_t transformFlag = iter->second;

        fFTPlan = fftw_plan_dft_r2c_1d(fTimeSize, fInputArray, fOutputArray, transformFlag);
        if (fFTPlan != NULL)
        {
            fIsInitialized = true;
        }
        else
        {
            fIsInitialized = false;
        }
        return;
    }

    KTFrequencySpectrumData* KTComplexFFTW::TransformData(const KTTimeSeriesDataFFTW* tsData)
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

        fFFTSignal(newData);

        return newData;
    }

    KTFrequencySpectrum* KTComplexFFTW::Transform(const KTTimeSeriesFFTW* data) const
    {
        UInt_t nTimeBins = (UInt_t)data->GetNBins();
        if (nTimeBins != fTimeSize)
        {
            KTWARN(fftlog_simp, "Number of bins in the data provided does not match the number of bins set for this transform\n"
                    << "   Bin expected: " << fTimeSize << ";   Bins in data: " << data->GetNBins());
            return NULL;
        }

        Double_t freqBinWidth = 1. / (data->GetBinWidth() * (Double_t)nTimeBins);
        Double_t freqMin = -0.5 * freqBinWidth;
        Double_t freqMax = freqBinWidth * ((Double_t)GetFrequencySize() - 0.5);

        copy(data->begin(), data->end(), fInputArray);

        fftw_execute(fFTPlan);

        return ExtractTransformResult(freqMin, freqMax);
    }

    KTFrequencySpectrum* KTComplexFFTW::ExtractTransformResult(Double_t freqMin, Double_t freqMax) const
    {
        UInt_t freqSize = GetFrequencySize();
        Double_t normalization = sqrt(2. / (Double_t)GetTimeSize());

        Double_t tempReal, tempImag;
        KTFrequencySpectrum* newSpect = new KTFrequencySpectrum(freqSize, freqMin, freqMax);
        for (Int_t iPoint = 0; iPoint<freqSize; iPoint++)
        {
            (*newSpect)(iPoint).set_rect(fOutputArray[iPoint][0], fOutputArray[iPoint][1]);
            (*newSpect)(iPoint) *= normalization;
        }

        return newSpect;
    }

    void KTComplexFFTW::ProcessHeader(const KTEggHeader* header)
    {
        SetTimeSize(header->GetRecordSize());
        InitializeFFT();
        return;
    }

    void KTComplexFFTW::ProcessTimeSeriesData(const KTTimeSeriesDataFFTW* tsData)
    {
        KTFrequencySpectrumData* newData = TransformData(tsData);
        tsData->GetEvent()->AddData(newData);
        return;
    }

    void KTComplexFFTW::ProcessEvent(KTEvent* event)
    {
        KTDEBUG(fftlog_simp, "Performing FFT of event " << event->GetEventNumber());
        const KTTimeSeriesDataFFTW* tsData = dynamic_cast< KTTimeSeriesDataFFTW* >(event->GetData(KTTimeSeriesDataFFTW::StaticGetName()));
        if (tsData == NULL)
        {
            KTWARN(fftlog_simp, "No time series data was available in the event");
            return;
        }
        KTFrequencySpectrumData* newData = TransformData(tsData);
        event->AddData(newData);
        return;
    }

    void KTComplexFFTW::SetupTransformFlagMap()
    {
        fTransformFlagMap.clear();
        fTransformFlagMap["ESTIMATE"] = FFTW_ESTIMATE;
        fTransformFlagMap["MEASURE"] = FFTW_MEASURE;
        fTransformFlagMap["PATIENT"] = FFTW_PATIENT;
        fTransformFlagMap["EXHAUSTIVE"] = FFTW_EXHAUSTIVE;
        return;
    }


} /* namespace Katydid */
