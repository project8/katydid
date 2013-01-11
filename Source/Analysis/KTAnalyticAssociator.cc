/*
 * KTAnalyticAssociator.cc
 *
 *  Created on: Dec 17, 2012
 *      Author: nsoblath
 */

#include "KTAnalyticAssociator.hh"

#include "KTCacheDirectory.hh"
#include "KTComplexFFTW.hh"
#include "KTEggHeader.hh"
#include "KTEvent.hh"
#include "KTFactory.hh"
#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTFrequencySpectrumFFTW.hh"
#include "KTLogger.hh"
#include "KTPStoreNode.hh"
#include "KTTimeSeriesChannelData.hh"
#include "KTTimeSeriesData.hh"
#include "KTTimeSeriesFFTW.hh"

using std::string;

using boost::shared_ptr;


namespace Katydid
{
    KTLOGGER(aalog, "katydid.analysis");

    static KTDerivedRegistrar< KTProcessor, KTAnalyticAssociator > sWVRegistrar("analytic-associator");

    KTAnalyticAssociator::KTAnalyticAssociator() :
            KTProcessor(),
            fFullFFT(),
            fInputDataName("time-series"),
            fOutputDataName("wigner-ville"),
            fSaveFrequencySpectrum(false),
            fFSOutputDataName("frequency-spectrum-from-aa")

    {
        fConfigName = "analytic-associator";

        RegisterSignal("analytic-associate", &fAASignal, "void (const KTTimeSeriesDataFFTW*)");

        RegisterSlot("header", this, &KTAnalyticAssociator::ProcessHeader, "void (const KTEggHeader*)");
        RegisterSlot("ts-data", this, &KTAnalyticAssociator::ProcessTimeSeriesData, "void (const KTTimeSeriesData*)");
        RegisterSlot("event", this, &KTAnalyticAssociator::ProcessEvent, "void (shared_ptr<KTEvent>)");
    }

    KTAnalyticAssociator::~KTAnalyticAssociator()
    {
    }

    Bool_t KTAnalyticAssociator::Configure(const KTPStoreNode* node)
    {
        SetInputDataName(node->GetData< string >("input-data-name", fInputDataName));
        SetOutputDataName(node->GetData< string >("output-data-name", fOutputDataName));

        SetSaveFrequencySpectrum(node->GetData< Bool_t >("save-frequency-spectrum", fSaveFrequencySpectrum));
        SetFSOutputDataName(node->GetData< string >("aa-fs-output-data-name", fFSOutputDataName));

        const KTPStoreNode* fftNode = node->GetChild("complex-fftw");
        if (fftNode != NULL)
        {
            if (! fFullFFT.Configure(fftNode)) return false;
        }

        return true;
    }

    KTTimeSeriesData* KTAnalyticAssociator::CreateAssociateData(const KTTimeSeriesData* data, KTFrequencySpectrumDataFFTW** outputFSData)
    {
        if (! fFullFFT.GetIsInitialized())
        {
            fFullFFT.InitializeFFT();
            if (! fFullFFT.GetIsInitialized())
            {
                KTERROR(aalog, "Unable to initialize full FFT.");
                return NULL;
            }
        }

        if (fSaveFrequencySpectrum && outputFSData == NULL)
        {
            KTWARN(aalog, "The flag for saving the frequency spectrum is set, but no KTFrequencySpectrumDataFFTW** was provided;\n"
                    << "\tThe frequency spectrum will not be saved."
                    << "\tfSaveFrequencySpectrum is being set to false");
            fSaveFrequencySpectrum = false;
        }

        KTFrequencySpectrumDataFFTW* fsData = NULL;
        if (fSaveFrequencySpectrum)
        {
            fsData = new KTFrequencySpectrumDataFFTW(data->GetNTimeSeries());
            fsData->SetName(fFSOutputDataName);
            (*outputFSData) = fsData;
        }

        // New data to hold the time series of the analytic associate
        KTBasicTimeSeriesData* aaTSData = new KTBasicTimeSeriesData(data->GetNTimeSeries());

        // Calculate the analytic associates
        for (UInt_t iChannel = 0; iChannel < data->GetNTimeSeries(); iChannel++)
        {
            const KTTimeSeriesFFTW* nextInput = dynamic_cast< const KTTimeSeriesFFTW* >(data->GetTimeSeries(iChannel));
            if (nextInput == NULL)
            {
                KTERROR(aalog, "Incorrect time series type: time series did not cast to KTTimeSeriesFFTW. Other types of time series data are not yet supported.");
                delete aaTSData;
                delete fsData;
                return NULL;
            }

            KTFrequencySpectrumFFTW* newFS = NULL;
            KTTimeSeriesFFTW* newTS = NULL;
            if (fSaveFrequencySpectrum)
            {
                newTS = CalculateAnalyticAssociate(nextInput, &newFS);
                fsData->SetSpectrum(newFS, iChannel);
            }
            else
            {
                newTS = CalculateAnalyticAssociate(nextInput);
            }

            if (newTS == NULL)
            {
                KTERROR(aalog, "Channel <" << iChannel << "> did not transform correctly.");
                delete aaTSData;
                delete fsData;
                return NULL;
            }

            aaTSData->SetTimeSeries(newTS, iChannel);
        }

        aaTSData->SetEvent(data->GetEvent());
        aaTSData->SetName(fOutputDataName);

        fAASignal(aaTSData);

        return aaTSData;
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
            return NULL;
        }

        return outputTS;
    }

    void KTAnalyticAssociator::ProcessHeader(const KTEggHeader* header)
    {
        fFullFFT.ProcessHeader(header);
        return;
    }

    void KTAnalyticAssociator::ProcessTimeSeriesData(const KTTimeSeriesData* tsData)
    {
        //Pass these pointers in case the user wants to save these data.
        KTFrequencySpectrumDataFFTW* saveFreqSpec = NULL;

        KTTimeSeriesData* newData = CreateAssociateData(tsData, &saveFreqSpec);

        if (newData == NULL)
        {
            KTERROR(aalog, "Unable to transform data");
            return;
        }

        KTEvent* event = tsData->GetEvent();
        if (event != NULL)
        {
            event->AddData(newData);

            if (fSaveFrequencySpectrum) event->AddData(saveFreqSpec);
            else delete saveFreqSpec;
        }

        return;
    }

    void KTAnalyticAssociator::ProcessEvent(shared_ptr<KTEvent> event)
    {
        const KTTimeSeriesData* tsData = event->GetData< KTTimeSeriesData >(fInputDataName);
        if (tsData == NULL)
        {
            KTWARN(aalog, "No time series data named <" << fInputDataName << "> was available in the event");
            return;
        }

        ProcessTimeSeriesData(tsData);
        return;
    }

} /* namespace Katydid */
