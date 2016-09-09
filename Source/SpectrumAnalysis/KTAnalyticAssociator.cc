/*
 * KTAnalyticAssociator.cc
 *
 *  Created on: Dec 17, 2012
 *      Author: nsoblath
 */

#include "KTAnalyticAssociator.hh"

#include "KTAnalyticAssociateData.hh"
#include "KTEggHeader.hh"
#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTFrequencySpectrumFFTW.hh"
#include "KTNormalizedFSData.hh"
#include "KTTimeSeriesFFTW.hh"
#include "KTTimeSeriesReal.hh"

using std::string;

namespace Katydid
{
    KTLOGGER(aalog, "KTAnalyticAssociator");

    KT_REGISTER_PROCESSOR(KTAnalyticAssociator, "analytic-associator");

    KTAnalyticAssociator::KTAnalyticAssociator(const std::string& name) :
            KTProcessor(name),
            fForwardFFT(),
            fReverseFFT(),
            fSaveFrequencySpectrum(false),
            fAASignal("aa", this),
            fHeaderSlot("header", this, &KTAnalyticAssociator::InitializeWithHeader),
            fTimeSeriesSlot("ts", this, &KTAnalyticAssociator::CreateAssociateData, &fAASignal),
            fFSFFTWSlot("fs-fftw", this, &KTAnalyticAssociator::CreateAssociateData, &fAASignal),
            fNormFSFFTWSlot("norm-fs-fftw", this, &KTAnalyticAssociator::CreateAssociateData, &fAASignal)
    {
        fReverseFFT.SetRequestedState(KTReverseFFTW::kC2C);
    }

    KTAnalyticAssociator::~KTAnalyticAssociator()
    {
    }

    bool KTAnalyticAssociator::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return false;

        SetSaveFrequencySpectrum(node->get_value< bool >("save-frequency-spectrum", fSaveFrequencySpectrum));

        if (! fForwardFFT.Configure(node->node_at("forward-fftw")))
        {
            return false;
        }
        if (! fReverseFFT.Configure(node->node_at("reverse-fftw")))
        {
            return false;
        }

