/*
 * KTGainNormalization.cc
 *
 *  Created on: Jan 24, 2012
 *      Author: nsoblath
 */

#include "KTGainNormalization.hh"

#include "KTEvent.hh"
#include "KTFactory.hh"
#include "KTFrequencySpectrum.hh"
#include "KTFrequencySpectrumData.hh"
#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTFrequencySpectrumFFTW.hh"
#include "KTGainVariationData.hh"
#include "KTLogger.hh"
#include "KTPhysicalArray.hh"
#include "KTPStoreNode.hh"
#include "KTSlidingWindowFSData.hh"
#include "KTSlidingWindowFSDataFFTW.hh"

using std::string;
using boost::shared_ptr;

namespace Katydid
{
    KTLOGGER(gnlog, "katydid.analysis");

    static KTDerivedRegistrar< KTProcessor, KTGainNormalization > sGainNormRegistrar("gain-normalization");

    KTGainNormalization::KTGainNormalization() :
            KTProcessor(),
            fMinFrequency(0.),
            fMaxFrequency(1.),
            fMinBin(0),
            fMaxBin(1),
            fCalculateMinBin(true),
            fCalculateMaxBin(true),
            fGVInputDataName("gain-variation"),
            fFSInputDataName("frequency-spectrum"),
            fOutputDataName("gain-variation")
    {
        fConfigName = "gain-normalization";

        RegisterSignal("gain-norm-fs", &fFSSignal, "void (const KTFrequencySpectrumData*)");
        RegisterSignal("gain-norm-fs-fftw", &fFSFFTWSignal, "void (const KTFrequencySpectrumDataFFTW*)");
        RegisterSignal("gain-norm-sw-fs", &fSWFSSignal, "void (const KTSlidingWindowFSData*)");
        RegisterSignal("gain-norm-sw-fs-fftw", &fSWFSFFTWSignal, "void (const KTSlidingWindowFSDataFFTW*)");

        RegisterSlot("event", this, &KTGainNormalization::ProcessEvent, "void (shared_ptr<KTEvent>)");
    }

    KTGainNormalization::~KTGainNormalization()
    {
    }

    Bool_t KTGainNormalization::Configure(const KTPStoreNode* node)
    {
        if (node == NULL) return false;

        if (node->HasData("min-frequency"))
        {
            SetMinFrequency(node->GetData< Double_t >("min-frequency"));
        }
        if (node->HasData("max-frequency"))
        {
            SetMaxFrequency(node->GetData< Double_t >("max-frequency"));
        }

        if (node->HasData("min-bin"))
        {
            SetMinBin(node->GetData< UInt_t >("min-bin"));
        }
        if (node->HasData("max-bin"))
        {
            SetMaxBin(node->GetData< UInt_t >("max-bin"));
        }

        SetGVInputDataName(node->GetData< string >("gv-input-data-name", fGVInputDataName));
        SetFSInputDataName(node->GetData< string >("fs-input-data-name", fFSInputDataName));
        SetOutputDataName(node->GetData< string >("output-data-name", fOutputDataName));

        return true;
    }


    KTFrequencySpectrumData* KTGainNormalization::Normalize(const KTFrequencySpectrumData* fsData, const KTGainVariationData* gvData)
    {
        UInt_t nChannels = fsData->GetNChannels();
        if (nChannels != gvData->GetNChannels())
        {
            KTERROR(gnlog, "Mismatch in the number of channels between the frequency spectrum data and the gain variation data! Aborting.");
            return NULL;
        }

        KTFrequencySpectrumData* newData = new KTFrequencySpectrumData(nChannels);

        for (UInt_t iChannel=0; iChannel<nChannels; iChannel++)
        {
            KTFrequencySpectrum* newSpectrum = Normalize(fsData->GetSpectrum(iChannel), gvData->GetSpline(iChannel));
            if (newSpectrum == NULL)
            {
                KTERROR(gnlog, "Normalization of spectrum " << iChannel << " failed for some reason. Continuing processing.");
                continue;
            }
            newData->SetSpectrum(newSpectrum, iChannel);
        }

        newData->SetName(fOutputDataName);
        newData->SetEvent(fsData->GetEvent());

        fFSSignal(newData);

        return newData;
    }

