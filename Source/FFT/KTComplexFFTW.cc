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
#include "KTTimeSeriesDataFFTW.hh"
#include "KTPStoreNode.hh"

#include <algorithm>
#include <cmath>

using std::copy;
using std::string;
using std::vector;

namespace Katydid
{

    static KTDerivedRegistrar< KTProcessor, KTComplexFFTW > sSimpleFFTRegistrar("complex-fft");

    KTComplexFFTW::KTComplexFFTW() :
            KTFFT(),
            KTProcessor(),
            fFTPlan(),
            fActivePlanIndex(0),
            fTimeSize(0),
            fInputArray(NULL),
            fOutputArray(NULL),
            fDirection("FORWARD"),
            fDirectionMap(),
            fTransformFlag("MEASURE"),
            fTransformFlagMap(),
            fIsInitialized(false),
            fFFTSignal()
    {
        fConfigName = "complex-fftw";

        RegisterSignal("fft", &fFFTSignal, "void (const KTWriteableData*)");

        RegisterSlot("header", this, &KTComplexFFTW::ProcessHeader, "void (const KTEggHeader*)");
        RegisterSlot("ts-data", this, &KTComplexFFTW::ProcessTimeSeriesData, "void (const KTTimeSeriesDataFFTW*)");
        RegisterSlot("event", this, &KTComplexFFTW::ProcessEvent, "void (KTEvent*)");

        SetupInternalMaps();
    }

    KTComplexFFTW::KTComplexFFTW(UInt_t timeSize) :
            KTFFT(),
            KTProcessor(),
            fFTPlan(),
            fActivePlanIndex(0),
            fTimeSize(timeSize),
            fInputArray((fftw_complex*) fftw_malloc(sizeof(double) * timeSize)),
            fOutputArray((fftw_complex*) fftw_malloc(sizeof(fftw_complex) * fTimeSize)),
            fDirection("FORWARD"),
            fDirectionMap(),
            fTransformFlag("MEASURE"),
            fTransformFlagMap(),
            fIsInitialized(false),
            fFFTSignal()
    {
        fConfigName = "complex-fftw";

        RegisterSignal("fft", &fFFTSignal, "void (const KTWriteableData*)");

        RegisterSlot("header", this, &KTComplexFFTW::ProcessHeader, "void (const KTEggHeader*)");
        RegisterSlot("ts-data", this, &KTComplexFFTW::ProcessTimeSeriesData, "void (const KTTimeSeriesDataFFTW*)");
        RegisterSlot("event", this, &KTComplexFFTW::ProcessEvent, "void (KTEvent*)");

        SetupInternalMaps();
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

            SetDirection(node->GetData<string>("direction", fDirection));
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

        fFTPlan[0] = fftw_plan_dft_1d(fTimeSize, fInputArray, fOutputArray, FFTW_FORWARD, transformFlag | FFTW_PRESERVE_INPUT);
        fFTPlan[1] = fftw_plan_dft_1d(fTimeSize, fInputArray, fOutputArray, FFTW_BACKWARD, transformFlag | FFTW_PRESERVE_INPUT);

        if (fFTPlan != NULL)
        {
            fIsInitialized = true;
            // delete the input and output arrays to save memory, since they're not needed for the transform
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
        }
        else
        {
            fIsInitialized = false;
            KTERROR(fftlog_comp, "Unable to create FFT plan! FFT is not initialized.");
        }
        return;
    }

    KTFrequencySpectrumDataFFTW* KTComplexFFTW::TransformData(const KTTimeSeriesDataFFTW* tsData)
    {
        if (tsData->GetRecordSize() != GetTimeSize())
        {
            SetTimeSize(tsData->GetRecordSize());
            InitializeFFT();
        }

        if (! fIsInitialized)
        {
            KTERROR(fftlog_comp, "FFT must be initialized before the transform is performed\n"
                    << "   Please first call InitializeFFT(), then use a TakeData method to set the data, and then finally perform the transform.");
            return NULL;
        }

        KTFrequencySpectrumDataFFTW* newData = new KTFrequencySpectrumDataFFTW(tsData->GetNChannels());

        for (UInt_t iChannel = 0; iChannel < tsData->GetNChannels(); iChannel++)
        {
            KTFrequencySpectrumFFTW* nextResult = Transform(tsData->GetRecord(iChannel));
            if (nextResult == NULL)
            {
                KTERROR(fftlog_comp, "One of the channels did not transform correctly.");
                delete newData;
                return NULL;
            }
            newData->SetSpectrum(nextResult, iChannel);
        }

        KTDEBUG(fftlog_comp, "FFT complete; " << newData->GetNChannels() << " channel(s) transformed");

        newData->SetEvent(tsData->GetEvent());

        fFFTSignal(newData);

        return newData;
    }

