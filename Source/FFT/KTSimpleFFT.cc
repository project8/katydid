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
#include "KTPStoreNode.hh"

#include <algorithm>
#include <cmath>

using std::copy;
using std::string;
using std::vector;

namespace Katydid
{

    KTSimpleFFT::KTSimpleFFT() :
            //KTFFT(),
            KTProcessor(),
            KTConfigurable(),
            fFTPlan(),
            fTimeSize(0),
            fInputArray(NULL),
            fOutputArray(NULL),
            fTransformFlag("MEASURE"),
            fIsInitialized(false),
            fFreqBinWidth(1.),
            fFreqMin(-0.5),
            fFreqMax(0.5),
            fFFTSignal()
    {
        fConfigName = "simple-fft";

        RegisterSignal("fft", &fFFTSignal, "void (const KTWriteableData*)");

        RegisterSlot("header", this, &KTSimpleFFT::ProcessHeader, "void (const KTEggHeader*)");
        RegisterSlot("ts-data", this, &KTSimpleFFT::ProcessTimeSeriesData, "void (const KTTimeSeriesData*)");
        RegisterSlot("event", this, &KTSimpleFFT::ProcessEvent, "void (KTEvent*)");

        SetupTransformFlagMap();
    }

    KTSimpleFFT::KTSimpleFFT(UInt_t timeSize) :
            //KTFFT(),
            KTProcessor(),
            KTConfigurable(),
            fFTPlan(),
            fTimeSize(timeSize),
            fInputArray((double*) fftw_malloc(sizeof(double) * timeSize)),
            fOutputArray((fftw_complex*) fftw_malloc(sizeof(fftw_complex) * CalculateNFrequencyBins(timeSize))),
            fTransformFlag("MEASURE"),
            fIsInitialized(false),
            fFreqBinWidth(1.),
            fFreqMin(-0.5),
            fFreqMax(0.5),
            fFFTSignal()
    {
        fConfigName = "simple-fft";

        RegisterSignal("fft", &fFFTSignal, "void (const KTWriteableData*)");

        RegisterSlot("header", this, &KTSimpleFFT::ProcessHeader, "void (const KTEggHeader*)");
        RegisterSlot("ts-data", this, &KTSimpleFFT::ProcessTimeSeriesData, "void (const KTTimeSeriesData*)");
        RegisterSlot("event", this, &KTSimpleFFT::ProcessEvent, "void (KTEvent*)");

        SetupTransformFlagMap();
    }

    KTSimpleFFT::~KTSimpleFFT()
    {
        if (fInputArray != NULL) fftw_free(fInputArray);
        if (fOutputArray != NULL) fftw_free(fOutputArray);
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

        fFFTSignal(newData);

        return newData;
    }

    KTFrequencySpectrum* KTSimpleFFT::Transform(const KTTimeSeries* data) const
    {
        if ((UInt_t)data->GetNBins() != fTimeSize)
        {
            KTWARN(fftlog_simp, "Number of bins in the data provided does not match the number of bins set for this transform\n"
                    << "   Bin expected: " << fTimeSize << ";   Bins in data: " << data->GetNBins());
            return NULL;
        }

        copy(data->begin(), data->end(), fInputArray);

        fftw_execute(fFTPlan);

        return ExtractTransformResult();
    }

    KTFrequencySpectrum* KTSimpleFFT::ExtractTransformResult() const
    {
        UInt_t freqSize = this->GetFrequencySize();
        Double_t normalization = 1. / sqrt((Double_t)GetTimeSize());

        Double_t tempReal, tempImag;
        KTFrequencySpectrum* newSpect = new KTFrequencySpectrum(freqSize, fFreqMin, fFreqMax);
        for (Int_t iPoint = 0; iPoint<freqSize; iPoint++)
        {
            (*newSpect)[iPoint].set_rect(fOutputArray[iPoint][0], fOutputArray[iPoint][1]);
            (*newSpect)[iPoint] *= normalization;
        }

        return newSpect;
    }

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

    void KTSimpleFFT::SetupTransformFlagMap()
    {
        fTransformFlagMap.clear();
        fTransformFlagMap["ESTIMATE"] = FFTW_ESTIMATE;
        fTransformFlagMap["MEASURE"] = FFTW_MEASURE;
        fTransformFlagMap["PATIENT"] = FFTW_PATIENT;
        fTransformFlagMap["EXHAUSTIVE"] = FFTW_EXHAUSTIVE;
        return;
    }


} /* namespace Katydid */
