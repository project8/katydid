/*
 * KTFFTW.cc
 *
 *  Created on: Sep 12, 2011
 *      Author: nsoblath
 */

#include "KTFFTW.hh"

#include "KTAnalyticAssociateData.hh"
#include "KTCacheDirectory.hh"
#include "KTEggHeader.hh"
#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTLogger.hh"
#include "KTTimeSeriesData.hh"
#include "KTTimeSeriesFFTW.hh"
#include "KTParam.hh"

#include <algorithm>
#include <cmath>

using std::string;
using std::vector;


namespace Katydid
{
    KTLOGGER(fftwlog, "KTFFTW");

    KT_REGISTER_PROCESSOR(KTFFTW, "fftw");

    unsigned KTFFTW::sInstanceCount = 0;
    bool KTFFTW::sMultithreadedIsInitialized = false;

    KTFFTW::KTFFTW(const std::string& name) :
            KTFFT(),
            KTProcessor(name),
            fPrepareForwardTransform(true),
            fPrepareReverseTransform(true),
            fUseWisdom(true),
            fWisdomFilename("wisdom_complexfft.fftw3"),
            fTimeSize(0),
            fFrequencySize(0),
            fTransformFlag("ESTIMATE"),
            fTransformFlagMap(),
            fState(kR2C),
            fIsInitialized(false),
            fForwardPlan(),
            fReversePlan(),
            fInputArray(NULL),
            fOutputArray(NULL),
            fFFTForwardSignal("fft-forward", this),
            fFFTReverseSignal("fft-reverse", this),
            fHeaderSlot("header", this, &KTFFTW::InitializeWithHeader),
            fTimeSeriesSlot("ts", this, &KTFFTW::TransformData, &fFFTForwardSignal),
            fAASlot("aa", this, &KTFFTW::TransformData, &fFFTForwardSignal),
            fFSFFTWSlot("fs-fftw", this, &KTFFTW::TransformData, &fFFTReverseSignal)
    {
        sInstanceCount++;

        SetupInternalMaps();
    }

    KTFFTW::~KTFFTW()
    {
        FreeArrays();
        if (fForwardPlan != NULL) fftw_destroy_plan(fForwardPlan);
        if (fReversePlan != NULL) fftw_destroy_plan(fReversePlan);

#ifdef FFTW_NTHREADS
        if (sInstanceCount == 1)
        {
            fftw_cleanup_threads();
        }
#endif

        sInstanceCount--;
    }

    bool KTFFTW::Configure(const KTParamNode* node)
    {
        // Config-file settings
        if (node != NULL)
        {
            SetTransformFlag(node->GetValue("transform-flag", fTransformFlag));

            SetPrepareForwardTransform(node->GetValue("prep-forward-fft", fPrepareForwardTransform));
            SetPrepareReverseTransform(node->GetValue("prep-reverse-fft", fPrepareReverseTransform));

            SetUseWisdom(node->GetValue<bool>("use-wisdom", fUseWisdom));
            SetWisdomFilename(node->GetValue("wisdom-filename", fWisdomFilename));
        }

        if (fUseWisdom)
        {
            if (! KTCacheDirectory::GetInstance()->Configure())
            {
                KTWARN(fftwlog, "Unable to use wisdom because cache directory is not ready.");
                fUseWisdom = false;
            }
        }

        // Command-line settings
        //SetTransformFlag(fCLHandler->GetCommandLineValue< string >("transform-flag", fTransformFlag));

        return true;
    }

    bool KTFFTW::InitializeForRealTDD()
    {
        return InitializeFFT(kR2C);
    }

    bool KTFFTW::InitializeForComplexTDD()
    {
        return InitializeFFT(kC2C);
    }

