/*
 * KTForwardFFTW.cc
 *
 *  Created on: Sep 12, 2011
 *      Author: nsoblath
 */

#include "KTForwardFFTW.hh"

#include "KTAnalyticAssociateData.hh"
#include "KTCacheDirectory.hh"
#include "KTEggHeader.hh"
#include "KTFrequencySpectrumDataFFTW.hh"
#include "logger.hh"
#include "KTTimeSeriesData.hh"
#include "KTTimeSeriesFFTW.hh"
#include "KTTimeSeriesReal.hh"

#include <algorithm>
#include <cmath>

using std::string;
using std::vector;


namespace Katydid
{
    LOGGER(fftwlog, "KTForwardFFTW");

    KT_REGISTER_PROCESSOR(KTForwardFFTW, "forward-fftw");

    KTForwardFFTW::KTForwardFFTW(const std::string& name) :
            KTFFTW(),
            KTProcessor(name),
            fUseWisdom(true),
            fWisdomFilename("wisdom_complexfft.fftw3"),
            fComplexAsIQ(false),
            fTimeSize(0),
            fFrequencySize(0),
            fTransformFlag("ESTIMATE"),
            fTransformFlagMap(),
            fState(kNone),
            fIsInitialized(false),
            fForwardPlan(),
            fRInputArray(NULL),
            fCInputArray(NULL),
            fOutputArray(NULL),
            fFFTSignal("fft", this),
            fHeaderSlot("header", this, &KTForwardFFTW::InitializeWithHeader),
            fTSRealSlot("ts-real", this, &KTForwardFFTW::TransformRealData, &fFFTSignal),
            fTSComplexSlot("ts-fftw", this, &KTForwardFFTW::TransformComplexData, &fFFTSignal),
            fAASlot("aa", this, &KTForwardFFTW::TransformComplexData, &fFFTSignal),
            fTSRealAsComplexSlot("ts-real-as-complex", this, &KTForwardFFTW::TransformRealDataAsComplex, &fFFTSignal)
    {
        SetupInternalMaps();
    }

    KTForwardFFTW::~KTForwardFFTW()
    {
        FreeArrays();
        if (fForwardPlan != NULL) fftw_destroy_plan(fForwardPlan);
    }

    bool KTForwardFFTW::Configure(const scarab::param_node* node)
    {
        // Config-file settings
        if (node != NULL)
        {
            SetTransformFlag(node->get_value("transform-flag", fTransformFlag));

            SetUseWisdom(node->get_value<bool>("use-wisdom", fUseWisdom));
            SetWisdomFilename(node->get_value("wisdom-filename", fWisdomFilename));

            SetComplexAsIQ(node->get_value("transform-complex-as-iq", fComplexAsIQ));

            if( node->has("transform-state") )
            {
                string intendedState(node->get_value("transform-state"));
                if (intendedState == "r2c") fState = kR2C;
                else if (intendedState == "c2c") fState = kC2C;
                else if (intendedState == "rasc2c") fState = kRasC2C;
                else
                {
                    LERROR(fftwlog, "Invalid transform state requested: <" << intendedState << ">");
                    return false;
                }
            }
            else
            {
                if (node->has("transform-complex-as-iq"))
                {
                    LWARN(fftwlog, "Transform-complex-as-iq was requested, but the transform-state was not specified; the former setting will be ignored");
                }
            }
        }

        if (fUseWisdom)
        {
            if (! Nymph::KTCacheDirectory::get_instance()->Configure())
            {
                LWARN(fftwlog, "Unable to use wisdom because cache directory is not ready.");
                fUseWisdom = false;
            }
        }

        // Command-line settings
        //SetTransformFlag(fCLHandler->GetCommandLineValue< string >("transform-flag", fTransformFlag));

        return true;
    }

    bool KTForwardFFTW::InitializeForRealTDD(unsigned timeSize)
    {
        return InitializeFFT(kR2C, timeSize);
    }

    bool KTForwardFFTW::InitializeForRealAsComplexTDD(unsigned timeSize)
    {
        return InitializeFFT(kRasC2C, timeSize);
    }