        return true;
    }

    bool KTAnalyticAssociator::InitializeWithHeader(KTEggHeader& header)
    {
        if (! fForwardFFT.InitializeForRealAsComplexTDD(header.GetChannelHeader(0)->GetSliceSize()))
        {
            return false;
        }
        return fReverseFFT.InitializeWithHeader(header);
    }

    bool KTAnalyticAssociator::CheckAndDoFFTInit()
    {
        if (! fForwardFFT.GetIsInitialized())
        {
            fForwardFFT.InitializeForRealAsComplexTDD();
            if (! fForwardFFT.GetIsInitialized())
            {
                KTERROR(aalog, "Unable to initialize forward FFT.");
                return false;
            }
        }
        if (! fReverseFFT.GetIsInitialized())
        {
            fReverseFFT.InitializeForComplexTDD();
            if (! fReverseFFT.GetIsInitialized())
            {
                KTERROR(aalog, "Unable to initialize reverse FFT.");
                return false;
            }
        }
        return true;
    }

    bool KTAnalyticAssociator::CreateAssociateData(KTTimeSeriesData& tsData)
    {
        if (! CheckAndDoFFTInit())
        {
            return false;
        }

        unsigned nComponents = tsData.GetNComponents();

        KTFrequencySpectrumDataFFTW* fsData = NULL;
        if (fSaveFrequencySpectrum)
        {
            fsData = &(tsData.Of< KTFrequencySpectrumDataFFTW >().SetNComponents(nComponents));
        }

        // New data to hold the time series of the analytic associate
        KTAnalyticAssociateData& aaTSData = tsData.Of< KTAnalyticAssociateData >().SetNComponents(nComponents);

        // Calculate the analytic associates
        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            const KTTimeSeriesReal* nextInput = dynamic_cast< const KTTimeSeriesReal* >(tsData.GetTimeSeries(iComponent));
            if (nextInput == NULL)
            {
                KTERROR(aalog, "Incorrect time series type: time series did not cast to KTTimeSeriesReal. Other types of time series data are not yet supported.");
                return false;
            }

            KTFrequencySpectrumFFTW* newFS = NULL;
            KTTimeSeriesFFTW* newTS = NULL;
            if (fSaveFrequencySpectrum)
            {
                newTS = CalculateAnalyticAssociate(nextInput, &newFS);
                fsData->SetSpectrum(newFS, iComponent);
            }
            else
            {
                newTS = CalculateAnalyticAssociate(nextInput);
            }

            if (newTS == NULL)
            {
                KTERROR(aalog, "Component <" << iComponent << "> did not transform correctly.");
                return false;
            }

            aaTSData.SetTimeSeries(newTS, iComponent);
        }
        KTINFO(aalog, "Analytic associate calculation (from TS) complete for " << nComponents << " components");

        return true;
    }

    bool KTAnalyticAssociator::CreateAssociateData(KTFrequencySpectrumDataFFTW& fsData)
    {
        if (! CheckAndDoFFTInit())
        {
            return false;
        }

        unsigned nComponents = fsData.GetNComponents();

        // New data to hold the time series of the analytic associate
        KTAnalyticAssociateData& aaTSData = fsData.Of< KTAnalyticAssociateData >().SetNComponents(nComponents);

        // Calculate the analytic associates
        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            const KTFrequencySpectrumFFTW* nextInput = fsData.GetSpectrumFFTW(iComponent);

            KTTimeSeriesFFTW* newTS = CalculateAnalyticAssociate(nextInput);

            if (newTS == NULL)
            {
                KTERROR(aalog, "Component <" << iComponent << "> did not transform correctly.");
                return false;
            }

            aaTSData.SetTimeSeries(newTS, iComponent);
        }
        KTINFO(aalog, "Analytic associate calculation (from FS) complete for " << nComponents << " components");

        return true;
    }

    bool KTAnalyticAssociator::CreateAssociateData(KTNormalizedFSDataFFTW& fsData)
    {
        if (! CheckAndDoFFTInit())
        {
            return false;
        }

        unsigned nComponents = fsData.GetNComponents();

        // New data to hold the time series of the analytic associate
        KTAnalyticAssociateData& aaTSData = fsData.Of< KTAnalyticAssociateData >().SetNComponents(nComponents);

        // Calculate the analytic associates
        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            const KTFrequencySpectrumFFTW* nextInput = fsData.GetSpectrumFFTW(iComponent);

            KTTimeSeriesFFTW* newTS = CalculateAnalyticAssociate(nextInput);

            if (newTS == NULL)
            {
                KTERROR(aalog, "Component <" << iComponent << "> did not transform correctly.");
                return false;
            }

            aaTSData.SetTimeSeries(newTS, iComponent);
        }
        KTINFO(aalog, "Analytic associate calculation (from normalized FS) complete for " << nComponents << " components");

        return true;
    }


    KTTimeSeriesFFTW* KTAnalyticAssociator::CalculateAnalyticAssociate(const KTTimeSeriesReal* inputTS, KTFrequencySpectrumFFTW** outputFS)
    {
        // Forward FFT
        KTFrequencySpectrumFFTW* freqSpec = fForwardFFT.TransformAsComplex(inputTS);
        if (freqSpec == NULL)
        {
            KTERROR(aalog, "Something went wrong with the forward FFT on the time series.");
            return NULL;
        }

        // Calculate the analytic associate in frequency space
        freqSpec->AnalyticAssociate();

        // reverse FFT
        KTTimeSeriesFFTW* outputTS = fReverseFFT.TransformToComplex(freqSpec);

        // copy the address of the frequency spectrum to outputFS if it's being kept; otherwise delete
        if (fSaveFrequencySpectrum) outputFS = &freqSpec;
        else delete freqSpec;

        if (outputTS == NULL)
        {
            KTERROR(aalog, "Something went wrong with the reverse FFT on the frequency spectrum.");
        }

        return outputTS;
    }

    KTTimeSeriesFFTW* KTAnalyticAssociator::CalculateAnalyticAssociate(const KTFrequencySpectrumFFTW* inputFS)
    {
        KTFrequencySpectrumFFTW aaFS(*inputFS);

        // Calculate the analytic associate in frequency space
        aaFS.AnalyticAssociate();

        // reverse FFT
        KTTimeSeriesFFTW* outputTS = fReverseFFT.TransformToComplex(&aaFS);
        if (outputTS == NULL)
        {
            KTERROR(aalog, "Something went wrong with the reverse FFT on the frequency spectrum.");
        }

        return outputTS;
    }

} /* namespace Katydid */
