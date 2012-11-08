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
#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTTimeSeriesChannelData.hh"
#include "KTTimeSeriesPairedData.hh"
#include "KTTimeSeriesFFTW.hh"
#include "KTPStoreNode.hh"

#include <algorithm>
#include <cmath>

using std::string;
using std::vector;

namespace Katydid
{

    static KTDerivedRegistrar< KTProcessor, KTComplexFFTW > sSimpleFFTRegistrar("complex-fft");

    KTComplexFFTW::KTComplexFFTW() :
            KTFFT(),
            KTProcessor(),
            fForwardPlan(),
            fReversePlan(),
            fSize(0),
            fInputArray(NULL),
            fOutputArray(NULL),
            fTransformFlag("MEASURE"),
            fTransformFlagMap(),
            fIsInitialized(false),
            fFFTForwardSignal(),
            fFFTReverseSignal()
    {
        fConfigName = "complex-fftw";

        RegisterSignal("fft-forward", &fFFTForwardSignal, "void (const KTWriteableData*)");
        RegisterSignal("fft-reverse", &fFFTReverseSignal, "void (const KTWriteableData*)");

        RegisterSlot("header", this, &KTComplexFFTW::ProcessHeader, "void (const KTEggHeader*)");
        RegisterSlot("ts-data", this, &KTComplexFFTW::ProcessTimeSeriesData, "void (const KTTimeSeriesData*)");
        RegisterSlot("fs-data", this, &KTComplexFFTW::ProcessFrequencySpectrumData, "void (const KTFrequencySpectrumDataFFTW*)");
        RegisterSlot("event-forward", this, &KTComplexFFTW::ProcessEventForward, "void (KTEvent*)");
        RegisterSlot("event-reverse", this, &KTComplexFFTW::ProcessEventReverse, "void (KTEvent*)");
        RegisterSlot("event-forward-named-data", this, &KTComplexFFTW::ProcessEventForward, "void (KTEvent*, const string&)");
        RegisterSlot("event-reverse-named-data", this, &KTComplexFFTW::ProcessEventReverse, "void (KTEvent*, const string&)");

        SetupInternalMaps();
    }

    KTComplexFFTW::KTComplexFFTW(UInt_t size) :
            KTFFT(),
            KTProcessor(),
            fForwardPlan(),
            fReversePlan(),
            fSize(size),
            fInputArray((fftw_complex*) fftw_malloc(sizeof(double) * size)),
            fOutputArray((fftw_complex*) fftw_malloc(sizeof(fftw_complex) * size)),
            fTransformFlag("MEASURE"),
            fTransformFlagMap(),
            fIsInitialized(false),
            fFFTForwardSignal(),
            fFFTReverseSignal()
    {
        fConfigName = "complex-fftw";

        RegisterSignal("fft-forward", &fFFTForwardSignal, "void (const KTWriteableData*)");
        RegisterSignal("fft-reverse", &fFFTReverseSignal, "void (const KTWriteableData*)");

        RegisterSlot("header", this, &KTComplexFFTW::ProcessHeader, "void (const KTEggHeader*)");
        RegisterSlot("ts-data", this, &KTComplexFFTW::ProcessTimeSeriesData, "void (const KTTimeSeriesData*)");
        RegisterSlot("fs-data", this, &KTComplexFFTW::ProcessFrequencySpectrumData, "void (const KTFrequencySpectrumDataFFTW*)");
        RegisterSlot("event-forward", this, &KTComplexFFTW::ProcessEventForward, "void (KTEvent*)");
        RegisterSlot("event-reverse", this, &KTComplexFFTW::ProcessEventReverse, "void (KTEvent*)");
        RegisterSlot("event-forward-named-data", this, &KTComplexFFTW::ProcessEventForward, "void (KTEvent*, const string&)");
        RegisterSlot("event-reverse-named-data", this, &KTComplexFFTW::ProcessEventReverse, "void (KTEvent*, const string&)");

        SetupInternalMaps();
    }

    KTComplexFFTW::~KTComplexFFTW()
    {
        FreeArrays();
        fftw_destroy_plan(fForwardPlan);
        fftw_destroy_plan(fReversePlan);
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
        UInt_t transformFlag = iter->second;

        // allocate the input and output arrays if they're not there already
        AllocateArrays();

        KTDEBUG(fftlog_comp, "Creating plan: " << fSize << " bins; forward FFT");
        fForwardPlan = fftw_plan_dft_1d(fSize, fInputArray, fOutputArray, FFTW_FORWARD, transformFlag | FFTW_PRESERVE_INPUT);
        KTDEBUG(fftlog_comp, "Creating plan: " << fSize << " bins; backward FFT");
        fReversePlan = fftw_plan_dft_1d(fSize, fInputArray, fOutputArray, FFTW_BACKWARD, transformFlag | FFTW_PRESERVE_INPUT);

        if (fForwardPlan != NULL && fReversePlan != NULL)
        {
            fIsInitialized = true;
            // delete the input and output arrays to save memory, since they're not needed for the transform
            FreeArrays();
            KTDEBUG(fftlog_comp, "FFTW plans created; Initialization complete.");
        }
        else
        {
            fIsInitialized = false;
            if (fForwardPlan == NULL)
            {
                KTERROR(fftlog_comp, "Unable to create the forward FFT plan! FFT is not initialized.");
            }
            if (fReversePlan == NULL)
            {
                KTERROR(fftlog_comp, "Unable to create the reverse FFT plan! FFT is not initialized.");
            }
        }
        return;
    }