    bool KTForwardFFTW::InitializeForComplexTDD(unsigned timeSize)
    {
        return InitializeFFT(kC2C, timeSize);
    }

    bool KTForwardFFTW::InitializeFFT(KTForwardFFTW::State intendedState, unsigned timeSize)
    {
        if (intendedState == kNone)
        {
            LERROR(fftwlog, "Cannot initialize FFT for state <" << intendedState << ">");
            return false;
        }

        if (timeSize == 0) timeSize = fTimeSize;

        // Set the time size whether or not it's different from fTimeSize, since the frequency size might not be
        // right for the intended state.
        SetTimeSizeForState(timeSize, intendedState);

        // fTransformFlag is guaranteed to be valid in the Set method.
        LDEBUG(fftwlog, "Transform flag: " << fTransformFlag);
        TransformFlagMap::const_iterator iter = fTransformFlagMap.find(fTransformFlag);
        unsigned transformFlag = iter->second;

        // allocate the input and output arrays if they're not there already
        if (! AllocateArrays(intendedState))
        {
            LERROR(fftwlog, "Unable to allocate arrays");
            return false;
        }

        if (fUseWisdom)
        {
            LDEBUG(fftwlog, "Reading wisdom from file <" << fWisdomFilename << ">");
            if (fftw_import_wisdom_from_filename(fWisdomFilename.c_str()) == 0)
            {
                LWARN(fftwlog, "Unable to read FFTW wisdom from file <" << fWisdomFilename << ">");
            }
        }

        InitializeMultithreaded();

        if (intendedState == kR2C)
        {
            LDEBUG(fftwlog, "Creating R2C plan: " << fTimeSize << " time bins; forward FFT");
            // No FFTW_PRESERVE_INPUT, since the input array contents are replaced for each FFT
            fForwardPlan = fftw_plan_dft_r2c_1d(fTimeSize, fRInputArray, fOutputArray, transformFlag);
            // deleting arrays to save space
            // input array is required; output array is not needed
            LDEBUG(fftwlog, "Freeing output array");
            fftw_free(fOutputArray);
            fOutputArray = NULL;
        }
        else if (intendedState == kC2C)
        {
            LDEBUG(fftwlog, "Creating C2C plan: " << fTimeSize << " time bins; forward FFT");
            // Add FFTW_PRESERVE_INPUT so that the input array content is not destroyed during the FFT
            fForwardPlan = fftw_plan_dft_1d(fTimeSize, fCInputArray, fOutputArray, FFTW_FORWARD, transformFlag | FFTW_PRESERVE_INPUT);
            // deleting arrays to save space
            FreeArrays();
        }
        else // intendedState == kRasC2C
        {
            LDEBUG(fftwlog, "Creating RasC2C plan: " << fTimeSize << " time bins; forward FFT");
            // No FFTW_PRESERVE_INPUT, since the input array contents are replaced for each FFT
            fForwardPlan = fftw_plan_dft_1d(fTimeSize, fCInputArray, fOutputArray, FFTW_FORWARD, transformFlag);
            // deleting arrays to save space
            // input array not required for C2C, but is for kRasC2C; output array not needed in either case
            LDEBUG(fftwlog, "Freeing output array");
            fftw_free(fOutputArray);
            fOutputArray = NULL;
        }

        if (fForwardPlan != NULL)
        {
            fIsInitialized = true;
            if (fUseWisdom)
            {
                if (fftw_export_wisdom_to_filename(fWisdomFilename.c_str()) == 0)
                {
                    LWARN(fftwlog, "Unable to write FFTW wisdom to file <" << fWisdomFilename << ">");
                }
            }
            LDEBUG(fftwlog, "FFTW plan created; Initialization complete.");
        }
        else
        {
            fIsInitialized = false;
            LERROR(fftwlog, "Unable to create the forward FFT plan! FFT is not initialized.");;
            return false;
        }

        fState = intendedState;
        return true;
    }

