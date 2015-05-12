/*
 * KTComplexFFTW.cc
 *
 *  Created on: Sep 12, 2011
 *      Author: nsoblath
 */

#include "KTComplexFFTW.hh"

#include "KTAnalyticAssociateData.hh"
#include "KTCacheDirectory.hh"
#include "KTEggHeader.hh"
#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTTimeSeriesData.hh"
#include "KTTimeSeriesFFTW.hh"
#include "KTParam.hh"

#include <algorithm>
#include <cmath>

using std::string;
using std::vector;


namespace Katydid
{

    KT_REGISTER_PROCESSOR(KTComplexFFTW, "complex-fftw");

    KTComplexFFTW::KTComplexFFTW(const std::string& name) :
            KTFFTW(),
            KTProcessor(name),
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
            fFFTForwardSignal("fft-forward", this),
            fFFTReverseSignal("fft-reverse", this),
            fHeaderSlot("header", this, &KTComplexFFTW::InitializeWithHeader),
            fTimeSeriesSlot("ts", this, &KTComplexFFTW::TransformData, &fFFTForwardSignal),
            fAASlot("aa", this, &KTComplexFFTW::TransformData, &fFFTForwardSignal),
            fFSFFTWSlot("fs-fftw", this, &KTComplexFFTW::TransformData, &fFFTReverseSignal)
    {
        SetupInternalMaps();
    }

    KTComplexFFTW::~KTComplexFFTW()
    {
        FreeArrays();
        fftw_destroy_plan(fForwardPlan);
        fftw_destroy_plan(fReversePlan);
    }

    bool KTComplexFFTW::Configure(const KTParamNode* node)
    {
        // Config-file settings
        if (node != NULL)
        {
            SetTransformFlag(node->GetValue("transform-flag", fTransformFlag));

            SetUseWisdom(node->GetValue<bool>("use-wisdom", fUseWisdom));
            SetWisdomFilename(node->GetValue("wisdom-filename", fWisdomFilename));
        }

        if (fUseWisdom)
        {
            if (! KTCacheDirectory::GetInstance()->Configure())
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
        unsigned transformFlag = iter->second;

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

        InitializeMultithreaded();

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

    bool KTComplexFFTW::InitializeWithHeader(KTEggHeader& header)
    {
        SetSize(header.GetChannelHeader(0)->GetSliceSize());
        InitializeFFT();
        return true;
    }

    bool KTComplexFFTW::TransformData(KTTimeSeriesData& tsData)
    {
        if (tsData.GetTimeSeries(0)->GetNTimeBins() != GetSize())
        {
            SetSize(tsData.GetTimeSeries(0)->GetNTimeBins());
            InitializeFFT();
        }

        if (! fIsInitialized)
        {
            KTERROR(fftlog_comp, "FFT must be initialized before the transform is performed\n"
                    << "   Please first call InitializeFFT(), then perform the transform.");
            return false;
        }

        unsigned nComponents = tsData.GetNComponents();

        KTFrequencySpectrumDataFFTW& newData = tsData.Of< KTFrequencySpectrumDataFFTW >().SetNComponents(nComponents);

        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            const KTTimeSeriesFFTW* nextInput = dynamic_cast< const KTTimeSeriesFFTW* >(tsData.GetTimeSeries(iComponent));
            if (nextInput == NULL)
            {
                KTERROR(fftlog_comp, "Incorrect time series type: time series did not cast to KTTimeSeriesFFTW.");
                return false;
            }

            KTFrequencySpectrumFFTW* nextResult = Transform(nextInput);

            if (nextResult == NULL)
            {
                KTERROR(fftlog_comp, "Channel <" << iComponent << "> did not transform correctly.");
                return false;
            }
            KTDEBUG(fftlog_comp, "FFT computed; size: " << nextResult->size() << "; range: " << nextResult->GetRangeMin() << " - " << nextResult->GetRangeMax());
            newData.SetSpectrum(nextResult, iComponent);
        }

        KTINFO(fftlog_comp, "FFT complete; " << nComponents << " channel(s) transformed");

        return true;
    }

    bool KTComplexFFTW::TransformData(KTAnalyticAssociateData& tsData)
    {
        if (tsData.GetTimeSeries(0)->GetNTimeBins() != GetSize())
        {
            SetSize(tsData.GetTimeSeries(0)->GetNTimeBins());
            InitializeFFT();
        }

        if (! fIsInitialized)
        {
            KTERROR(fftlog_comp, "FFT must be initialized before the transform is performed\n"
                    << "   Please first call InitializeFFT(), then perform the transform.");
            return false;
        }

        unsigned nComponents = tsData.GetNComponents();

        KTFrequencySpectrumDataFFTW& newData = tsData.Of< KTFrequencySpectrumDataFFTW >().SetNComponents(nComponents);

        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            const KTTimeSeriesFFTW* nextInput = dynamic_cast< const KTTimeSeriesFFTW* >(tsData.GetTimeSeries(iComponent));
            if (nextInput == NULL)
            {
                KTERROR(fftlog_comp, "Incorrect time series type: time series did not cast to KTTimeSeriesFFTW.");
                return false;
            }

            KTFrequencySpectrumFFTW* nextResult = Transform(nextInput);

            if (nextResult == NULL)
            {
                KTERROR(fftlog_comp, "Channel <" << iComponent << "> did not transform correctly.");
                return false;
            }
            KTDEBUG(fftlog_comp, "FFT computed; size: " << nextResult->size() << "; range: " << nextResult->GetRangeMin() << " - " << nextResult->GetRangeMax());
            newData.SetSpectrum(nextResult, iComponent);
        }

        KTINFO(fftlog_comp, "FFT complete; " << nComponents << " channel(s) transformed");

        return true;
    }