    KTFrequencySpectrumDataFFTW* KTGainNormalization::Normalize(const KTFrequencySpectrumDataFFTW* fsData, const KTGainVariationData* gvData)
    {
        UInt_t nChannels = fsData->GetNChannels();
        if (nChannels != gvData->GetNChannels())
        {
            KTERROR(gnlog, "Mismatch in the number of channels between the frequency spectrum data and the gain variation data! Aborting.");
            return NULL;
        }

        KTFrequencySpectrumDataFFTW* newData = new KTFrequencySpectrumDataFFTW(nChannels);

        for (UInt_t iChannel=0; iChannel<nChannels; iChannel++)
        {
            KTFrequencySpectrumFFTW* newSpectrum = Normalize(fsData->GetSpectrum(iChannel), gvData->GetSpline(iChannel));
            if (newSpectrum == NULL)
            {
                KTERROR(gnlog, "Normalization of spectrum " << iChannel << " failed for some reason. Continuing processing.");
                continue;
            }
            newData->SetSpectrum(newSpectrum, iChannel);
        }

        newData->SetName(fOutputDataName);
        newData->SetEvent(fsData->GetEvent());

        fFSFFTWSignal(newData);

        return newData;
    }

    KTFrequencySpectrum* KTGainNormalization::Normalize(const KTFrequencySpectrum* frequencySpectrum, const KTSpline* spline)
    {
        UInt_t nBins = frequencySpectrum->size();
        Double_t freqMin = frequencySpectrum->GetRangeMin();
        Double_t freqMax = frequencySpectrum->GetRangeMax();

        KTSpline::Implementation* splineImp = spline->Implement(nBins, freqMin, freqMax);

        KTFrequencySpectrum* newSpectrum = new KTFrequencySpectrum(nBins, freqMin, freqMax);
        for (UInt_t iBin=0; iBin < nBins; iBin++)
        {
            (*newSpectrum)(iBin).set_polar((*frequencySpectrum)(iBin).abs() / (*splineImp)(iBin), (*frequencySpectrum)(iBin).arg());
        }

        spline->AddToCache(splineImp);

        return newSpectrum;
    }

    KTFrequencySpectrumFFTW* KTGainNormalization::Normalize(const KTFrequencySpectrumFFTW* frequencySpectrum, const KTSpline* spline)
    {
        UInt_t nBins = frequencySpectrum->size();
        Double_t freqMin = frequencySpectrum->GetRangeMin();
        Double_t freqMax = frequencySpectrum->GetRangeMax();

        KTSpline::Implementation* splineImp = spline->Implement(nBins, freqMin, freqMax);

        KTFrequencySpectrumFFTW* newSpectrum = new KTFrequencySpectrumFFTW(nBins, freqMin, freqMax);
        Double_t scaling = 1.;
        for (UInt_t iBin=0; iBin < nBins; iBin++)
        {
            scaling = 1. / (*splineImp)(iBin);
            (*newSpectrum)(iBin)[0] = (*frequencySpectrum)(iBin)[0] * scaling;
            (*newSpectrum)(iBin)[1] = (*frequencySpectrum)(iBin)[1] * scaling;
        }

        spline->AddToCache(splineImp);

        return newSpectrum;
    }

    void KTGainNormalization::ProcessEvent(shared_ptr<KTEvent> event)
    {
        const KTGainVariationData* gvData = event->GetData< KTGainVariationData >(fGVInputDataName);
        if (gvData == NULL)
        {
            KTWARN(gnlog, "No gain variation data named <" << fGVInputDataName << "> was available in the event");
            return;
        }

        const KTFrequencySpectrumData* fsData = event->GetData< KTFrequencySpectrumData >(fFSInputDataName);
        if (fsData != NULL)
        {
            KTFrequencySpectrumData* newData = Normalize(fsData, gvData);
            event->AddData(newData);
            return;
        }

        const KTFrequencySpectrumDataFFTW* fsDataFFTW = event->GetData< KTFrequencySpectrumDataFFTW >(fFSInputDataName);
        if (fsDataFFTW != NULL)
        {
            KTFrequencySpectrumDataFFTW* newData = Normalize(fsDataFFTW, gvData);
            event->AddData(newData);
            return;
        }

        /*
        const KTSlidingWindowFSData* swfsData = dynamic_cast< KTSlidingWindowFSData* >(event->GetData(fFSInputDataName));
        if (swfsData != NULL)
        {
            KTSlidingWindowFSData* newData = Normalize(swfsData, gvData);
            event->AddData(newData);
            return;
        }
        */
        /*
        const KTSlidingWindowFSDataFFTW* swfsDataFFTW = dynamic_cast< KTSlidingWindowFSDataFFTW* >(event->GetData(fFSInputDataName));
        if (swfsDataFFTW != NULL)
        {
            KTSlidingWindowFSDataFFTW* newData = Normalize(swfsDataFFTW, gvData);
            event->AddData(newData);
            return;
        }
        */

        KTWARN(gnlog, "No time series data named <" << fFSInputDataName << "> was available in the event");
        return;
    }


} /* namespace Katydid */