    bool KTForwardFFTW::InitializeWithHeader(KTEggHeader& header)
    {
        if (fState == kNone)
        {
            if (header.GetChannelHeader(0)->GetTSDataType() == KTChannelHeader::kReal)
            {
                return InitializeForRealTDD(header.GetChannelHeader(0)->GetSliceSize());
            }
            else // == KTChannelHeader::kComplex || KTChannelHeader::kIQ
            {
                if (header.GetChannelHeader(0)->GetTSDataType() == KTChannelHeader::kIQ) fComplexAsIQ = true;
                else fComplexAsIQ = false;
                return InitializeForComplexTDD(header.GetChannelHeader(0)->GetSliceSize());
            }
        }
        else
        {
            return InitializeFFT(fState, header.GetChannelHeader(0)->GetSliceSize());
        }
    }

    bool KTForwardFFTW::TransformRealData(KTTimeSeriesData& tsData)
    {
        if (fState != kR2C)
        {
            LERROR(fftwlog, "Cannot do transform of real data in state <" << fState << ">");
            return false;
        }

        if (tsData.GetTimeSeries(0)->GetNTimeBins() != GetTimeSize())
        {
            SetTimeSize(tsData.GetTimeSeries(0)->GetNTimeBins());
            InitializeForRealTDD();
        }

        if (! fIsInitialized)
        {
            LERROR(fftwlog, "FFT must be initialized before the transform is performed\n"
                    << "\tPlease initialize the FFT first, then perform the transform.");
            return false;
        }

        double timeBinWidth = tsData.GetTimeSeries(0)->GetTimeBinWidth();
        UpdateBinningCache(timeBinWidth);

        unsigned nComponents = tsData.GetNComponents();

        KTFrequencySpectrumDataFFTW& newData = tsData.Of< KTFrequencySpectrumDataFFTW >().SetNComponents(nComponents);

        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            const KTTimeSeriesReal* nextInput = dynamic_cast< const KTTimeSeriesReal* >(tsData.GetTimeSeries(iComponent));
            if (nextInput == NULL)
            {
                LERROR(fftwlog, "Incorrect time series type: time series did not cast to KTTimeSeriesReal.");
                return false;
            }

            KTFrequencySpectrumFFTW* nextResult = FastTransform(nextInput);

            if (nextResult == NULL)
            {
                LERROR(fftwlog, "Channel <" << iComponent << "> did not transform correctly.");
                return false;
            }
            LDEBUG(fftwlog, "FFT computed; size: " << nextResult->size() << "; range: " << nextResult->GetRangeMin() << " -> " << nextResult->GetRangeMax() << "; TimeBinWidth=" << timeBinWidth);
            newData.SetSpectrum(nextResult, iComponent);
        }

        LINFO(fftwlog, "FFT complete; " << nComponents << " channel(s) transformed");

