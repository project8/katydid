/*
 * KTAnalyticAssociator.cc
 *
 *  Created on: Dec 17, 2012
 *      Author: nsoblath
 */

#include "KTAnalyticAssociator.hh"

#include "KTComplexFFTW.hh"
#include "KTEggHeader.hh"
#include "KTFactory.hh"
#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTFrequencySpectrumFFTW.hh"
#include "KTLogger.hh"
#include "KTPStoreNode.hh"
#include "KTTimeSeriesFFTW.hh"

using std::string;

using boost::shared_ptr;


namespace Katydid
{
    KTLOGGER(aalog, "katydid.analysis");

    static KTDerivedRegistrar< KTProcessor, KTAnalyticAssociator > sAARegistrar("analytic-associator");

    KTAnalyticAssociator::KTAnalyticAssociator() :
            KTProcessor(),
            fFullFFT(),
            fSaveFrequencySpectrum(false)

    {
        fConfigName = "analytic-associator";

        RegisterSignal("analytic-associate", &fAASignal, "void (const KTTimeSeriesDataFFTW*)");

        RegisterSlot("header", this, &KTAnalyticAssociator::ProcessHeader, "void (const KTEggHeader*)");
        RegisterSlot("ts-data", this, &KTAnalyticAssociator::ProcessTimeSeriesData, "void (shared_ptr<KTData>)");
        RegisterSlot("fs-data", this, &KTAnalyticAssociator::ProcessFrequencySpectrumData, "void (shared_ptr<KTData>)");
    }

    KTAnalyticAssociator::~KTAnalyticAssociator()
    {
    }

    Bool_t KTAnalyticAssociator::Configure(const KTPStoreNode* node)
    {
        if (node == NULL) return false;

        SetSaveFrequencySpectrum(node->GetData< Bool_t >("save-frequency-spectrum", fSaveFrequencySpectrum));

        const KTPStoreNode* fftNode = node->GetChild("complex-fftw");
        if (fftNode != NULL)
        {
            if (! fFullFFT.Configure(fftNode)) return false;
        }

        return true;
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

        KTFrequencySpectrumDataFFTW& fsData;
        if (fSaveFrequencySpectrum)
        {
            fsData = tsData.Of< KTFrequencySpectrumDataFFTW >().SetNComponents(nComponents);
        }

        // New data to hold the time series of the analytic associate
        KTBasicTimeSeriesData& aaTSData = tsData.Of< KTAnalyticAssociateData >().SetNComponents(nComponents);

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
                fsData.SetSpectrum(newFS, iComponent);
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

        fAASignal(aaTSData);

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
        KTBasicTimeSeriesData& aaTSData = fsData.Of< KTAnalyticAssociateData >().SetNComponents(nComponents);

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

        fAASignal(aaTSData);

        return aaTSData;
    }


    KTTimeSeriesFFTW* KTAnalyticAssociator::CalculateAnalyticAssociate(const KTTimeSeriesFFTW* inputTS, KTFrequencySpectrumFFTW** outputFS)
    {
        // Forward FFT
        KTFrequencySpectrumFFTW* freqSpec = fFullFFT.TransformForward(inputTS);
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
        KTTimeSeriesFFTW* outputTS = fFullFFT.TransformReverse(freqSpec);
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
        KTTimeSeriesFFTW* outputTS = fFullFFT.TransformReverse(&aaFS);
        if (outputTS == NULL)
        {
            KTERROR(aalog, "Something went wrong with the reverse FFT on the frequency spectrum.");
        }

        return outputTS;
    }

    void KTAnalyticAssociator::ProcessHeader(const KTEggHeader* header)
    {
        fFullFFT.ProcessHeader(header);
        return;
    }

    void KTAnalyticAssociator::ProcessTimeSeriesData(shared_ptr<KTData> data)
    {
        if (! data->Has< KTTimeSeriesData >())
        {
            KTERROR(aalog, "No time series data was present");
            return;
        }
        if (! CreateAssociateData(data->Of< KTTimeSeriesData >()))
        {
            KTERROR(fftlog_comp, "Something went wrong while calculating the analytic associate from a time series");
            return;
        }
        fAASignal(data);
        return;
    }

    void KTAnalyticAssociator::ProcessFrequencySpectrumData(shared_ptr<KTData> data)
    {
        if (! data->Has< KTFrequencySpectrumDataFFTW >())
        {
            KTERROR(aalog, "No time series data was present");
            return;
        }
        if (! CreateAssociateData(data->Of< KTFrequencySpectrumDataFFTW >()))
        {
            KTERROR(fftlog_comp, "Something went wrong while calculating the analytic associate from a frequency spectrum");
            return;
        }
        fAASignal(data);
        return;
    }

} /* namespace Katydid */