    KTFrequencySpectrumFFTW* KTComplexFFTW::Transform(const KTTimeSeriesFFTW* data) const
    {
        UInt_t nTimeBins = (UInt_t)data->GetNBins();
        if (nTimeBins != fTimeSize)
        {
            KTWARN(fftlog_comp, "Number of bins in the data provided does not match the number of bins set for this transform\n"
                    << "   Bin expected: " << fTimeSize << ";   Bins in data: " << data->GetNBins());
            return NULL;
        }

        Double_t freqBinWidth = 1. / (data->GetBinWidth() * (Double_t)nTimeBins);
        Double_t freqMin = -0.5 * freqBinWidth;
        Double_t freqMax = freqBinWidth * ((Double_t)GetFrequencySize() - 0.5);

        KTFrequencySpectrumFFTW* newSpectrum = new KTFrequencySpectrumFFTW(CalculateNFrequencyBins(fTimeSize), fTimeSize, freqMin, freqMax);

        fftw_execute_dft(fFTPlan[fActivePlanIndex], data->GetData(), newSpectrum->GetData());

        return newSpectrum;
    }

    void KTComplexFFTW::SetTimeSize(UInt_t nBins)
    {
        fTimeSize = nBins;
        if (fInputArray != NULL)
        {
            fftw_free(fInputArray);
        }
        if (fOutputArray != NULL)
        {
            fftw_free(fOutputArray);
        }
        fInputArray = (fftw_complex*) fftw_malloc(sizeof(double) * fTimeSize);
        fOutputArray = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * fTimeSize);
        fIsInitialized = false;
        return;
    }

    void KTComplexFFTW::SetDirection(const std::string& dir)
    {
        DirectionMap::const_iterator cit = fDirectionMap.find(dir);
        if (cit == fDirectionMap.end())
        {
            KTWARN(fftlog_comp, "Invalid direction requested: " << dir << "\n\tNo change was made.");
            return;
        }
        fDirection = dir;
        fActivePlanIndex = cit->second;
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
        SetTimeSize(header->GetRecordSize());
        InitializeFFT();
        return;
    }

    void KTComplexFFTW::ProcessTimeSeriesData(const KTTimeSeriesDataFFTW* tsData)
    {
        KTFrequencySpectrumDataFFTW* newData = TransformData(tsData);
        tsData->GetEvent()->AddData(newData);
        return;
    }

    void KTComplexFFTW::ProcessEvent(KTEvent* event)
    {
        KTDEBUG(fftlog_comp, "Performing FFT of event " << event->GetEventNumber());
        const KTTimeSeriesDataFFTW* tsData = dynamic_cast< KTTimeSeriesDataFFTW* >(event->GetData(KTTimeSeriesDataFFTW::StaticGetName()));
        if (tsData == NULL)
        {
            KTWARN(fftlog_comp, "No time series data was available in the event");
            return;
        }
        KTFrequencySpectrumDataFFTW* newData = TransformData(tsData);
        event->AddData(newData);
        return;
    }

    void KTComplexFFTW::SetupInternalMaps()
    {
        // direction map
        fDirectionMap.clear();
        fDirectionMap["FORWARD"] = 0;
        fDirectionMap["BACKWARD"] = 1;

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
        if (fInputArray == NULL)
        {
            fInputArray = (fftw_complex*) fftw_malloc(sizeof(double) * fTimeSize);
        }
        if (fOutputArray == NULL)
        {
            fOutputArray = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * fTimeSize);
        }
        return;
    }


} /* namespace Katydid */