        return true;
    }

    bool KTForwardFFTW::TransformRealDataAsComplex(KTTimeSeriesData& tsData)
    {
        if (fState != kRasC2C)
        {
            LERROR(fftwlog, "Cannot do transform of real-as-complex data in state <" << fState << ">");
            return false;
        }

        if (tsData.GetTimeSeries(0)->GetNTimeBins() != GetTimeSize())
        {
            SetTimeSize(tsData.GetTimeSeries(0)->GetNTimeBins());
            InitializeForRealTDD();
        }

        if (! fIsInitialized)
        {
            LERROR(fftwlog, "FFT must be initialized before the transform is performed\n"
                    << "\tPlease initialize the FFT first, then perform the transform.");
            return false;
        }

        UpdateBinningCache(tsData.GetTimeSeries(0)->GetTimeBinWidth());

        unsigned nComponents = tsData.GetNComponents();

        KTFrequencySpectrumDataFFTW& newData = tsData.Of< KTFrequencySpectrumDataFFTW >().SetNComponents(nComponents);

        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            const KTTimeSeriesReal* nextInput = dynamic_cast< const KTTimeSeriesReal* >(tsData.GetTimeSeries(iComponent));
            if (nextInput == NULL)
            {
                LERROR(fftwlog, "Incorrect time series type: time series did not cast to KTTimeSeriesReal.");
                return false;
            }

            KTFrequencySpectrumFFTW* nextResult = FastTransformAsComplex(nextInput);

            if (nextResult == NULL)
            {
                LERROR(fftwlog, "Channel <" << iComponent << "> did not transform correctly.");
                return false;
            }
            LDEBUG(fftwlog, "FFT computed; size: " << nextResult->size() << "; range: " << nextResult->GetRangeMin() << " - " << nextResult->GetRangeMax());
            newData.SetSpectrum(nextResult, iComponent);
        }

        LINFO(fftwlog, "FFT complete; " << nComponents << " channel(s) transformed");

        return true;
    }

    bool KTForwardFFTW::TransformComplexData(KTTimeSeriesData& tsData)
    {
        if (fState != kC2C)
        {
            LERROR(fftwlog, "Cannot do transform of complex data in state <" << fState << ">");
            return false;
        }

        if (tsData.GetTimeSeries(0)->GetNTimeBins() != GetTimeSize())
        {
            SetTimeSize(tsData.GetTimeSeries(0)->GetNTimeBins());
            InitializeForComplexTDD();
        }

        if (! fIsInitialized)
        {
            LERROR(fftwlog, "FFT must be initialized before the transform is performed\n"
                    << "\tPlease initialize the FFT first, then perform the transform.");
            return false;
        }

        UpdateBinningCache(tsData.GetTimeSeries(0)->GetTimeBinWidth());

        unsigned nComponents = tsData.GetNComponents();

        KTFrequencySpectrumDataFFTW& newData = tsData.Of< KTFrequencySpectrumDataFFTW >().SetNComponents(nComponents);

        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            const KTTimeSeriesFFTW* nextInput = dynamic_cast< const KTTimeSeriesFFTW* >(tsData.GetTimeSeries(iComponent));
            if (nextInput == NULL)
            {
                LERROR(fftwlog, "Incorrect time series type: time series did not cast to KTTimeSeriesFFTW.");
                return false;
            }

            KTFrequencySpectrumFFTW* nextResult = FastTransform(nextInput);

            if (nextResult == NULL)
            {
                LERROR(fftwlog, "Channel <" << iComponent << "> did not transform correctly.");
                return false;
            }
            LDEBUG(fftwlog, "FFT computed; size: " << nextResult->size() << "; range: " << nextResult->GetRangeMin() << " - " << nextResult->GetRangeMax());
            newData.SetSpectrum(nextResult, iComponent);
        }

        LINFO(fftwlog, "FFT complete; " << nComponents << " channel(s) transformed");

        return true;
    }

    bool KTForwardFFTW::TransformComplexData(KTAnalyticAssociateData& tsData)
    {
        if (fState != kC2C)
        {
            LERROR(fftwlog, "Cannot do transform of complex data in state <" << fState << ">");
            return false;
        }

        if (tsData.GetTimeSeries(0)->GetNTimeBins() != fTimeSize)
        {
            SetTimeSize(tsData.GetTimeSeries(0)->GetNTimeBins());
            InitializeForComplexTDD();
        }

        if (! fIsInitialized)
        {
            LERROR(fftwlog, "FFT must be initialized before the transform is performed\n"
                    << "   Please first call InitializeFFT(), then perform the transform.");
            return false;
        }

        UpdateBinningCache(tsData.GetTimeSeries(0)->GetTimeBinWidth());

        unsigned nComponents = tsData.GetNComponents();

        KTFrequencySpectrumDataFFTW& newData = tsData.Of< KTFrequencySpectrumDataFFTW >().SetNComponents(nComponents);

        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            const KTTimeSeriesFFTW* nextInput = dynamic_cast< const KTTimeSeriesFFTW* >(tsData.GetTimeSeries(iComponent));
            if (nextInput == NULL)
            {
                LERROR(fftwlog, "Incorrect time series type: time series did not cast to KTTimeSeriesFFTW.");
                return false;
            }

            KTFrequencySpectrumFFTW* nextResult = FastTransform(nextInput);

            if (nextResult == NULL)
            {
                LERROR(fftwlog, "Channel <" << iComponent << "> did not transform correctly.");
                return false;
            }
            LDEBUG(fftwlog, "FFT computed; size: " << nextResult->size() << "; range: " << nextResult->GetRangeMin() << " - " << nextResult->GetRangeMax());
            newData.SetSpectrum(nextResult, iComponent);
        }

        LINFO(fftwlog, "FFT complete; " << nComponents << " channel(s) transformed");

        return true;
    }

    KTFrequencySpectrumFFTW* KTForwardFFTW::Transform(const KTTimeSeriesReal* ts) const
    {
        if (ts->size() != fTimeSize)
        {
            LWARN(fftwlog, "Number of bins in the data provided does not match the number of bins set for this transform\n"
                    << "   Bin expected: " << fTimeSize << ";   Bins in data: " << ts->size());
            return NULL;
        }

        UpdateBinningCache(ts->GetTimeBinWidth());

        return FastTransform(ts);
    }

    KTFrequencySpectrumFFTW* KTForwardFFTW::FastTransform(const KTTimeSeriesReal* ts) const
    {
        KTFrequencySpectrumFFTW* newFS = new KTFrequencySpectrumFFTW(fFrequencySize, fFreqMinCache, fFreqMaxCache, false);

        DoTransform(ts, newFS);

        newFS->SetNTimeBins(fTimeSize);

        return newFS;
    }

    void KTForwardFFTW::DoTransform(const KTTimeSeriesReal* tsIn, KTFrequencySpectrumFFTW* fsOut) const
    {
        std::copy(tsIn->begin(), tsIn->end(), fRInputArray);
        fftw_execute_dft_r2c(fForwardPlan, fRInputArray, reinterpret_cast<fftw_complex*>(fsOut->GetData().data()));
        (*fsOut) *= sqrt(2. / (double)fTimeSize);
        return;
    }

    KTFrequencySpectrumFFTW* KTForwardFFTW::TransformAsComplex(const KTTimeSeriesReal* ts) const
    {
        if (ts->size() != fTimeSize)
        {
            LWARN(fftwlog, "Number of bins in the data provided does not match the number of bins set for this transform\n"
                    << "   Bin expected: " << fTimeSize << ";   Bins in data: " << ts->size());
            return NULL;
        }

        UpdateBinningCache(ts->GetTimeBinWidth());

        return FastTransformAsComplex(ts);
    }

    KTFrequencySpectrumFFTW* KTForwardFFTW::FastTransformAsComplex(const KTTimeSeriesReal* ts) const
    {
        KTFrequencySpectrumFFTW* newFS = new KTFrequencySpectrumFFTW(fFrequencySize, fFreqMinCache, fFreqMaxCache, true);

        DoTransformAsComplex(ts, newFS);

        newFS->SetNTimeBins(fTimeSize);

        return newFS;
    }

    void KTForwardFFTW::DoTransformAsComplex(const KTTimeSeriesReal* tsIn, KTFrequencySpectrumFFTW* fsOut) const
    {
        for (unsigned iBin = 0; iBin < fTimeSize; ++iBin)
        {
            fCInputArray[iBin][0] = tsIn->GetData()[iBin];
            fCInputArray[iBin][1] = 0;
        }
        fftw_execute_dft(fForwardPlan, fCInputArray, reinterpret_cast<fftw_complex*>(fsOut->GetData().data()));
        (*fsOut) *= sqrt(1. / (double)fTimeSize);
        return;
    }

    KTFrequencySpectrumFFTW* KTForwardFFTW::Transform(const KTTimeSeriesFFTW* ts) const
    {
        if (ts->size() != fTimeSize)
        {
            LWARN(fftwlog, "Number of bins in the data provided does not match the number of bins set for this transform\n"
                    << "   Bin expected: " << fTimeSize << ";   Bins in data: " << ts->size());
            return NULL;
        }

        UpdateBinningCache(ts->GetTimeBinWidth());

        return FastTransform(ts);
    }

    KTFrequencySpectrumFFTW* KTForwardFFTW::FastTransform(const KTTimeSeriesFFTW* ts) const
    {
        KTFrequencySpectrumFFTW* newFS = new KTFrequencySpectrumFFTW(fFrequencySize, fFreqMinCache, fFreqMaxCache, true);

        DoTransform(ts, newFS);

        newFS->SetNTimeBins(fTimeSize);

        return newFS;
    }

    void KTForwardFFTW::DoTransform(const KTTimeSeriesFFTW* tsIn, KTFrequencySpectrumFFTW* fsOut) const
    {
        fftw_complex *dataIn = 
                        const_cast<fftw_complex*>(
                                        reinterpret_cast<const fftw_complex*>(
                                                    tsIn->GetData().data()));
        fftw_complex *dataOut = reinterpret_cast<fftw_complex*>(
                                                    fsOut->GetData().data());
        fftw_execute_dft(fForwardPlan, dataIn, dataOut);
        (*fsOut) *= sqrt(1. / (double)  fTimeSize);
        return;
    }

    void KTForwardFFTW::SetTimeSize(unsigned nBins)
    {
        SetTimeSizeForState(nBins, fState);
        return;
    }

    void KTForwardFFTW::SetTimeSizeForState(unsigned nBins, KTForwardFFTW::State intendedState)
    {
        fTimeSize = nBins;
        if (intendedState == kR2C)
        {
            fFrequencySize = nBins / 2 + 1;
        }
        else if (intendedState == kC2C || intendedState == kRasC2C)
        {
            fFrequencySize = nBins;
        }
        else
        {
            LDEBUG(fftwlog, "Time size set while in state <" << fState << ">; frequency size not changed");
        }
        LDEBUG(fftwlog, "Time size set to " << fTimeSize << "; frequency size set to " << fFrequencySize);

        // clear things for good measure
        FreeArrays();

        fIsInitialized = false;
        return;
    }

   void KTForwardFFTW::SetTransformFlag(const std::string& flag)
    {
        if (fTransformFlagMap.find(flag) == fTransformFlagMap.end())
        {
            LWARN(fftwlog, "Invalid transform flag requested: " << flag << "\n\tNo change was made.");
            return;
        }

        // delete the plan
        if (fForwardPlan != NULL) fftw_destroy_plan(fForwardPlan);

        fTransformFlag = flag;
        fIsInitialized = false;
        return;
    }

    void KTForwardFFTW::SetupInternalMaps()
    {
        // transform flag map
        fTransformFlagMap.clear();
        fTransformFlagMap["ESTIMATE"] = FFTW_ESTIMATE;
        fTransformFlagMap["MEASURE"] = FFTW_MEASURE;
        fTransformFlagMap["PATIENT"] = FFTW_PATIENT;
        fTransformFlagMap["EXHAUSTIVE"] = FFTW_EXHAUSTIVE;
        return;
    }

    bool KTForwardFFTW::AllocateArrays(State intendedState)
    {
        FreeArrays();
        if (intendedState == kNone) intendedState = fState;
        if (intendedState == kNone)
        {
            LERROR(fftwlog, "Cannot allocate arrays for state <" << intendedState << ">");
            return false;
        }

        if (intendedState == kR2C)
        {
            if (fRInputArray == NULL)
            {
                LDEBUG(fftwlog, "Allocating real input array");
                fRInputArray = (double*) fftw_malloc(sizeof(double) * fTimeSize);
            }
        }
        else //  intendedState == kC2C or kRasC2C
        {
            if (fCInputArray == NULL)
            {
                LDEBUG(fftwlog, "Allocating complex input array");
                fCInputArray = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * fTimeSize);
            }
        }
        if (fOutputArray == NULL)
        {
            LDEBUG(fftwlog, "Allocating output array");
            fOutputArray = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * fFrequencySize);
        }

        return true;
    }

    void KTForwardFFTW::FreeArrays()
    {
        if (fRInputArray != NULL)
        {
            LDEBUG(fftwlog, "Freeing real input array");
            fftw_free(fRInputArray);
            fRInputArray = NULL;
        }
        if (fCInputArray != NULL)
        {
            LDEBUG(fftwlog, "Freeing complex input array");
            fftw_free(fCInputArray);
            fCInputArray = NULL;
        }
        if (fOutputArray != NULL)
        {
            LDEBUG(fftwlog, "Freeing output array");
            fftw_free(fOutputArray);
            fOutputArray = NULL;
        }
        return;
    }


} /* namespace Katydid */
