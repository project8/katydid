/*
 * KTReverseFFTW.cc
 *
 *  Created on: Sep 12, 2011
 *      Author: nsoblath
 */

#include "KTReverseFFTW.hh"

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
    LOGGER(fftwlog, "KTReverseFFTW");

    KT_REGISTER_PROCESSOR(KTReverseFFTW, "reverse-fftw");

    KTReverseFFTW::KTReverseFFTW(const std::string& name) :
            KTFFTW(),
            KTProcessor(name),
            fUseWisdom(true),
            fWisdomFilename("wisdom_complexfft.fftw3"),
            fRequestedState(kNone),
            fTimeSize(0),
            fFrequencySize(0),
            fTransformFlag("ESTIMATE"),
            fTransformFlagMap(),
            fState(kNone),
            fIsInitialized(false),
            fReversePlan(),
            fInputArray(NULL),
            fROutputArray(NULL),
            fCOutputArray(NULL),
            fFFTSignal("fft", this),
            fHeaderSlot("header", this, &KTReverseFFTW::InitializeWithHeader),
            fFSFFTWToRealSlot("fs-fftw-to-real", this, &KTReverseFFTW::TransformDataToReal, &fFFTSignal),
            fFSFFTWToComplexSlot("fs-fftw-to-complex", this, &KTReverseFFTW::TransformDataToComplex, &fFFTSignal)
    {
        SetupInternalMaps();
    }

    KTReverseFFTW::~KTReverseFFTW()
    {
        FreeArrays();
        if (fReversePlan != NULL) fftw_destroy_plan(fReversePlan);
    }

    bool KTReverseFFTW::Configure(const scarab::param_node* node)
    {
        // Config-file settings
        if (node != NULL)
        {
            SetTransformFlag(node->get_value("transform-flag", fTransformFlag));

            SetUseWisdom(node->get_value<bool>("use-wisdom", fUseWisdom));
            SetWisdomFilename(node->get_value("wisdom-filename", fWisdomFilename));

            if (node->has("transform-to"))
            {
                string request(node->get_value("transform-to"));
                if (request == "real")
                {
                    SetRequestedState(kC2R);
                }
                else if (request == "complex")
                {
                    SetRequestedState(kC2C);
                }
                else
                {
                    LERROR(fftwlog, "Invalid transform request: " << request);
                    return false;
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

    bool KTReverseFFTW::InitializeForRealTDD(unsigned timeSize)
    {
        return InitializeFFT(kC2R, timeSize);
    }

    bool KTReverseFFTW::InitializeForComplexTDD(unsigned timeSize)
    {
        return InitializeFFT(kC2C, timeSize);
    }

    bool KTReverseFFTW::InitializeFromRequestedState(unsigned timeSize)
    {
        return InitializeFFT(fRequestedState, timeSize);
    }

    bool KTReverseFFTW::InitializeWithHeader(KTEggHeader& header)
    {
        return InitializeFromRequestedState(header.GetChannelHeader(0)->GetSliceSize());
    }

    bool KTReverseFFTW::InitializeFFT(KTReverseFFTW::State intendedState, unsigned timeSize)
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

        if (intendedState == kC2R)
        {
            LDEBUG(fftwlog, "Creating C2R plan: " << fTimeSize << " time bins; reverse FFT");
            // Add FFTW_PRESERVE_INPUT so that the input array content is not destroyed during the FFT
            fReversePlan = fftw_plan_dft_c2r_1d(fTimeSize, fInputArray, fROutputArray, transformFlag | FFTW_PRESERVE_INPUT);
            // deleting arrays to save space
            // output array (fROutputArray) is required; input array is not needed
            fftw_free(fInputArray);
            fInputArray = NULL;
        }
        else // intendedState == kC2C || kRasC2C
        {
            LDEBUG(fftwlog, "Creating C2C plan: " << fTimeSize << " time bins; forward FFT");
            // Add FFTW_PRESERVE_INPUT so that the input array content is not destroyed during the FFT
            fReversePlan = fftw_plan_dft_1d(fTimeSize, fInputArray, fCOutputArray, FFTW_BACKWARD, transformFlag | FFTW_PRESERVE_INPUT);
            // deleting arrays to save space; neither input nor output are needed
            FreeArrays();
        }

        if (fReversePlan != NULL)
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
            LERROR(fftwlog, "Unable to create the reverse FFT plan! FFT is not initialized.");;
            return false;
        }

        fState = intendedState;
        return true;
    }

    bool KTReverseFFTW::TransformDataToReal(KTFrequencySpectrumDataFFTW& fsData)
    {
        if (fState != kC2R)
        {
            LERROR(fftwlog, "Cannot do transform to real data in state <" << fState << ">");
            return false;
        }

        if (fsData.GetSpectrumFFTW(0)->size() != GetFrequencySize())
        {
            SetFrequencySize(fsData.GetSpectrumFFTW(0)->size());
            InitializeForRealTDD();
        }

        if (! fIsInitialized)
        {
            LERROR(fftwlog, "FFT must be initialized before the transform is performed\n"
                    << "\tPlease initialize the FFT first, then perform the transform.");
            return false;
        }

        UpdateBinningCache(fsData.GetSpectrumFFTW(0)->GetFrequencyBinWidth());

        unsigned nComponents = fsData.GetNComponents();

        KTTimeSeriesData& newData = fsData.Of< KTTimeSeriesData >().SetNComponents(nComponents);

        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            const KTFrequencySpectrumFFTW* nextInput = fsData.GetSpectrumFFTW(iComponent);
            if (nextInput == NULL)
            {
                LERROR(fftwlog, "Frequency spectrum <" << iComponent << "> does not appear to be present.");
                return false;
            }

            KTTimeSeriesReal* nextResult = FastTransformToReal(nextInput);

            if (nextResult == NULL)
            {
                LERROR(fftwlog, "One of the channels did not transform correctly.");
                return false;
            }
            newData.SetTimeSeries(nextResult, iComponent);
        }

        LDEBUG(fftwlog, "FFT complete; " << nComponents << " component(s) transformed");

        return true;
    }

    KTTimeSeriesReal* KTReverseFFTW::TransformToReal(const KTFrequencySpectrumFFTW* fs) const
    {
        if (fs->size() != fFrequencySize)
        {
            LWARN(fftwlog, "Number of bins in the data provided does not match the number of bins set for this transform\n"
                    << "\tBin expected: " << fFrequencySize << ";   Bins in data: " << fs->size());
            return NULL;
        }

        UpdateBinningCache(fs->GetFrequencyBinWidth());

        return FastTransformToReal(fs);
    }

    KTTimeSeriesReal* KTReverseFFTW::FastTransformToReal(const KTFrequencySpectrumFFTW* fs) const
    {
        KTTimeSeriesReal* newTS = new KTTimeSeriesReal(fTimeSize, fTimeMinCache, fTimeMaxCache);

        DoTransform(fs, newTS);

        return newTS;
    }

    void KTReverseFFTW::DoTransform(const KTFrequencySpectrumFFTW* fsIn, KTTimeSeriesReal* tsOut) const
    {
        fftw_complex *data = 
                        const_cast<fftw_complex*>(
                                        reinterpret_cast<const fftw_complex*>(
                                                    fsIn->GetData().data()));
        
        fftw_execute_dft_c2r(fReversePlan, data, fROutputArray);
        std::copy(fROutputArray, fROutputArray+fTimeSize, tsOut->begin());
        (*tsOut) *= sqrt(1. / double(fTimeSize));
        return;
    }

    bool KTReverseFFTW::TransformDataToComplex(KTFrequencySpectrumDataFFTW& fsData)
    {
        if (fState != kC2C)
        {
            LERROR(fftwlog, "Cannot do transform to complex data in state <" << fState << ">");
            return false;
        }

        if (fsData.GetSpectrumFFTW(0)->size() != GetFrequencySize())
        {
            SetFrequencySize(fsData.GetSpectrumFFTW(0)->size());
            InitializeForComplexTDD();
        }

        if (! fIsInitialized)
        {
            LERROR(fftwlog, "FFT must be initialized before the transform is performed\n"
                    << "\tPlease initialize the FFT first, then perform the transform.");
            return false;
        }

        UpdateBinningCache(fsData.GetSpectrumFFTW(0)->GetFrequencyBinWidth());

        unsigned nComponents = fsData.GetNComponents();

        KTTimeSeriesData& newData = fsData.Of< KTTimeSeriesData >().SetNComponents(nComponents);

        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            const KTFrequencySpectrumFFTW* nextInput = fsData.GetSpectrumFFTW(iComponent);
            if (nextInput == NULL)
            {
                LERROR(fftwlog, "Frequency spectrum <" << iComponent << "> does not appear to be present.");
                return false;
            }

            KTTimeSeriesFFTW* nextResult = FastTransformToComplex(nextInput);

            if (nextResult == NULL)
            {
                LERROR(fftwlog, "One of the channels did not transform correctly.");
                return false;
            }
            newData.SetTimeSeries(nextResult, iComponent);
        }

        LDEBUG(fftwlog, "FFT complete; " << nComponents << " component(s) transformed");

        return true;
    }

    KTTimeSeriesFFTW* KTReverseFFTW::TransformToComplex(const KTFrequencySpectrumFFTW* fs) const
    {
        if (fs->size() != fFrequencySize)
        {
            LWARN(fftwlog, "Number of bins in the data provided does not match the number of bins set for this transform\n"
                    << "\tBin expected: " << fFrequencySize << ";   Bins in data: " << fs->size());
            return NULL;
        }

        UpdateBinningCache(fs->GetFrequencyBinWidth());

        return FastTransformToComplex(fs);
    }

    KTTimeSeriesFFTW* KTReverseFFTW::FastTransformToComplex(const KTFrequencySpectrumFFTW* fs) const
    {
        KTTimeSeriesFFTW* newTS = new KTTimeSeriesFFTW(fTimeSize, fTimeMinCache, fTimeMaxCache);

        DoTransform(fs, newTS);

        return newTS;
    }

    void KTReverseFFTW::DoTransform(const KTFrequencySpectrumFFTW* fsIn, KTTimeSeriesFFTW* tsOut) const
    {
        
        fftw_complex *dataIn = 
                        const_cast<fftw_complex*>(
                                        reinterpret_cast<const fftw_complex*>(
                                                    fsIn->GetData().data()));
                                                    
        fftw_complex *dataOut = reinterpret_cast<fftw_complex*>(
                                                    tsOut->GetData().data());
        fftw_execute_dft(fReversePlan, dataIn, dataOut);
        (*tsOut) *= sqrt(1. / double(fTimeSize));
        return;
    }

    void KTReverseFFTW::SetTimeSize(unsigned nBins)
    {
        SetTimeSizeForState(nBins, fState);
        return;
    }

    void KTReverseFFTW::SetTimeSizeForState(unsigned nBins, KTReverseFFTW::State intendedState)
    {
        fTimeSize = nBins;
        if (intendedState == kC2R)
        {
            fFrequencySize = nBins / 2 + 1;
        }
        else if (intendedState == kC2C)
        {
            fFrequencySize = nBins;
        }
        else
        {
            LDEBUG(fftwlog, "Time size set while in state <" << fState << ">; frequency size not changed");
        }

        // clear things for good measure
        FreeArrays();

        fIsInitialized = false;
        return;
    }

    void KTReverseFFTW::SetFrequencySize(unsigned nBins)
    {
        SetFrequencySizeForState(nBins, fState);
        return;
    }

    void KTReverseFFTW::SetFrequencySizeForState(unsigned nBins, KTReverseFFTW::State intendedState)
    {
        fFrequencySize = nBins;
        if (intendedState == kC2R)
        {
            fTimeSize = (nBins - 1) * 2;
        }
        else if (intendedState == kC2C)
        {
            fTimeSize = nBins;
        }
        else
        {
            LDEBUG(fftwlog, "Frequency size set while in state <" << fState << ">; time size not changed");
        }

        // clear things for good measure
        FreeArrays();

        fIsInitialized = false;
        return;
    }

    void KTReverseFFTW::SetTransformFlag(const std::string& flag)
    {
        if (fTransformFlagMap.find(flag) == fTransformFlagMap.end())
        {
            LWARN(fftwlog, "Invalid transform flag requested: " << flag << "\n\tNo change was made.");
            return;
        }

        // delete the plan
        if (fReversePlan != NULL) fftw_destroy_plan(fReversePlan);

        fTransformFlag = flag;
        fIsInitialized = false;
        return;
    }

    void KTReverseFFTW::SetupInternalMaps()
    {
        // transform flag map
        fTransformFlagMap.clear();
        fTransformFlagMap["ESTIMATE"] = FFTW_ESTIMATE;
        fTransformFlagMap["MEASURE"] = FFTW_MEASURE;
        fTransformFlagMap["PATIENT"] = FFTW_PATIENT;
        fTransformFlagMap["EXHAUSTIVE"] = FFTW_EXHAUSTIVE;
        return;
    }

    bool KTReverseFFTW::AllocateArrays(State intendedState)
    {
        FreeArrays();
        if (intendedState == kNone) intendedState = fState;
        if (intendedState == kNone)
        {
            LERROR(fftwlog, "Cannot allocate arrays for state <" << intendedState << ">");
            return false;
        }

        if (fInputArray == NULL)
        {
            fInputArray = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * fFrequencySize);
        }
        if (intendedState == kC2R)
        {
            if (fROutputArray == NULL)
            {
                fROutputArray = (double*) fftw_malloc(sizeof(double) * fTimeSize);
            }
        }
        else //  intendedState == kC2C
        {
            if (fCOutputArray == NULL)
            {
                fCOutputArray = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * fTimeSize);
            }
        }

        return true;
    }

    void KTReverseFFTW::FreeArrays()
    {
        if (fROutputArray != NULL)
        {
            fftw_free(fROutputArray);
            fROutputArray = NULL;
        }
        if (fCOutputArray != NULL)
        {
            fftw_free(fCOutputArray);
            fCOutputArray = NULL;
        }
        if (fInputArray != NULL)
        {
            fftw_free(fInputArray);
            fInputArray = NULL;
        }
        return;
    }


} /* namespace Katydid */
