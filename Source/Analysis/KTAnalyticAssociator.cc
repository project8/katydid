/*
 * KTAnalyticAssociator.cc
 *
 *  Created on: Dec 17, 2012
 *      Author: nsoblath
 */

#include "KTAnalyticAssociator.hh"

#include "KTAnalyticAssociateData.hh"
#include "KTComplexFFTW.hh"
#include "KTEggHeader.hh"
#include "KTNOFactory.hh"
#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTFrequencySpectrumFFTW.hh"
#include "KTLogger.hh"
#include "KTNormalizedFSData.hh"
#include "KTPStoreNode.hh"
#include "KTTimeSeriesFFTW.hh"

using std::string;

using boost::shared_ptr;


namespace Katydid
{
    KTLOGGER(aalog, "katydid.analysis");

    static KTDerivedNORegistrar< KTProcessor, KTAnalyticAssociator > sAARegistrar("analytic-associator");

    KTAnalyticAssociator::KTAnalyticAssociator(const std::string& name) :
            KTProcessor(name),
            fFullFFT(),
            fSaveFrequencySpectrum(false),
            fAASignal("aa", this),
            fHeaderSlot("header", this, &KTAnalyticAssociator::InitializeWithHeader),
            fTimeSeriesSlot("ts", this, &KTAnalyticAssociator::CreateAssociateData, &fAASignal),
            fFSFFTWSlot("fs-fftw", this, &KTAnalyticAssociator::CreateAssociateData, &fAASignal),
            fNormFSFFTWSlot("norm-fs-fftw", this, &KTAnalyticAssociator::CreateAssociateData, &fAASignal)
    {
    }

    KTAnalyticAssociator::~KTAnalyticAssociator()
    {
    }

    Bool_t KTAnalyticAssociator::Configure(const KTPStoreNode* node)
    {
        if (node == NULL) return false;

        SetSaveFrequencySpectrum(node->GetData< Bool_t >("save-frequency-spectrum", fSaveFrequencySpectrum));

        const KTPStoreNode fftNode = node->GetChild("complex-fftw");
        if (fftNode.IsValid())
        {
            if (! fFullFFT.Configure(&fftNode)) return false;
        }

        return true;
    }

    void KTAnalyticAssociator::InitializeWithHeader(const KTEggHeader* header)
    {
        fFullFFT.InitializeWithHeader(header);
        return;
    }

    Bool_t KTAnalyticAssociator::CreateAssociateData(KTTimeSeriesData& tsData)
    {
        if (! fFullFFT.GetIsInitialized())
        {
            fFullFFT.InitializeFFT();
            if (! fFullFFT.GetIsInitialized())
            {
                KTERROR(aalog, "Unable to initialize full FFT.");
                return false;
            }
        }

        UInt_t nComponents = tsData.GetNComponents();

        KTFrequencySpectrumDataFFTW* fsData = NULL;
        if (fSaveFrequencySpectrum)
        {
            fsData = &(tsData.Of< KTFrequencySpectrumDataFFTW >().SetNComponents(nComponents));
        }

        // New data to hold the time series of the analytic associate
        KTAnalyticAssociateData& aaTSData = tsData.Of< KTAnalyticAssociateData >().SetNComponents(nComponents);

        // Calculate the analytic associates
        for (UInt_t iComponent = 0; iComponent < nComponents; iComponent++)
        {
            const KTTimeSeriesFFTW* nextInput = dynamic_cast< const KTTimeSeriesFFTW* >(tsData.GetTimeSeries(iComponent));
            if (nextInput == NULL)
            {
                KTERROR(aalog, "Incorrect time series type: time series did not cast to KTTimeSeriesFFTW. Other types of time series data are not yet supported.");
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

    Bool_t KTAnalyticAssociator::CreateAssociateData(KTFrequencySpectrumDataFFTW& fsData)
    {
        if (! fFullFFT.GetIsInitialized())
        {
            fFullFFT.InitializeFFT();
            if (! fFullFFT.GetIsInitialized())
            {
                KTERROR(aalog, "Unable to initialize full FFT.");
                return false;
            }
        }

        UInt_t nComponents = fsData.GetNComponents();

        // New data to hold the time series of the analytic associate
        KTAnalyticAssociateData& aaTSData = fsData.Of< KTAnalyticAssociateData >().SetNComponents(nComponents);

        // Calculate the analytic associates
        for (UInt_t iComponent = 0; iComponent < nComponents; iComponent++)
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

    Bool_t KTAnalyticAssociator::CreateAssociateData(KTNormalizedFSDataFFTW& fsData)
    {
        if (! fFullFFT.GetIsInitialized())
        {
            fFullFFT.InitializeFFT();
            if (! fFullFFT.GetIsInitialized())
            {
                KTERROR(aalog, "Unable to initialize full FFT.");
                return false;
            }
        }

        UInt_t nComponents = fsData.GetNComponents();

        // New data to hold the time series of the analytic associate
        KTAnalyticAssociateData& aaTSData = fsData.Of< KTAnalyticAssociateData >().SetNComponents(nComponents);

        // Calculate the analytic associates
        for (UInt_t iComponent = 0; iComponent < nComponents; iComponent++)
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


    KTTimeSeriesFFTW* KTAnalyticAssociator::CalculateAnalyticAssociate(const KTTimeSeriesFFTW* inputTS, KTFrequencySpectrumFFTW** outputFS)
    {
        // Forward FFT
        KTFrequencySpectrumFFTW* freqSpec = fFullFFT.Transform(inputTS);
        if (freqSpec == NULL)
        {
            KTERROR(aalog, "Something went wrong with the forward FFT on the time series.");
            return NULL;
        }
        // copy the address of the frequency spectrum to outputFS
        outputFS = &freqSpec;

        // Calculate the analytic associate in frequency space
        freqSpec->AnalyticAssociate();

        // reverse FFT
        KTTimeSeriesFFTW* outputTS = fFullFFT.Transform(freqSpec);
        if (outputTS == NULL)
        {
            KTERROR(aalog, "Something went wrong with the reverse FFT on the frequency spectrum.");
            if (outputFS == NULL) delete freqSpec;
        }

        return outputTS;
    }

    KTTimeSeriesFFTW* KTAnalyticAssociator::CalculateAnalyticAssociate(const KTFrequencySpectrumFFTW* inputFS)
    {
        KTFrequencySpectrumFFTW aaFS(*inputFS);

        // Calculate the analytic associate in frequency space
        aaFS.AnalyticAssociate();

        // reverse FFT
        KTTimeSeriesFFTW* outputTS = fFullFFT.Transform(&aaFS);
        if (outputTS == NULL)
        {
            KTERROR(aalog, "Something went wrong with the reverse FFT on the frequency spectrum.");
        }

        return outputTS;
    }

} /* namespace Katydid */
