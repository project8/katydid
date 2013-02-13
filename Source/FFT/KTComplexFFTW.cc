/*
 * KTComplexFFTW.cc
 *
 *  Created on: Sep 12, 2011
 *      Author: nsoblath
 */

#include "KTComplexFFTW.hh"

#include "KTCacheDirectory.hh"
#include "KTEggHeader.hh"
#include "KTBundle.hh"
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
using boost::shared_ptr;

namespace Katydid
{

    static KTDerivedRegistrar< KTProcessor, KTComplexFFTW > sSimpleFFTRegistrar("complex-fftw");

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
            fUseWisdom(true),
            fWisdomFilename("wisdom_complexfft.fftw3"),
            fForwardInputDataName("time-series"),
            fForwardOutputDataName("frequency-spectrum"),
            fReverseInputDataName("frequency-spectrum"),
            fReverseOutputDataName("time-series"),
            fFFTForwardSignal(),
            fFFTReverseSignal()
    {
        fConfigName = "complex-fftw";

        RegisterSignal("fft-forward", &fFFTForwardSignal, "void (const KTFrequencySpectrumDataFFTW*)");
        RegisterSignal("fft-reverse", &fFFTReverseSignal, "void (const KTTimeSeriesData*)");

        RegisterSlot("header", this, &KTComplexFFTW::ProcessHeader, "void (const KTEggHeader*)");
        RegisterSlot("ts-data", this, &KTComplexFFTW::ProcessTimeSeriesData, "void (const KTTimeSeriesData*)");
        RegisterSlot("fs-data", this, &KTComplexFFTW::ProcessFrequencySpectrumData, "void (const KTFrequencySpectrumDataFFTW*)");
        RegisterSlot("bundle-forward", this, &KTComplexFFTW::ProcessBundleForward, "void (KTBundle*)");
        RegisterSlot("bundle-reverse", this, &KTComplexFFTW::ProcessBundleReverse, "void (KTBundle*)");

        SetupInternalMaps();
    }

    KTComplexFFTW::~KTComplexFFTW()
    {
        FreeArrays();
        fftw_destroy_plan(fForwardPlan);
        fftw_destroy_plan(fReversePlan);
        fftw_cleanup_threads();
    }

    Bool_t KTComplexFFTW::Configure(const KTPStoreNode* node)
    {
        // Config-file settings
        if (node != NULL)
        {
            SetTransformFlag(node->GetData<string>("transform-flag", fTransformFlag));

            SetUseWisdom(node->GetData<Bool_t>("use-wisdom", fUseWisdom));
            SetWisdomFilename(node->GetData<string>("wisdom-filename", fWisdomFilename));

            SetForwardInputDataName(node->GetData<string>("forward-input-data-name", fForwardInputDataName));
            SetForwardOutputDataName(node->GetData<string>("forward-output-data-name", fForwardOutputDataName));
            SetReverseInputDataName(node->GetData<string>("reverse-input-data-name", fReverseInputDataName));
            SetReverseOutputDataName(node->GetData<string>("reverse-output-data-name", fReverseOutputDataName));
        }

        if (fUseWisdom)
        {
            if (! KTCacheDirectory::GetInstance()->PrepareForUse())
            {
                KTWARN(fftlog_comp, "Unable to use wisdom because cache directory is not ready.");
                fUseWisdom = false;
            }
        }

        // Command-line settings
        //SetTransformFlag(fCLHandler->GetCommandLineValue< string >("transform-flag", fTransformFlag));

        return true;
    }