    bool KTComplexFFTW::TransformData(KTFrequencySpectrumDataFFTW& fsData)
    {
        if (fsData.GetSpectrumFFTW(0)->size() != GetSize())
        {
            SetSize(fsData.GetSpectrumFFTW(0)->size());
            InitializeFFT();
        }

        if (! fIsInitialized)
        {
            KTERROR(fftlog_comp, "FFT must be initialized before the transform is performed\n"
                    << "   Please first call InitializeFFT(), then perform the transform.");
            return false;
        }

        unsigned nComponents = fsData.GetNComponents();

        KTTimeSeriesData& newData = fsData.Of< KTTimeSeriesData >().SetNComponents(nComponents);

        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            const KTFrequencySpectrumFFTW* nextInput = fsData.GetSpectrumFFTW(iComponent);
            if (nextInput == NULL)
            {
                KTERROR(fftlog_comp, "Frequency spectrum <" << iComponent << "> does not appear to be present.");
                return false;
            }

            KTTimeSeriesFFTW* nextResult = Transform(nextInput);

            if (nextResult == NULL)
            {
                KTERROR(fftlog_comp, "One of the channels did not transform correctly.");
                return false;
            }
            newData.SetTimeSeries(nextResult, iComponent);
        }

        KTDEBUG(fftlog_comp, "FFT complete; " << nComponents << " component(s) transformed");

        return true;
    }

    KTFrequencySpectrumFFTW* KTComplexFFTW::Transform(const KTTimeSeriesFFTW* ts) const
    {
        unsigned nBins = ts->size();
        if (nBins != fSize)
        {
            KTWARN(fftlog_comp, "Number of bins in the data provided does not match the number of bins set for this transform\n"
                    << "   Bin expected: " << fSize << ";   Bins in data: " << nBins);
            return NULL;
        }

        double timeBinWidth = ts->GetTimeBinWidth();
        double freqMin = GetMinFrequency(timeBinWidth);
        double freqMax = GetMaxFrequency(timeBinWidth);

        KTFrequencySpectrumFFTW* newFS = new KTFrequencySpectrumFFTW(nBins, freqMin, freqMax);

        DoTransform(ts, newFS);
        //fftw_execute_dft(fForwardPlan, ts->GetData(), newSpectrum->GetData());

        newFS->SetNTimeBins(nBins);

        return newFS;
    }

    void KTComplexFFTW::DoTransform(const KTTimeSeriesFFTW* tsIn, KTFrequencySpectrumFFTW* fsOut) const
    {
        fftw_execute_dft(fForwardPlan, tsIn->GetData(), fsOut->GetData());
        (*fsOut) *= sqrt(1. / fSize);
        return;
    }

    KTTimeSeriesFFTW* KTComplexFFTW::Transform(const KTFrequencySpectrumFFTW* fs) const
    {
        unsigned nBins = fs->size();
        if (nBins != fSize)
        {
            KTWARN(fftlog_comp, "Number of bins in the data provided does not match the number of bins set for this transform\n"
                    << "   Bin expected: " << fSize << ";   Bins in data: " << nBins);
            return NULL;
        }

        KTTimeSeriesFFTW* newTS = new KTTimeSeriesFFTW(nBins, GetMinTime(), GetMaxTime(fs->GetBinWidth()));

        DoTransform(fs, newTS);
        //fftw_execute_dft(fReversePlan, fs->GetData(), newRecord->GetData());


        return newTS;
    }

    void KTComplexFFTW::DoTransform(const KTFrequencySpectrumFFTW* fsIn, KTTimeSeriesFFTW* tsOut) const
    {
        fftw_execute_dft(fReversePlan, fsIn->GetData(), tsOut->GetData());
        (*tsOut) *= sqrt(1. / double(fSize));

        return;
    }

    void KTComplexFFTW::SetSize(unsigned nBins)
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
