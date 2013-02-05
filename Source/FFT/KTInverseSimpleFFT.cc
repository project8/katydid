/*
 * KTInverseSimpleFFT.cc
 *
 *  Created on: Sep 12, 2011
 *      Author: nsoblath
 */

#include "KTInverseSimpleFFT.hh"

#include "KTEggHeader.hh"
#include "KTBundle.hh"
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

    KTInverseSimpleFFT::KTInverseSimpleFFT() :
            //KTFFT(),
            KTProcessor(),
            KTConfigurable(),
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

        RegisterSlot("header", this, &KTInverseSimpleFFT::ProcessHeader, "void (const KTEggHeader*)");
        RegisterSlot("ts-data", this, &KTInverseSimpleFFT::ProcessTimeSeriesData, "void (const KTTimeSeriesData*)");
        RegisterSlot("bundle", this, &KTInverseSimpleFFT::ProcessEvent, "void (KTBundle*)");

        SetupTransformFlagMap();
    }

    KTInverseSimpleFFT::KTInverseSimpleFFT(UInt_t timeSize) :
            //KTFFT(),
            KTProcessor(),
            KTConfigurable(),
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

        RegisterSlot("header", this, &KTInverseSimpleFFT::ProcessHeader, "void (const KTEggHeader*)");
        RegisterSlot("ts-data", this, &KTInverseSimpleFFT::ProcessTimeSeriesData, "void (const KTTimeSeriesData*)");
        RegisterSlot("bundle", this, &KTInverseSimpleFFT::ProcessEvent, "void (KTBundle*)");

        SetupTransformFlagMap();
    }

    KTInverseSimpleFFT::~KTInverseSimpleFFT()
    {
        if (fInputArray != NULL) fftw_free(fInputArray);
        if (fOutputArray != NULL) fftw_free(fOutputArray);
    }

    Bool_t KTInverseSimpleFFT::Configure(const KTPStoreNode* node)
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

    void KTInverseSimpleFFT::InitializeFFT()
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

    KTFrequencySpectrumData* KTInverseSimpleFFT::TransformData(const KTTimeSeriesData* tsData)
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

        KTFrequencySpectrumData* newData = new KTFrequencySpectrumData(tsData->GetNTimeSeries());

        for (UInt_t iChannel = 0; iChannel < tsData->GetNTimeSeries(); iChannel++)
        {
            KTFrequencySpectrum* nextResult = Transform(tsData->GetTimeSeries(iChannel));
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

    KTFrequencySpectrum* KTInverseSimpleFFT::Transform(const KTTimeSeries* data) const
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

    KTFrequencySpectrum* KTInverseSimpleFFT::ExtractTransformResult(Double_t freqMin, Double_t freqMax) const
    {
        UInt_t freqSize = GetFrequencySize();
        Double_t normalization = sqrt(2. / (Double_t)GetTimeSize());

        Double_t tempReal, tempImag;
        KTFrequencySpectrum* newSpect = new KTFrequencySpectrum(freqSize, freqMin, freqMax);
        for (Int_t iPoint = 0; iPoint<freqSize; iPoint++)
        {
            (*newSpect)[iPoint].set_rect(fOutputArray[iPoint][0], fOutputArray[iPoint][1]);
            (*newSpect)[iPoint] *= normalization;
        }

        return newSpect;
    }

    void KTInverseSimpleFFT::ProcessHeader(const KTEggHeader* header)
    {
        SetTimeSize(header->GetRecordSize());
        InitializeFFT();
        return;
    }

    void KTInverseSimpleFFT::ProcessTimeSeriesData(const KTTimeSeriesData* tsData)
    {
        KTFrequencySpectrumData* newData = TransformData(tsData);
        tsData->GetEvent()->AddData(newData);
        return;
    }

    void KTInverseSimpleFFT::ProcessEvent(KTBundle* bundle)
    {
        KTDEBUG(fftlog_simp, "Performing FFT of bundle " << bundle->GetEventNumber());
        const KTTimeSeriesData* tsData = bundle->GetData< KTTimeSeriesData >(KTTimeSeriesData::StaticGetName());
        if (tsData == NULL)
        {
            KTWARN(fftlog_simp, "No time series data was available in the bundle");
            return;
        }
        KTFrequencySpectrumData* newData = TransformData(tsData);
        bundle->AddData(newData);
        return;
    }

    void KTInverseSimpleFFT::SetupTransformFlagMap()
    {
        fTransformFlagMap.clear();
        fTransformFlagMap["ESTIMATE"] = FFTW_ESTIMATE;
        fTransformFlagMap["MEASURE"] = FFTW_MEASURE;
        fTransformFlagMap["PATIENT"] = FFTW_PATIENT;
        fTransformFlagMap["EXHAUSTIVE"] = FFTW_EXHAUSTIVE;
        return;
    }


} /* namespace Katydid */
