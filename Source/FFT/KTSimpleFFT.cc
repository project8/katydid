/*
 * KTSimpleFFT.cc
 *
 *  Created on: Sep 12, 2011
 *      Author: nsoblath
 */

#include "KTSimpleFFT.hh"

#include "KTCacheDirectory.hh"
#include "KTEggHeader.hh"
#include "KTBundle.hh"
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
using boost::shared_ptr;

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
            fUseWisdom(true),
            fWisdomFilename("wisdom_simplefft.fftw3"),
            fInputDataName("time-series"),
            fOutputDataName("frequency-spectrum"),
            fFFTSignal()
    {
        fConfigName = "simple-fft";

        RegisterSignal("fft", &fFFTSignal, "void (const KTFrequencySpectrumData*)");

        RegisterSlot("header", this, &KTSimpleFFT::ProcessHeader, "void (const KTEggHeader*)");
        RegisterSlot("ts-data", this, &KTSimpleFFT::ProcessTimeSeriesData, "void (const KTTimeSeriesData*)");
        RegisterSlot("bundle", this, &KTSimpleFFT::ProcessEvent, "void (KTBundle*)");

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

            SetUseWisdom(node->GetData<Bool_t>("use-wisdom", fUseWisdom));
            SetWisdomFilename(node->GetData<string>("wisdom-filename", fWisdomFilename));

            SetInputDataName(node->GetData<string>("input-data-name", fInputDataName));
            SetOutputDataName(node->GetData<string>("output-data-name", fOutputDataName));

        }

        if (fUseWisdom)
        {
            if (! KTCacheDirectory::GetInstance()->PrepareForUse())
            {
                KTWARN(fftlog_simp, "Unable to use wisdom because cache directory is not ready.");
                fUseWisdom = false;
            }
        }

        // Command-line settings
        //SetTransformFlag(fCLHandler->GetCommandLineValue< string >("transform-flag", fTransformFlag));

        return true;
    }

    void KTSimpleFFT::InitializeFFT()
    {
        // fTransformFlag is guaranteed to be valid in the Set method.
        KTDEBUG(fftlog_simp, "Transform flag: " << fTransformFlag);
        TransformFlagMap::const_iterator iter = fTransformFlagMap.find(fTransformFlag);
        Int_t transformFlag = iter->second;

        if (fUseWisdom)
        {
            KTDEBUG(fftlog_simp, "Reading wisdom from file <" << fWisdomFilename << ">");
            if (fftw_import_wisdom_from_filename(fWisdomFilename.c_str()) == 0)
            {
                KTWARN(fftlog_simp, "Unable to read FFTW wisdom from file <" << fWisdomFilename << ">");
            }
        }

        fFTPlan = fftw_plan_dft_r2c_1d(fTimeSize, fInputArray, fOutputArray, transformFlag);
        if (fFTPlan != NULL)
        {
            fIsInitialized = true;
            KTDEBUG(fftlog_simp, "FFT is initialized" << '\n' <<
                    "\tTime-domain size: " << fTimeSize << '\n' <<
                    "\tFrequency-domain size: " << GetFrequencySize());
            if (fUseWisdom)
            {
                if (fftw_export_wisdom_to_filename(fWisdomFilename.c_str()) == 0)
                {
                    KTWARN(fftlog_simp, "Unable to write FFTW wisdom to file <" << fWisdomFilename << ">");
                }
            }
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

        newData->SetTimeInRun(tsData->GetTimeInRun());
        newData->SetSliceNumber(tsData->GetSliceNumber());

        KTDEBUG(fftlog_simp, "FFT complete; " << newData->GetNChannels() << " channel(s) transformed");

        newData->SetName(fOutputDataName);

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
        SetTimeSize(header->GetSliceSize());
        InitializeFFT();
        return;
    }

    void KTSimpleFFT::ProcessTimeSeriesData(const KTTimeSeriesData* tsData)
    {
        KTFrequencySpectrumData* newData = TransformData(tsData);
        if (tsData->GetEvent() != NULL)
        {
            KTBundle* bundle = tsData->GetEvent();
            newData->SetEvent(bundle);
            bundle->AddData(newData);
            fFFTSignal(newData);
        }
        return;
    }

    void KTSimpleFFT::ProcessEvent(shared_ptr<KTBundle> bundle)
    {
        const KTTimeSeriesData* tsData = bundle->GetData< KTTimeSeriesData >(fInputDataName);
        if (tsData == NULL)
        {
            KTWARN(fftlog_simp, "No time series data named <" << fInputDataName << "> was available in the bundle");
            return;
        }

        ProcessTimeSeriesData(tsData);
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