    void KTComplexFFTW::InitializeFFT()
    {
        // fTransformFlag is guaranteed to be valid in the Set method.
        KTDEBUG(fftlog_comp, "Transform flag: " << fTransformFlag);
        TransformFlagMap::const_iterator iter = fTransformFlagMap.find(fTransformFlag);
        UInt_t transformFlag = iter->second;

        // allocate the input and output arrays if they're not there already
        AllocateArrays();

        if (fUseWisdom)
        {
            KTDEBUG(fftlog_comp, "Reading wisdom from file <" << fWisdomFilename << ">");
            if (fftw_import_wisdom_from_filename(fWisdomFilename.c_str()) == 0)
            {
                KTWARN(fftlog_comp, "Unable to read FFTW wisdom from file <" << fWisdomFilename << ">");
            }
        }

#ifdef FFTW_NTHREADS
        fftw_init_threads();
        fftw_plan_with_nthreads(FFTW_NTHREADS);
        KTDEBUG(fftlog_comp, "Configuring FFTW to use up to " << FFTW_NTHREADS << " threads.");
#endif

        KTDEBUG(fftlog_comp, "Creating plan: " << fSize << " bins; forward FFT");
        fForwardPlan = fftw_plan_dft_1d(fSize, fInputArray, fOutputArray, FFTW_FORWARD, transformFlag | FFTW_PRESERVE_INPUT);
        KTDEBUG(fftlog_comp, "Creating plan: " << fSize << " bins; backward FFT");
        fReversePlan = fftw_plan_dft_1d(fSize, fInputArray, fOutputArray, FFTW_BACKWARD, transformFlag | FFTW_PRESERVE_INPUT);

        if (fForwardPlan != NULL && fReversePlan != NULL)
        {
            fIsInitialized = true;
            // delete the input and output arrays to save memory, since they're not needed for the transform
            FreeArrays();
            if (fUseWisdom)
            {
                if (fftw_export_wisdom_to_filename(fWisdomFilename.c_str()) == 0)
                {
                    KTWARN(fftlog_comp, "Unable to write FFTW wisdom to file <" << fWisdomFilename << ">");
                }
            }
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

        newData->SetTimeInRun(tsData->GetTimeInRun());
        newData->SetTimeLength(Double_t(tsData->GetTimeSeries(0)->GetNTimeBins()) * tsData->GetTimeSeries(0)->GetTimeBinWidth());
        newData->SetSliceNumber(tsData->GetSliceNumber());

        KTDEBUG(fftlog_comp, "FFT complete; " << newData->GetNComponents() << " channel(s) transformed");

        newData->SetName(fForwardOutputDataName);

        return newData;
    }

    KTTimeSeriesData* KTComplexFFTW::TransformData(const KTFrequencySpectrumDataFFTW* fsData)
    {
        if (fsData->GetNComponents() < 1)
        {
            KTWARN(fftlog_comp, "Data has no channels!");
            return NULL;
        }
        if (fsData->GetSpectrumFFTW(0)->size() != GetSize())
        {
            SetSize(fsData->GetSpectrumFFTW(0)->size());
            InitializeFFT();
        }

        if (! fIsInitialized)
        {
            KTERROR(fftlog_comp, "FFT must be initialized before the transform is performed\n"
                    << "   Please first call InitializeFFT(), then perform the transform.");
            return NULL;
        }

        KTBasicTimeSeriesData* newData = new KTBasicTimeSeriesData(fsData->GetNComponents());

        for (UInt_t iChannel = 0; iChannel < fsData->GetNComponents(); iChannel++)
        {
            const KTFrequencySpectrumFFTW* nextInput = fsData->GetSpectrumFFTW(iChannel);
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

        newData->SetTimeInRun(fsData->GetTimeInRun());
        newData->SetSliceNumber(fsData->GetSliceNumber());

        KTDEBUG(fftlog_comp, "FFT complete; " << newData->GetNTimeSeries() << " channel(s) transformed");

        newData->SetName(fReverseOutputDataName);

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
        SetSize(header->GetSliceSize());
        InitializeFFT();
        return;
    }

    void KTComplexFFTW::ProcessTimeSeriesData(const KTTimeSeriesData* tsData)
    {
        KTFrequencySpectrumDataFFTW* newData = TransformData(tsData);
        if (newData != NULL)
        {
            KTBundle* bundle = tsData->GetBundle();
            newData->SetBundle(bundle);
            if (bundle != NULL)
                bundle->AddData(newData);
            fFFTForwardSignal(newData);
        }
        return;
    }

    void KTComplexFFTW::ProcessFrequencySpectrumData(const KTFrequencySpectrumDataFFTW* fsData)
    {
        KTTimeSeriesData* newData = TransformData(fsData);
        if (newData != NULL)
        {
            KTBundle* bundle = fsData->GetBundle();
            newData->SetBundle(bundle);
            if (bundle != NULL)
                bundle->AddData(newData);
            fFFTReverseSignal(newData);
        }
        return;
    }

    void KTComplexFFTW::ProcessBundleForward(shared_ptr<KTBundle> bundle)
    {
        KTDEBUG(fftlog_comp, "Performing forward FFT of bundle " << bundle->GetBundleNumber());
        const KTTimeSeriesData* tsData = bundle->GetData< KTTimeSeriesData >(fForwardInputDataName);
        if (tsData == NULL)
        {
            KTWARN(fftlog_comp, "No time series data named <" << fForwardInputDataName << "> was available in the bundle");
            return;
        }

        ProcessTimeSeriesData(tsData);
        return;
    }

    void KTComplexFFTW::ProcessBundleReverse(shared_ptr<KTBundle> bundle)
    {
        KTDEBUG(fftlog_comp, "Performing reverse FFT of bundle " << bundle->GetBundleNumber());
        const KTFrequencySpectrumDataFFTW* fsData = bundle->GetData< KTFrequencySpectrumDataFFTW >(fReverseInputDataName);
        if (fsData == NULL)
        {
            KTWARN(fftlog_comp, "No frequency spectrum data named <" << fReverseInputDataName << "> was available in the bundle");
            return;
        }

        ProcessFrequencySpectrumData(fsData);
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
            fInputArray = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * fSize);
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