    KTFrequencySpectrumDataFFTW* KTComplexFFTW::TransformData(const KTTimeSeriesData* tsData)
    {
        if (tsData->GetNTimeSeries() < 1)
        {
            KTWARN(fftlog_comp, "Data has no channels!");
            return NULL;
        }
        if (tsData->GetTimeSeries(0)->GetNTimeBins() != GetSize())
        {
            SetSize(tsData->GetTimeSeries(0)->GetNTimeBins());
            InitializeFFT();
        }

        if (! fIsInitialized)
        {
            KTERROR(fftlog_comp, "FFT must be initialized before the transform is performed\n"
                    << "   Please first call InitializeFFT(), then perform the transform.");
            return NULL;
        }

        KTFrequencySpectrumDataFFTW* newData = new KTFrequencySpectrumDataFFTW(tsData->GetNTimeSeries());

        for (UInt_t iChannel = 0; iChannel < tsData->GetNTimeSeries(); iChannel++)
        {
            const KTTimeSeriesFFTW* nextInput = dynamic_cast< const KTTimeSeriesFFTW* >(tsData->GetTimeSeries(iChannel));
            if (nextInput == NULL)
            {
                KTERROR(fftlog_comp, "Incorrect time series type: time series did not cast to KTTimeSeriesFFTW.");
                delete newData;
                return NULL;
            }

            KTFrequencySpectrumFFTW* nextResult = Transform(nextInput);

            if (nextResult == NULL)
            {
                KTERROR(fftlog_comp, "Channel <" << iChannel << "> did not transform correctly.");
                delete newData;
                return NULL;
            }
            newData->SetSpectrum(nextResult, iChannel);
        }

        KTDEBUG(fftlog_comp, "FFT complete; " << newData->GetNChannels() << " channel(s) transformed");

        // just sets the event pointer; doesn't actually add the data to the event
        // this way anything receiving the signal can use the event pointer
        newData->SetEvent(tsData->GetEvent());

        fFFTForwardSignal(newData);

        return newData;
    }

    KTTimeSeriesData* KTComplexFFTW::TransformData(const KTFrequencySpectrumDataFFTW* fsData)
    {
        if (fsData->GetNChannels() < 1)
        {
            KTWARN(fftlog_comp, "Data has no channels!");
            return NULL;
        }
        if (fsData->GetSpectrum(0)->size() != GetSize())
        {
            SetSize(fsData->GetSpectrum(0)->size());
            InitializeFFT();
        }

        if (! fIsInitialized)
        {
            KTERROR(fftlog_comp, "FFT must be initialized before the transform is performed\n"
                    << "   Please first call InitializeFFT(), then perform the transform.");
            return NULL;
        }

        KTBasicTimeSeriesData* newData = new KTBasicTimeSeriesData(fsData->GetNChannels());

        for (UInt_t iChannel = 0; iChannel < fsData->GetNChannels(); iChannel++)
        {
            const KTFrequencySpectrumFFTW* nextInput = fsData->GetSpectrum(iChannel);
            if (nextInput == NULL)
            {
                KTERROR(fftlog_comp, "Frequency spectrum <" << iChannel << "> does not appear to be present.");
                delete newData;
                return NULL;
            }

            KTTimeSeriesFFTW* nextResult = Transform(nextInput);

            if (nextResult == NULL)
            {
                KTERROR(fftlog_comp, "One of the channels did not transform correctly.");
                delete newData;
                return NULL;
            }
            newData->SetTimeSeries(nextResult, iChannel);
        }

        KTDEBUG(fftlog_comp, "FFT complete; " << newData->GetNTimeSeries() << " channel(s) transformed");

        //newData->SetEvent(fsData->GetEvent());

        fFFTReverseSignal(newData);

        return newData;
    }

    KTFrequencySpectrumFFTW* KTComplexFFTW::Transform(const KTTimeSeriesFFTW* data) const
    {
        UInt_t nBins = data->size();
        if (nBins != fSize)
        {
            KTWARN(fftlog_comp, "Number of bins in the data provided does not match the number of bins set for this transform\n"
                    << "   Bin expected: " << fSize << ";   Bins in data: " << nBins);
            return NULL;
        }

        Double_t timeBinWidth = data->GetTimeBinWidth();
        Double_t freqMin = GetMinFrequency(timeBinWidth);
        Double_t freqMax = GetMaxFrequency(timeBinWidth);

        KTFrequencySpectrumFFTW* newSpectrum = new KTFrequencySpectrumFFTW(nBins, freqMin, freqMax);

        fftw_execute_dft(fForwardPlan, data->GetData(), newSpectrum->GetData());

        (*newSpectrum) *= sqrt(1. / fSize);

        return newSpectrum;
    }

