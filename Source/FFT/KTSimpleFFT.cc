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
#include "KTNOFactory.hh"
#include "KTFrequencySpectrumDataPolar.hh"
#include "KTTimeSeriesData.hh"
#include "KTTimeSeriesReal.hh"
#include "KTPStoreNode.hh"

#include <algorithm>
#include <cmath>

using std::copy;
using std::string;
using std::vector;


namespace Katydid
{

    static KTDerivedNORegistrar< KTProcessor, KTSimpleFFT > sSimpleFFTRegistrar("simple-fft");

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
            fHeaderSlot("header", this, &KTSimpleFFT::InitializeWithHeader),
            fTimeSeriesSlot("ts", this, &KTSimpleFFT::TransformData, &fFFTForwardSignal),
            fFSPolarSlot("fs-polar", this, &KTSimpleFFT::TransformData, &fFFTReverseSignal),
            fCorrSlot("corr", this, &KTSimpleFFT::TransformData, &fFFTReverseCorrSignal)
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

    Bool_t KTSimpleFFT::Configure(const KTPStoreNode* node)
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
        Int_t transformFlag = iter->second;

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

    void KTSimpleFFT::InitializeWithHeader(const KTEggHeader* header)
    {
        KTDEBUG(fftlog_simp, "Initializing via KTEggHeader");
        SetTimeSize(header->GetSliceSize());
        InitializeFFT();
        return;
    }

    Bool_t KTSimpleFFT::TransformData(KTTimeSeriesData& tsData)
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

        UInt_t nComponents = tsData.GetNComponents();
        KTFrequencySpectrumDataPolar& newData = tsData.Of< KTFrequencySpectrumDataPolar >().SetNComponents(nComponents);

        for (UInt_t iComponent = 0; iComponent < nComponents; iComponent++)
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

        KTINFO(fftlog_simp, "FFT complete; " << nComponents << " component(s) transformed");

        return true;
    }

    Bool_t KTSimpleFFT::TransformData(KTFrequencySpectrumDataPolar& fsData)
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

        UInt_t nComponents = fsData.GetNComponents();
        KTTimeSeriesData& newData = fsData.Of< KTTimeSeriesData >().SetNComponents(nComponents);

        for (UInt_t iComponent = 0; iComponent < nComponents; iComponent++)
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

        KTINFO(fftlog_simp, "FFT complete; " << nComponents << " component(s) transformed");

        return true;
    }

    Bool_t KTSimpleFFT::TransformData(KTCorrelationData& fsData)
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

        UInt_t nComponents = fsData.GetNComponents();
        KTCorrelationTSData& newData = fsData.Of< KTCorrelationTSData >().SetNComponents(nComponents);

        for (UInt_t iComponent = 0; iComponent < nComponents; iComponent++)
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

        KTINFO(fftlog_simp, "FFT complete; " << nComponents << " component(s) transformed");

        return true;
    }

    KTFrequencySpectrumPolar* KTSimpleFFT::Transform(const KTTimeSeriesReal* data) const
    {
        UInt_t nTimeBins = (UInt_t)data->size();
        if (nTimeBins != fTimeSize)
        {
            KTWARN(fftlog_simp, "Number of bins in the data provided does not match the number of bins set for this transform\n"
                    << "   Bin expected: " << fTimeSize << ";   Bins in data: " << nTimeBins);
            return NULL;
        }

        Double_t timeBinWidth = data->GetTimeBinWidth();

        copy(data->begin(), data->end(), fTSArray);

        fftw_execute(fForwardPlan);

        return ExtractForwardTransformResult(GetMinFrequency(timeBinWidth), GetMaxFrequency(timeBinWidth));
    }

    KTTimeSeriesReal* KTSimpleFFT::Transform(const KTFrequencySpectrumPolar* data) const
    {
        UInt_t nBins = (UInt_t)data->size();
        UInt_t freqSize = GetFrequencySize();
        UInt_t timeSize = GetTimeSize();
        if (nBins != freqSize)
        {
            KTWARN(fftlog_simp, "Number of bins in the data provided does not match the number of bins set for this transform\n"
                    << "   Bin expected: " << freqSize << ";   Bins in data: " << nBins);
            return NULL;
        }

        for (UInt_t iPoint = 0; iPoint < freqSize; iPoint++)
        {
            fFSArray[iPoint][0] = real((*data)(iPoint));
            fFSArray[iPoint][1] = imag((*data)(iPoint));
        }

        fftw_execute(fReversePlan);

        KTTimeSeriesReal* newTS = new KTTimeSeriesReal(timeSize, GetMinTime(), GetMaxTime(data->GetBinWidth()));
        copy(fTSArray, fTSArray + timeSize, newTS->begin());

        return newTS;
    }

    KTFrequencySpectrumPolar* KTSimpleFFT::ExtractForwardTransformResult(Double_t freqMin, Double_t freqMax) const
    {
        UInt_t freqSize = GetFrequencySize();
        Double_t normalization = sqrt(2. / (Double_t)GetTimeSize());

        //Double_t tempReal, tempImag;
        KTFrequencySpectrumPolar* newSpect = new KTFrequencySpectrumPolar(freqSize, freqMin, freqMax);
        for (UInt_t iPoint = 0; iPoint<freqSize; iPoint++)
        {
            (*newSpect)(iPoint).set_rect(fFSArray[iPoint][0], fFSArray[iPoint][1]);
            (*newSpect)(iPoint) *= normalization;
        }

        return newSpect;
    }

    void KTSimpleFFT::SetTimeSize(UInt_t nBins)
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
    void KTSimpleFFT::ProcessHeader(const KTEggHeader* header)
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
