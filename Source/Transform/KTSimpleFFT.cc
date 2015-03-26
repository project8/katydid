/*
 * KTSimpleFFT.cc
 *
 *  Created on: Sep 12, 2011
 *      Author: nsoblath
 */

#include "KTSimpleFFT.hh"

#include "KTCacheDirectory.hh"
#include "KTCorrelationData.hh"
#include "KTCorrelationTSData.hh"
#include "KTEggHeader.hh"
#include "KTFrequencySpectrumDataPolar.hh"
#include "KTTimeSeriesData.hh"
#include "KTTimeSeriesFFTW.hh"
#include "KTTimeSeriesReal.hh"
#include "KTParam.hh"

#include <algorithm>
#include <cmath>

using std::copy;
using std::string;
using std::vector;


namespace Katydid
{

    KT_REGISTER_PROCESSOR(KTSimpleFFT, "simple-fft");

    KTSimpleFFT::KTSimpleFFT(const std::string& name) :
            KTFFT(),
            KTProcessor(name),
            fForwardPlan(),
            fReversePlan(),
            fTimeSize(0),
            fTSArray(NULL),
            fFSArray(NULL),
            fTransformFlag("MEASURE"),
            fIsInitialized(false),
            fUseWisdom(true),
            fWisdomFilename("wisdom_simplefft.fftw3"),
            fFFTForwardSignal("fft-forward", this),
            fFFTReverseSignal("fft-reverse", this),
            fFFTReverseCorrSignal("fft-reverse-corr", this),
            fFFTForwardAgainSignal("fft-forward-again", this),
            fHeaderSlot("header", this, &KTSimpleFFT::InitializeWithHeader),
            fTimeSeriesSlot("ts", this, &KTSimpleFFT::TransformData, &fFFTForwardSignal),
            fFSPolarSlot("fs-polar", this, &KTSimpleFFT::TransformData, &fFFTReverseSignal),
            fCorrSlot("corr", this, &KTSimpleFFT::TransformData, &fFFTReverseCorrSignal),
            fFSPolarForwardSlot("fs-polar-forward", this, &KTSimpleFFT::TransformDataAgain, &fFFTForwardAgainSignal)
    {
        SetupTransformFlagMap();
    }

    KTSimpleFFT::~KTSimpleFFT()
    {
        if (fTSArray != NULL) fftw_free(fTSArray);
        if (fFSArray != NULL) fftw_free(fFSArray);
        fftw_destroy_plan(fForwardPlan);
        fftw_destroy_plan(fReversePlan);
    }

    bool KTSimpleFFT::Configure(const KTParamNode* node)
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
        int transformFlag = iter->second;

        if (fUseWisdom)
        {
            KTDEBUG(fftlog_simp, "Reading wisdom from file <" << fWisdomFilename << ">");
            if (fftw_import_wisdom_from_filename(fWisdomFilename.c_str()) == 0)
            {
                KTWARN(fftlog_simp, "Unable to read FFTW wisdom from file <" << fWisdomFilename << ">");
            }
        }

        // SetTimeSize should have been called already to allocate the TS and FS arrays

        KTDEBUG(fftlog_simp, "Creating plan: " << fTimeSize << " bins; forward FFT");
        fForwardPlan = fftw_plan_dft_r2c_1d(fTimeSize, fTSArray, fFSArray, transformFlag);
        KTDEBUG(fftlog_simp, "Creating plan: " << fTimeSize << " bins; reverse FFT");
        fReversePlan = fftw_plan_dft_c2r_1d(fTimeSize, fFSArray, fTSArray, transformFlag);