    KTTimeSeriesFFTW* KTComplexFFTW::Transform(const KTFrequencySpectrumFFTW* data) const
    {
        UInt_t nBins = data->size();
        if (nBins != fSize)
        {
            KTWARN(fftlog_comp, "Number of bins in the data provided does not match the number of bins set for this transform\n"
                    << "   Bin expected: " << fSize << ";   Bins in data: " << nBins);
            return NULL;
        }

        KTTimeSeriesFFTW* newRecord = new KTTimeSeriesFFTW(nBins, GetMinTime(), GetMaxTime(data->GetBinWidth()));

        fftw_execute_dft(fReversePlan, data->GetData(), newRecord->GetData());

        (*newRecord) *= sqrt(1. / Double_t(fSize));

        return newRecord;
    }

    void KTComplexFFTW::SetSize(UInt_t nBins)
    {
        fSize = nBins;
        if (fInputArray != NULL)
        {
            fftw_free(fInputArray);
        }
        if (fOutputArray != NULL)
        {
            fftw_free(fOutputArray);
        }
        fInputArray = (fftw_complex*) fftw_malloc(sizeof(double) * fSize);
        fOutputArray = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * fSize);
        fIsInitialized = false;
        return;
    }

    void KTComplexFFTW::SetTransformFlag(const std::string& flag)
    {
        if (fTransformFlagMap.find(flag) == fTransformFlagMap.end())
        {
            KTWARN(fftlog_comp, "Invalid transform flag requested: " << flag << "\n\tNo change was made.");
            return;
        }
        fTransformFlag = flag;
        fIsInitialized = false;
        return;
    }

    void KTComplexFFTW::ProcessHeader(const KTEggHeader* header)
    {
        SetSize(header->GetRecordSize());
        InitializeFFT();
        return;
    }

    void KTComplexFFTW::ProcessTimeSeriesData(const KTTimeSeriesData* tsData)
    {
        KTFrequencySpectrumDataFFTW* newData = TransformData(tsData);
        if (tsData->GetEvent() != NULL)
            tsData->GetEvent()->AddData(newData);
        return;
    }

    void KTComplexFFTW::ProcessFrequencySpectrumData(const KTFrequencySpectrumDataFFTW* fsData)
    {
        KTTimeSeriesData* newData = TransformData(fsData);
        if (fsData->GetEvent() != NULL)
            fsData->GetEvent()->AddData(newData);
        return;
    }

    void KTComplexFFTW::ProcessEventForward(KTEvent* event, const string& dataName)
    {
        KTDEBUG(fftlog_comp, "Performing forward FFT of event " << event->GetEventNumber());
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
            KTWARN(fftlog_comp, "No time series data was available in the event");
            return;
        }

        KTFrequencySpectrumDataFFTW* newData = TransformData(tsData);
        event->AddData(newData);
        return;
    }

    void KTComplexFFTW::ProcessEventReverse(KTEvent* event, const string& dataName)
    {
        KTDEBUG(fftlog_comp, "Performing reverse FFT of event " << event->GetEventNumber());
        const KTFrequencySpectrumDataFFTW* fsData = NULL;
        if (dataName.empty())
        {
            fsData = dynamic_cast< KTFrequencySpectrumDataFFTW* >(event->GetData(KTFrequencySpectrumDataFFTW::StaticGetName()));
        }
        else
        {
            fsData = dynamic_cast< KTFrequencySpectrumDataFFTW* >(event->GetData(dataName));
        }

        if (fsData == NULL)
        {
            KTWARN(fftlog_comp, "No frequency spectrum data was available in the event");
            return;
        }

        KTTimeSeriesData* newData = TransformData(fsData);
        event->AddData(newData);
        return;
    }

    void KTComplexFFTW::SetupInternalMaps()
    {
        // transform flag map
        fTransformFlagMap.clear();
        fTransformFlagMap["ESTIMATE"] = FFTW_ESTIMATE;
        fTransformFlagMap["MEASURE"] = FFTW_MEASURE;
        fTransformFlagMap["PATIENT"] = FFTW_PATIENT;
        fTransformFlagMap["EXHAUSTIVE"] = FFTW_EXHAUSTIVE;
        return;
    }

    void KTComplexFFTW::AllocateArrays()
    {
        FreeArrays();
        if (fInputArray == NULL)
        {
            fInputArray = (fftw_complex*) fftw_malloc(sizeof(double) * fSize);
        }
        if (fOutputArray == NULL)
        {
            fOutputArray = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * fSize);
        }
        return;
    }

    void KTComplexFFTW::FreeArrays()
    {
        if (fInputArray != NULL)
        {
            fftw_free(fInputArray);
            fInputArray = NULL;
        }
        if (fOutputArray != NULL)
        {
            fftw_free(fOutputArray);
            fOutputArray = NULL;
        }
        return;
    }


} /* namespace Katydid */
