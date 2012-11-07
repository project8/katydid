/*
 * KTSimpleFFT.cc
 *
 *  Created on: Sep 12, 2011
 *      Author: nsoblath
 */

#include "KTSimpleFFT.hh"

#include "KTEggHeader.hh"
#include "KTEvent.hh"
#include "KTFactory.hh"
#include "KTFrequencySpectrumData.hh"
#include "KTTimeSeriesChannelData.hh"
#include "KTTimeSeriesPairedData.hh"
#include "KTTimeSeriesReal.hh"
#include "KTPStoreNode.hh"

#include <algorithm>
#include <cmath>

using std::copy;
using std::string;
using std::vector;

namespace Katydid
{

    static KTDerivedRegistrar< KTProcessor, KTSimpleFFT > sSimpleFFTRegistrar("simple-fft");

    KTSimpleFFT::KTSimpleFFT() :
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

        RegisterSlot("header", this, &KTSimpleFFT::ProcessHeader, "void (const KTEggHeader*)");
        RegisterSlot("ts-data", this, &KTSimpleFFT::ProcessTimeSeriesData, "void (const KTTimeSeriesData*)");
        RegisterSlot("event", this, &KTSimpleFFT::ProcessEvent, "void (KTEvent*)");
        RegisterSlot("event-named-data", this, &KTSimpleFFT::ProcessEvent, "void (KTEvent*, const string&)");

        SetupTransformFlagMap();
    }

    KTSimpleFFT::KTSimpleFFT(UInt_t timeSize) :
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

        RegisterSlot("header", this, &KTSimpleFFT::ProcessHeader, "void (const KTEggHeader*)");
        RegisterSlot("ts-data", this, &KTSimpleFFT::ProcessTimeSeriesData, "void (const KTTimeSeriesData*)");
        RegisterSlot("event", this, &KTSimpleFFT::ProcessEvent, "void (KTEvent*)");

        SetupTransformFlagMap();
    }

    KTSimpleFFT::~KTSimpleFFT()
    {
        if (fInputArray != NULL) fftw_free(fInputArray);
        if (fOutputArray != NULL) fftw_free(fOutputArray);
        fftw_destroy_plan(fFTPlan);
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
            KTDEBUG(fftlog_simp, "FFT is initialized" << '\n' <<
                    "\tTime-domain size: " << fTimeSize << '\n' <<
                    "\tFrequency-domain size: " << GetFrequencySize());
        }
        else
        {
            fIsInitialized = false;
        }
        return;
    }

    KTFrequencySpectrumData* KTSimpleFFT::TransformData(const KTTimeSeriesData* tsData)
    {
        if (tsData->GetNTimeSeries() < 1)
        {
            KTWARN(fftlog_simp, "Data has no channels!");
            return NULL;
        }
        if (tsData->GetTimeSeries(0)->GetNTimeBins() != GetTimeSize())
        {
            SetTimeSize(tsData->GetTimeSeries(0)->GetNTimeBins());
            InitializeFFT();
        }

        if (! fIsInitialized)
        {
            KTERROR(fftlog_simp, "FFT must be initialized before the transform is performed\n"
                    << "   Please first call InitializeFFT(), then use a TakeData method to set the data, and then finally perform the transform.");
            return NULL;
        }

        KTFrequencySpectrumData* newData = new KTFrequencySpectrumData(tsData->GetNTimeSeries());

        for (UInt_t iChannel = 0; iChannel < tsData->GetNTimeSeries(); iChannel++)
        {
            const KTTimeSeriesReal* nextInput = dynamic_cast< const KTTimeSeriesReal* >(tsData->GetTimeSeries(iChannel));
            if (nextInput == NULL)
            {
                KTERROR(fftlog_simp, "Incorrect time series type: time series did not cast to KTTimeSeriesReal.");
                delete newData;
                return NULL;
            }
            KTFrequencySpectrum* nextResult = Transform(nextInput);
            if (nextResult == NULL)
            {
                KTERROR(fftlog_simp, "One of the channels did not transform correctly.");
                delete newData;
                return NULL;
            }
            newData->SetSpectrum(nextResult, iChannel);
        }

        KTDEBUG(fftlog_simp, "FFT complete; " << newData->GetNChannels() << " channel(s) transformed");

        // just sets the event pointer; doesn't actually add the data to the event
        // this way anything receiving the signal can use the event pointer
        newData->SetEvent(tsData->GetEvent());

        fFFTSignal(newData);

        return newData;
    }

    KTFrequencySpectrum* KTSimpleFFT::Transform(const KTTimeSeriesReal* data) const
    {
        UInt_t nTimeBins = (UInt_t)data->size();
        if (nTimeBins != fTimeSize)
        {
            KTWARN(fftlog_simp, "Number of bins in the data provided does not match the number of bins set for this transform\n"
                    << "   Bin expected: " << fTimeSize << ";   Bins in data: " << nTimeBins);
            return NULL;
        }

        Double_t timeBinWidth = data->GetTimeBinWidth();

        copy(data->begin(), data->end(), fInputArray);

        fftw_execute(fFTPlan);

        return ExtractTransformResult(GetMinFrequency(timeBinWidth), GetMaxFrequency(timeBinWidth));
    }

    KTFrequencySpectrum* KTSimpleFFT::ExtractTransformResult(Double_t freqMin, Double_t freqMax) const
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

    void KTSimpleFFT::SetTimeSize(UInt_t nBins)
    {
        fTimeSize = nBins;
        if (fInputArray != NULL) fftw_free(fInputArray);
        if (fOutputArray != NULL) fftw_free(fOutputArray);
        fInputArray = (double*) fftw_malloc(sizeof(double) * fTimeSize);
        fOutputArray = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * CalculateNFrequencyBins(fTimeSize));
        fIsInitialized = false;
        return;
    }

    void KTSimpleFFT::SetTransformFlag(const std::string& flag)
    {
        if (fTransformFlagMap.find(flag) == fTransformFlagMap.end())
        {
            KTWARN(fftlog_simp, "Invalid transform flag requested: " << flag << "\n\tNo change was made.");
            return;
        }
        fTransformFlag = flag;
        fIsInitialized = false;
        return;
    }

    void KTSimpleFFT::ProcessHeader(const KTEggHeader* header)
    {
        SetTimeSize(header->GetRecordSize());
        InitializeFFT();
        return;
    }

    void KTSimpleFFT::ProcessTimeSeriesData(const KTTimeSeriesData* tsData)
    {
        KTFrequencySpectrumData* newData = TransformData(tsData);
        if (tsData->GetEvent() != NULL)
            tsData->GetEvent()->AddData(newData);
        return;
    }

    void KTSimpleFFT::ProcessEvent(KTEvent* event, const string& dataName)
    {
        const KTTimeSeriesData* tsData = NULL;
        if (dataName.empty())
        {
            tsData = dynamic_cast< KTTimeSeriesData* >(event->GetData(KTProgenitorTimeSeriesData::StaticGetName()));
            if (tsData == NULL)
                tsData = dynamic_cast< KTTimeSeriesData* >(event->GetData(KTBasicTimeSeriesData::StaticGetName()));
            if (tsData == NULL)
                tsData = dynamic_cast< KTTimeSeriesData* >(event->GetData(KTTimeSeriesPairedData::StaticGetName()));
        }
        else
        {
            tsData = dynamic_cast< KTTimeSeriesData* >(event->GetData(dataName));
        }

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