    bool KTFFTW::InitializeFFT(KTFFTW::State intendedState)
    {
        // fTransformFlag is guaranteed to be valid in the Set method.
        KTDEBUG(fftwlog, "Transform flag: " << fTransformFlag);
        TransformFlagMap::const_iterator iter = fTransformFlagMap.find(fTransformFlag);
        unsigned transformFlag = iter->second;

        // allocate the input and output arrays if they're not there already
        AllocateArrays();

        if (fUseWisdom)
        {
            KTDEBUG(fftwlog, "Reading wisdom from file <" << fWisdomFilename << ">");
            if (fftw_import_wisdom_from_filename(fWisdomFilename.c_str()) == 0)
            {
                KTWARN(fftwlog, "Unable to read FFTW wisdom from file <" << fWisdomFilename << ">");
            }
        }

        // setup fftw threads if using multithreading
#ifdef FFTW_NTHREADS
        if (! sMultithreadedIsInitialized)
        {
            fftw_init_threads();
            fftw_plan_with_nthreads(FFTW_NTHREADS);
            KTDEBUG(fftwlog, "Configuring FFTW to use up to " << FFTW_NTHREADS << " threads.");
            sMultithreadedIsInitialized = true;
        }
#endif

        if (intendedState == kR2C)
        {
            if (fPrepareForwardTransform)
            {
                KTDEBUG(fftlog_simp, "Creating plan: " << fTimeSize << " time bins; forward FFT");
                fForwardPlan = fftw_plan_dft_r2c_1d(fTimeSize, fTSArray, fFSArray, transformFlag);
            }
            if (fPrepareReverseTransform)
            {
                KTDEBUG(fftlog_simp, "Creating plan: " << fFrequencySize << " frequency bins; reverse FFT");
                fReversePlan = fftw_plan_dft_c2r_1d(fTimeSize, fFSArray, fTSArray, transformFlag);
            }

        }
        else // intendedState == kC2C
        {
            if (fPrepareForwardTransform)
            {
                KTDEBUG(fftwlog, "Creating plan: " << fTimeSize << " time bins; forward FFT");
                fForwardPlan = fftw_plan_dft_1d(fSize, fInputArray, fOutputArray, FFTW_FORWARD, transformFlag | FFTW_PRESERVE_INPUT);
            }
            if (fPrepareReverseTransform)
            {
                KTDEBUG(fftwlog, "Creating plan: " << fFrequencySize << " frequency bins; backward FFT");
                fReversePlan = fftw_plan_dft_1d(fSize, fInputArray, fOutputArray, FFTW_BACKWARD, transformFlag | FFTW_PRESERVE_INPUT);
            }


        }

        if (fForwardPlan != NULL && fReversePlan != NULL)
        {
            fIsInitialized = true;
            // delete the input and output arrays to save memory, since they're not needed for the transform
            FreeArrays();
            if (fUseWisdom)
            {
                if (fftw_export_wisdom_to_filename(fWisdomFilename.c_str()) == 0)
                {
                    KTWARN(fftwlog, "Unable to write FFTW wisdom to file <" << fWisdomFilename << ">");
                }
            }
            KTDEBUG(fftwlog, "FFTW plans created; Initialization complete.");
        }
        else
        {
            fIsInitialized = false;
            if (fForwardPlan == NULL)
            {
                KTERROR(fftwlog, "Unable to create the forward FFT plan! FFT is not initialized.");;
            }
            if (fReversePlan == NULL)
            {
                KTERROR(fftwlog, "Unable to create the reverse FFT plan! FFT is not initialized.");
            }
            return false;
        }
        return true;
    }

    bool KTFFTW::InitializeWithHeader(KTEggHeader& header)
    {
        SetTimeSize(header.GetSliceSize());
        if (???)
        {
            return InitializeForRealTDD();
        }
        else
        {
            return InitializeForComplexTDD();
        }
    }

    bool KTFFTW::TransformRealData(KTTimeSeriesData& tsData)
    {
        if (tsData.GetTimeSeries(0)->GetNTimeBins() != GetTimeSize())
        {
            SetSize(tsData.GetTimeSeries(0)->GetNTimeBins());
            InitializeFFT();
        }

        if (! fIsInitialized)
        {
            KTERROR(fftwlog, "FFT must be initialized before the transform is performed\n"
                    << "   Please first call InitializeFFT(), then perform the transform.");
            return false;
        }

        unsigned nComponents = tsData.GetNComponents();

        KTFrequencySpectrumDataFFTW& newData = tsData.Of< KTFrequencySpectrumDataFFTW >().SetNComponents(nComponents);

        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            const KTTimeSeriesReal* nextInput = dynamic_cast< const KTTimeSeriesReal* >(tsData.GetTimeSeries(iComponent));
            if (nextInput == NULL)
            {
                KTERROR(fftwlog, "Incorrect time series type: time series did not cast to KTTimeSeriesReal.");
                return false;
            }

            KTFrequencySpectrumFFTW* nextResult = Transform(nextInput);

            if (nextResult == NULL)
            {
                KTERROR(fftwlog, "Channel <" << iComponent << "> did not transform correctly.");
                return false;
            }
            KTDEBUG(fftwlog, "FFT computed; size: " << nextResult->size() << "; range: " << nextResult->GetRangeMin() << " - " << nextResult->GetRangeMax());
            newData.SetSpectrum(nextResult, iComponent);
        }