        if (fForwardPlan != NULL && fReversePlan != NULL)
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
            KTDEBUG(fftlog_simp, "FFTW plans created; Initialization complete.");
        }
        else
        {
            fIsInitialized = false;
            if (fForwardPlan == NULL)
            {
                KTERROR(fftlog_simp, "Unable to create the forward FFT plan! FFT is not initialized.");
            }
            if (fReversePlan == NULL)
            {
                KTERROR(fftlog_simp, "Unable to create the reverse FFT plan! FFT is not initialized.");
            }
        }
        return;
    }

    bool KTSimpleFFT::InitializeWithHeader(KTEggHeader& header)
    {
        KTDEBUG(fftlog_simp, "Initializing via KTEggHeader");
        SetTimeSize(header.GetChannelHeader(0)->GetSliceSize());
        InitializeFFT();
        return true;
    }

    bool KTSimpleFFT::TransformData(KTTimeSeriesData& tsData)
    {
        if (tsData.GetTimeSeries(0)->GetNTimeBins() != GetTimeSize())
        {
            SetTimeSize(tsData.GetTimeSeries(0)->GetNTimeBins());
            InitializeFFT();
        }

        if (! fIsInitialized)
        {
            KTERROR(fftlog_simp, "FFT must be initialized before the transform is performed\n"
                    << "   Please first call InitializeFFT(), then use a TakeData method to set the data, and then finally perform the transform.");
            return false;
        }

        unsigned nComponents = tsData.GetNComponents();
        KTFrequencySpectrumDataPolar& newData = tsData.Of< KTFrequencySpectrumDataPolar >().SetNComponents(nComponents);

        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            const KTTimeSeriesReal* nextInput = dynamic_cast< const KTTimeSeriesReal* >(tsData.GetTimeSeries(iComponent));
            if (nextInput == NULL)
            {
                KTERROR(fftlog_simp, "Incorrect time series type: time series did not cast to KTTimeSeriesReal.");
                return false;
            }
            KTFrequencySpectrumPolar* nextResult = Transform(nextInput);
            if (nextResult == NULL)
            {
                KTERROR(fftlog_simp, "One of the channels did not transform correctly.");
                return false;
            }
            newData.SetSpectrum(nextResult, iComponent);
        }

        KTINFO(fftlog_simp, "FFT forward complete; " << nComponents << " component(s) transformed");

        return true;
    }

    bool KTSimpleFFT::TransformData(KTFrequencySpectrumDataPolar& fsData)
    {
        if (fsData.GetSpectrumPolar(0)->GetNFrequencyBins() != GetFrequencySize())
        {
            SetTimeSize((fsData.GetSpectrumPolar(0)->GetNFrequencyBins() - 1) * 2);
            InitializeFFT();
            KTWARN(fftlog_simp, "FFT initialized with " << GetTimeSize() << " time bins; to avoid ambiguity in the number of time bins, please initialize the FFT correctly before using");
        }

        if (! fIsInitialized)
        {
            KTERROR(fftlog_simp, "FFT must be initialized before the transform is performed\n"
                    << "   Please first call InitializeFFT(), then use a TakeData method to set the data, and then finally perform the transform.");
            return false;
        }

        unsigned nComponents = fsData.GetNComponents();
        KTTimeSeriesData& newData = fsData.Of< KTTimeSeriesData >().SetNComponents(nComponents);

        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            const KTFrequencySpectrumPolar* nextInput = fsData.GetSpectrumPolar(iComponent);
            if (nextInput == NULL)
            {
                KTERROR(fftlog_simp, "Frequency spectrum <" << iComponent << "> does not appear to be present.");
                return false;
            }
            KTTimeSeriesReal* nextResult = Transform(nextInput);
            if (nextResult == NULL)
            {
                KTERROR(fftlog_simp, "One of the channels did not transform correctly.");
                return false;
            }
            newData.SetTimeSeries(nextResult, iComponent);
        }

        KTINFO(fftlog_simp, "FFT reverse complete; " << nComponents << " component(s) transformed");

        return true;
    }

    bool KTSimpleFFT::TransformData(KTCorrelationData& fsData)
    {
        if (fsData.GetSpectrumPolar(0)->GetNFrequencyBins() != GetFrequencySize())
        {
            SetTimeSize((fsData.GetSpectrumPolar(0)->GetNFrequencyBins() - 1) * 2);
            InitializeFFT();
            KTWARN(fftlog_simp, "FFT initialized with " << GetTimeSize() << " time bins; to avoid ambiguity in the number of time bins, please initialize the FFT correctly before using");
        }

        if (! fIsInitialized)
        {
            KTERROR(fftlog_simp, "FFT must be initialized before the transform is performed\n"
                    << "   Please first call InitializeFFT(), then use a TakeData method to set the data, and then finally perform the transform.");
            return false;
        }

        unsigned nComponents = fsData.GetNComponents();
        KTCorrelationTSData& newData = fsData.Of< KTCorrelationTSData >().SetNComponents(nComponents);

        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            const KTFrequencySpectrumPolar* nextInput = fsData.GetSpectrumPolar(iComponent);
            if (nextInput == NULL)
            {
                KTERROR(fftlog_simp, "Frequency spectrum <" << iComponent << "> does not appear to be present.");
                return false;
            }
            KTTimeSeriesReal* nextResult = Transform(nextInput);
            if (nextResult == NULL)
            {
                KTERROR(fftlog_simp, "One of the channels did not transform correctly.");
                return false;
            }
            newData.SetTimeSeries(nextResult, iComponent);
        }

        KTINFO(fftlog_simp, "FFT reverse (corr) complete; " << nComponents << " component(s) transformed");

        return true;
    }

    bool KTSimpleFFT::TransformDataAgain(KTFrequencySpectrumDataPolar& fsData, KTTimeSeriesData& tsData)
    {
        // Forward transforming a frequency spectrum, so set the time size using the number of frequency bins
        if (fsData.GetSpectrumPolar(0)->GetNFrequencyBins() != GetTimeSize())
        {
            SetTimeSize(fsData.GetSpectrumPolar(0)->GetNFrequencyBins());
            InitializeFFT();
        }

        if (! fIsInitialized)
        {
            KTERROR(fftlog_simp, "FFT must be initialized before the transform is performed\n"
                    << "   Please first call InitializeFFT(), then use a TakeData method to set the data, and then finally perform the transform.");
            return false;
        }

        unsigned nComponents = fsData.GetNComponents();
        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            const KTFrequencySpectrumPolar* nextInput = fsData.GetSpectrumPolar(iComponent);
            if (nextInput == NULL)
            {
                KTERROR(fftlog_simp, "Frequency spectrum <" << iComponent << "> does not appear to be present.");
                return false;
            }
            KTTimeSeriesFFTW* nextResult = TransformAgain(nextInput);
            if (nextResult == NULL)
            {
                KTERROR(fftlog_simp, "One of the channels did not transform correctly.");
                return false;
            }
            tsData.SetTimeSeries(nextResult, iComponent);
        }

        KTINFO(fftlog_simp, "FFT forward again complete; " << nComponents << " component(s) transformed");

        return true;
    }

    KTFrequencySpectrumPolar* KTSimpleFFT::Transform(const KTTimeSeriesReal* data) const
    {
        unsigned nTimeBins = (unsigned)data->size();
        if (nTimeBins != fTimeSize)
        {
            KTWARN(fftlog_simp, "Number of bins in the data provided does not match the number of bins set for this transform\n"
                    << "   Bin expected: " << fTimeSize << ";   Bins in data: " << nTimeBins);
            return NULL;
        }

        double timeBinWidth = data->GetTimeBinWidth();

        copy(data->begin(), data->end(), fTSArray);

        fftw_execute(fForwardPlan);

        KTFrequencySpectrumPolar* newSpect = ExtractForwardTransformResult(GetMinFrequency(timeBinWidth), GetMaxFrequency(timeBinWidth));
        newSpect->SetNTimeBins(nTimeBins);
        return newSpect;
    }

    KTTimeSeriesReal* KTSimpleFFT::Transform(const KTFrequencySpectrumPolar* data) const
    {
        unsigned nBins = (unsigned)data->size();
        unsigned freqSize = GetFrequencySize();
        unsigned timeSize = GetTimeSize();
        if (nBins != freqSize)
        {
            KTWARN(fftlog_simp, "Number of bins in the data provided does not match the number of bins set for this transform\n"
                    << "   Bin expected: " << freqSize << ";   Bins in data: " << nBins);
            return NULL;
        }

        for (unsigned iPoint = 0; iPoint < freqSize; ++iPoint)
        {
            fFSArray[iPoint][0] = real((*data)(iPoint));
            fFSArray[iPoint][1] = imag((*data)(iPoint));
        }

        fftw_execute(fReversePlan);

        KTTimeSeriesReal* newTS = new KTTimeSeriesReal(timeSize, GetMinTime(), GetMaxTime(data->GetBinWidth()));
        copy(fTSArray, fTSArray + timeSize, newTS->begin());

        return newTS;
    }

    KTTimeSeriesFFTW* KTSimpleFFT::TransformAgain(const KTFrequencySpectrumPolar* data) const
    {
        // forward transforming a frequency spectrum, so time and frequency sizes are reversed
        unsigned nBins = (unsigned)data->size();
        unsigned timeSize = GetFrequencySize();
        unsigned freqSize = GetTimeSize();
        double timeBinWidth = data->GetFrequencyBinWidth();
        double normalization = sqrt(2. / (double)timeSize);
        if (nBins != freqSize)
        {
            KTWARN(fftlog_simp, "Number of bins in the data provided does not match the number of bins set for this transform\n"
                    << "   Bin expected: " << freqSize << ";   Bins in data: " << nBins);
            return NULL;
        }

        for (unsigned iPoint = 0; iPoint < freqSize; ++iPoint)
        {
            fTSArray[iPoint] = (*data)(iPoint).abs();
        }

        fftw_execute(fForwardPlan);

        KTTimeSeriesFFTW* newTS = new KTTimeSeriesFFTW(timeSize, GetMinFrequency(timeBinWidth), GetMaxFrequency(timeBinWidth));
        for (unsigned iPoint = 0; iPoint<freqSize; ++iPoint)
        {
            (*newTS)(iPoint)[0] = normalization * fFSArray[iPoint][0];
            (*newTS)(iPoint)[1] = normalization * fFSArray[iPoint][1];
        }

        return newTS;
    }

    KTFrequencySpectrumPolar* KTSimpleFFT::ExtractForwardTransformResult(double freqMin, double freqMax) const
    {
        unsigned freqSize = GetFrequencySize();
        double normalization = sqrt(2. / (double)GetTimeSize());

        //double tempReal, tempImag;
        KTFrequencySpectrumPolar* newSpect = new KTFrequencySpectrumPolar(freqSize, freqMin, freqMax);
        for (unsigned iPoint = 0; iPoint<freqSize; ++iPoint)
        {
            (*newSpect)(iPoint).set_rect(fFSArray[iPoint][0], fFSArray[iPoint][1]);
            (*newSpect)(iPoint) *= normalization;
        }

        return newSpect;
    }

    void KTSimpleFFT::SetTimeSize(unsigned nBins)
    {
        fTimeSize = nBins;
        if (fTSArray != NULL) fftw_free(fTSArray);
        if (fFSArray != NULL) fftw_free(fFSArray);
        fTSArray = (double*) fftw_malloc(sizeof(double) * fTimeSize);
        fFSArray = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * CalculateNFrequencyBins(fTimeSize));
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


/*
    void KTSimpleFFT::ProcessHeader(KTEggHeader* header)
    {
        SetTimeSize(header->GetSliceSize());
        InitializeFFT();
        return;
    }

    void KTSimpleFFT::ProcessTimeSeriesData(KTDataPtr data)
    {
        if (! data->Has< KTTimeSeriesData >())
        {
            KTERROR(fftlog_simp, "No time series data was present");
            return;
        }
        if (! TransformData(data->Of< KTTimeSeriesData >()))
        {
            KTERROR(fftlog_simp, "Something went wrong while performing the FFT");
            return;
        }
        fFFTSignal(data);
        return;
    }
*/

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
