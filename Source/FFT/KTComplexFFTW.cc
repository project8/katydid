/*
 * KTComplexFFTW.cc
 *
 *  Created on: Sep 12, 2011
 *      Author: nsoblath
 */

#include "KTComplexFFTW.hh"

#include "KTCacheDirectory.hh"
#include "KTEggHeader.hh"
#include "KTFactory.hh"
#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTTimeSeriesData.hh"
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
            fFFTForwardSignal(),
            fFFTReverseSignal()
    {
        fConfigName = "complex-fftw";

        RegisterSignal("fft-forward", &fFFTForwardSignal, "void (shared_ptr<KTData>)");
        RegisterSignal("fft-reverse", &fFFTReverseSignal, "void (shared_ptr<KTData>)");

        RegisterSlot("header", this, &KTComplexFFTW::ProcessHeader, "void (const KTEggHeader*)");
        RegisterSlot("ts", this, &KTComplexFFTW::ProcessTimeSeriesData, "void (shared_ptr<KTData>)");
        RegisterSlot("fs-fftw", this, &KTComplexFFTW::ProcessFrequencySpectrumDataFFTW, "void (shared_ptr<KTData>)");

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

    Bool_t KTComplexFFTW::TransformData(KTTimeSeriesData& tsData)
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
            return NULL;
        }

        UInt_t nComponents = tsData.GetNComponents();

        KTFrequencySpectrumDataFFTW& newData = tsData.Of< KTFrequencySpectrumDataFFTW >().SetNComponents(nComponents);

        for (UInt_t iComponent = 0; iComponent < nComponents; iComponent++)
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
            newData.SetSpectrum(nextResult, iComponent);
        }

        KTDEBUG(fftlog_comp, "FFT complete; " << nComponents << " channel(s) transformed");

        return true;
    }

    Bool_t KTComplexFFTW::TransformData(KTFrequencySpectrumDataFFTW& fsData)
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

        UInt_t nComponents = fsData.GetNComponents();

        KTTimeSeriesData& newData = fsData.Of< KTTimeSeriesData >().SetNComponents(nComponents);

        for (UInt_t iComponent = 0; iComponent < nComponents; iComponent++)
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

    void KTComplexFFTW::ProcessTimeSeriesData(shared_ptr<KTData> data)
    {
        if (! data->Has< KTTimeSeriesData >())
        {
            KTERROR(fftlog_comp, "No time series data was present");
            return;
        }
        if (! TransformData(data->Of< KTTimeSeriesData >()))
        {
            KTERROR(fftlog_comp, "Something went wrong while performing a forward FFT");
            return;
        }
        fFFTForwardSignal(data);
        return;
    }

    void KTComplexFFTW::ProcessFrequencySpectrumDataFFTW(shared_ptr<KTData> data)
    {
        if (! data->Has< KTFrequencySpectrumDataFFTW >())
        {
            KTERROR(fftlog_comp, "No frequency spectrum data was present");
            return;
        }
        if (! TransformData(data->Of< KTFrequencySpectrumDataFFTW >()))
        {
            KTERROR(fftlog_comp, "Something went wrong while performing a forward FFT");
            return;
        }
        fFFTReverseSignal(data);
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