        KTINFO(fftwlog, "FFT complete; " << nComponents << " channel(s) transformed");

        return true;
    }

    bool KTFFTW::TransformComplexData(KTTimeSeriesData& tsData)
    {
        if (tsData.GetTimeSeries(0)->GetNTimeBins() != GetTimeSize())
        {
            SetSize(tsData.GetTimeSeries(0)->GetNTimeBins());
            InitializeFFT();
        }

        if (! fIsInitialized)
        {
            KTERROR(fftwlog, "FFT must be initialized before the transform is performed\n"
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
                KTERROR(fftwlog, "Incorrect time series type: time series did not cast to KTTimeSeriesFFTW.");
                return false;
            }

            KTFrequencySpectrumFFTW* nextResult = Transform(nextInput);

            if (nextResult == NULL)
            {
                KTERROR(fftwlog, "Channel <" << iComponent << "> did not transform correctly.");
                return false;
            }
            KTDEBUG(fftwlog, "FFT computed; size: " << nextResult->size() << "; range: " << nextResult->GetRangeMin() << " - " << nextResult->GetRangeMax());
            newData.SetSpectrum(nextResult, iComponent);
        }

        KTINFO(fftwlog, "FFT complete; " << nComponents << " channel(s) transformed");

        return true;
    }

    bool KTFFTW::TransformComplexData(KTAnalyticAssociateData& tsData)
    {
        if (tsData.GetTimeSeries(0)->GetNTimeBins() != GetTimeSize())
        {
            SetSize(tsData.GetTimeSeries(0)->GetNTimeBins());
            InitializeFFT();
        }

        if (! fIsInitialized)
        {
            KTERROR(fftwlog, "FFT must be initialized before the transform is performed\n"
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
                KTERROR(fftwlog, "Incorrect time series type: time series did not cast to KTTimeSeriesFFTW.");
                return false;
            }

            KTFrequencySpectrumFFTW* nextResult = Transform(nextInput);

            if (nextResult == NULL)
            {
                KTERROR(fftwlog, "Channel <" << iComponent << "> did not transform correctly.");
                return false;
            }
            KTDEBUG(fftwlog, "FFT computed; size: " << nextResult->size() << "; range: " << nextResult->GetRangeMin() << " - " << nextResult->GetRangeMax());
            newData.SetSpectrum(nextResult, iComponent);
        }

        KTINFO(fftwlog, "FFT complete; " << nComponents << " channel(s) transformed");

        return true;
    }

    bool KTFFTW::TransformData(KTFrequencySpectrumDataFFTW& fsData)
    {
        if (fsData.GetSpectrumFFTW(0)->size() != GetSize())
        {
            SetSize(fsData.GetSpectrumFFTW(0)->size());
            InitializeFFT();
        }

        if (! fIsInitialized)
        {
            KTERROR(fftwlog, "FFT must be initialized before the transform is performed\n"
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
                KTERROR(fftwlog, "Frequency spectrum <" << iComponent << "> does not appear to be present.");
                return false;
            }

            KTTimeSeriesFFTW* nextResult = Transform(nextInput);

            if (nextResult == NULL)
            {
                KTERROR(fftwlog, "One of the channels did not transform correctly.");
                return false;
            }
            newData.SetTimeSeries(nextResult, iComponent);
        }

        KTDEBUG(fftwlog, "FFT complete; " << nComponents << " component(s) transformed");

        return true;
    }

    KTFrequencySpectrumFFTW* KTFFTW::Transform(const KTTimeSeriesReal* ts) const
    {
        unsigned nBins = ts->size();
        if (nBins != fSize)
        {
            KTWARN(fftwlog, "Number of bins in the data provided does not match the number of bins set for this transform\n"
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

    KTFrequencySpectrumFFTW* KTFFTW::Transform(const KTTimeSeriesFFTW* ts) const
    {
        unsigned nBins = ts->size();
        if (nBins != fSize)
        {
            KTWARN(fftwlog, "Number of bins in the data provided does not match the number of bins set for this transform\n"
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

    void KTFFTW::DoTransform(const KTTimeSeriesReal* tsIn, KTFrequencySpectrumFFTW* fsOut) const
    {
        copy(tsIn->begin(), tsIn->end(), fTSArray);
        fftw_execute_dft(fForwardPlan, fTSArray, fsOut->GetData());
        (*fsOut) *= sqrt(2. / (double)fTimeSize);
        return;
    }

    void KTFFTW::DoTransform(const KTTimeSeriesFFTW* tsIn, KTFrequencySpectrumFFTW* fsOut) const
    {
        fftw_execute_dft(fForwardPlan, tsIn->GetData(), fsOut->GetData());
        (*fsOut) *= sqrt(1. / (double)fTimeSize);
        return;
    }

    KTTimeSeriesFFTW* KTFFTW::Transform(const KTFrequencySpectrumFFTW* fs) const
    {
        unsigned nBins = fs->size();
        if (nBins != fSize)
        {
            KTWARN(fftwlog, "Number of bins in the data provided does not match the number of bins set for this transform\n"
                    << "   Bin expected: " << fSize << ";   Bins in data: " << nBins);
            return NULL;
        }

        KTTimeSeriesFFTW* newTS = new KTTimeSeriesFFTW(nBins, GetMinTime(), GetMaxTime(fs->GetBinWidth()));

        DoTransform(fs, newTS);
        //fftw_execute_dft(fReversePlan, fs->GetData(), newRecord->GetData());


        return newTS;
    }

    void KTFFTW::DoTransform(const KTFrequencySpectrumFFTW* fsIn, KTTimeSeriesFFTW* tsOut) const
    {
        fftw_execute_dft(fReversePlan, fsIn->GetData(), tsOut->GetData());
        (*tsOut) *= sqrt(1. / double(fSize));

        return;
    }

    void KTFFTW::SetTimeSize(unsigned nBins)
    {
        fTimeSize = nBins;
        if (fState == kR2C) fFrequencySize = nBins / 2 + 1;
        else fFrequencySize = nBins;

        FreeArrays();
        //fInputArray = (fftw_complex*) fftw_malloc(sizeof(double) * fSize);
        //fOutputArray = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * fSize);

        fIsInitialized = false;
        return;
    }

    void KTFFTW::SetFrequencySize(unsigned nBins)
    {
        fFrequencySize = nBins;
        if (fState == kR2C) fFrequencySize = (nBins - 1) * 2;
        else fFrequencySize = nBins;

        FreeArrays();
        //fInputArray = (fftw_complex*) fftw_malloc(sizeof(double) * fSize);
        //fOutputArray = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * fSize);

        fIsInitialized = false;
        return;
    }

    void KTFFTW::SetTransformFlag(const std::string& flag)
    {
        if (fTransformFlagMap.find(flag) == fTransformFlagMap.end())
        {
            KTWARN(fftwlog, "Invalid transform flag requested: " << flag << "\n\tNo change was made.");
            return;
        }
        fTransformFlag = flag;
        fIsInitialized = false;
        return;
    }

    void KTFFTW::SetupInternalMaps()
    {
        // transform flag map
        fTransformFlagMap.clear();
        fTransformFlagMap["ESTIMATE"] = FFTW_ESTIMATE;
        fTransformFlagMap["MEASURE"] = FFTW_MEASURE;
        fTransformFlagMap["PATIENT"] = FFTW_PATIENT;
        fTransformFlagMap["EXHAUSTIVE"] = FFTW_EXHAUSTIVE;
        return;
    }

    void KTFFTW::AllocateArrays()
    {
        FreeArrays();
        if (fState == kR2C)
        {
            if (fInputArray == NULL)
            {
                fInputArray = (double*) fftw_malloc(sizeof(double) * fTimeSize);
            }
            if (fOutputArray == NULL)
            {
                fOutputArray = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * fFrequencySize);
            }
        }
        else
        {
            if (fInputArray == NULL)
            {
                fInputArray = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * fTimeSize);
            }
            if (fOutputArray == NULL)
            {
                fOutputArray = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * fFrequencySize);
            }
        }
        return;
    }

    void KTFFTW::FreeArrays()
    {
        if (fTSArray != NULL)
        {
            fftw_free(fTSArray);
            fTSArray = NULL;
        }
        if (fFSArray != NULL)
        {
            fftw_free(fFSArray);
            fFSArray = NULL;
        }
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
