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
        if (fCalculateMinBin) SetMinBin(fsData->GetSpectrum(0)->FindBin(fMinFrequency));
        if (fCalculateMaxBin) SetMaxBin(fsData->GetSpectrum(0)->FindBin(fMaxFrequency));

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

        newData->SetTimeInRun(fsData->GetTimeInRun());
        newData->SetSliceNumber(fsData->GetSliceNumber());

        newData->SetName(fOutputDataName);

        fFSSignal(newData);

        return newData;
    }

    KTFrequencySpectrumDataFFTW* KTGainNormalization::Normalize(const KTFrequencySpectrumDataFFTW* fsData, const KTGainVariationData* gvData)
    {
        if (fCalculateMinBin) SetMinBin(fsData->GetSpectrum(0)->FindBin(fMinFrequency));
        if (fCalculateMaxBin) SetMaxBin(fsData->GetSpectrum(0)->FindBin(fMaxFrequency));

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

        newData->SetTimeInRun(fsData->GetTimeInRun());
        newData->SetSliceNumber(fsData->GetSliceNumber());

        newData->SetName(fOutputDataName);

        return newData;
    }

    KTFrequencySpectrum* KTGainNormalization::Normalize(const KTFrequencySpectrum* frequencySpectrum, const KTSpline* spline)
    {
        UInt_t nBins = fMaxBin - fMinBin + 1;
        Double_t freqMin = frequencySpectrum->GetBinLowEdge(fMinBin);
        Double_t freqMax = frequencySpectrum->GetBinLowEdge(fMaxBin) + frequencySpectrum->GetBinWidth();

        KTSpline::Implementation* splineImp = spline->Implement(nBins, freqMin, freqMax);

        UInt_t nSpectrumBins = frequencySpectrum->size();
        Double_t freqSpectrumMin = frequencySpectrum->GetRangeMin();
        Double_t freqSpectrumMax = frequencySpectrum->GetRangeMax();

        KTFrequencySpectrum* newSpectrum = new KTFrequencySpectrum(nSpectrumBins, freqSpectrumMin, freqSpectrumMax);

        // First directly copy data that's outside the scaling range
        for (UInt_t iBin=0; iBin < fMinBin; iBin++)
        {
            (*newSpectrum)(iBin).set_polar((*frequencySpectrum)(iBin).abs(), (*frequencySpectrum)(iBin).arg());
        }
        for (UInt_t iBin=fMaxBin+1; iBin < nSpectrumBins; iBin++)
        {
            (*newSpectrum)(iBin).set_polar((*frequencySpectrum)(iBin).abs(), (*frequencySpectrum)(iBin).arg());
        }

        // Then scale the bins within the scaling range
        for (UInt_t iBin=fMinBin; iBin < fMaxBin+1; iBin++)
        {
            (*newSpectrum)(iBin).set_polar((*frequencySpectrum)(iBin).abs() / (*splineImp)(iBin), (*frequencySpectrum)(iBin).arg());
        }

        spline->AddToCache(splineImp);

        return newSpectrum;
    }

    KTFrequencySpectrumFFTW* KTGainNormalization::Normalize(const KTFrequencySpectrumFFTW* frequencySpectrum, const KTSpline* spline)
    {
        // PLEASE NOTE: There is on situation in which this normalization function will not operate properly: if the array size
        //              is even, and scaling is requested all the way up to the Nyquist bin, the Nyquist bin will not be scaled.

        UInt_t nBins = fMaxBin - fMinBin + 1;
        Double_t freqMin = frequencySpectrum->GetBinLowEdge(fMinBin);
        Double_t freqMax = frequencySpectrum->GetBinLowEdge(fMaxBin) + frequencySpectrum->GetBinWidth();

        KTSpline::Implementation* splineImp = spline->Implement(nBins, freqMin, freqMax);

        UInt_t nSpectrumBins = frequencySpectrum->size();
        Double_t freqSpectrumMin = frequencySpectrum->GetRangeMin();
        Double_t freqSpectrumMax = frequencySpectrum->GetRangeMax();

        KTFrequencySpectrumFFTW* newSpectrum = new KTFrequencySpectrumFFTW(nSpectrumBins, freqSpectrumMin, freqSpectrumMax);

        // First directly copy data that's outside the scaling range
        // DC bin
        UInt_t dcBin = frequencySpectrum->GetDCBin();
        (*newSpectrum)(dcBin)[0] = (*frequencySpectrum)(dcBin)[0];
        (*newSpectrum)(dcBin)[1] = (*frequencySpectrum)(dcBin)[1];
        // All of the other bins outside the scaling range, both positive and negative frequencies
        for (UInt_t iBinPos=dcBin + 1, iBinNeg=dcBin - 1; iBinPos < fMinBin; iBinPos++, iBinNeg--)
        {
            (*newSpectrum)(iBinPos)[0] = (*frequencySpectrum)(iBinPos)[0];
            (*newSpectrum)(iBinPos)[1] = (*frequencySpectrum)(iBinPos)[1];
            (*newSpectrum)(iBinNeg)[0] = (*frequencySpectrum)(iBinNeg)[0];
            (*newSpectrum)(iBinNeg)[1] = (*frequencySpectrum)(iBinNeg)[1];
            //KTDEBUG(gnlog, "binpos = " << iBinPos << "    binneg = " << iBinNeg);
        }
        for (UInt_t iBinPos=fMaxBin+1, iBinNeg=dcBin - (fMaxBin+1-dcBin); iBinPos < nSpectrumBins; iBinPos++, iBinNeg--)
        {
            (*newSpectrum)(iBinPos)[0] = (*frequencySpectrum)(iBinPos)[0];
            (*newSpectrum)(iBinPos)[1] = (*frequencySpectrum)(iBinPos)[1];
            (*newSpectrum)(iBinNeg)[0] = (*frequencySpectrum)(iBinNeg)[0];
            (*newSpectrum)(iBinNeg)[1] = (*frequencySpectrum)(iBinNeg)[1];
            //KTDEBUG(gnlog, "binpos = " << iBinPos << "    binneg = " << iBinNeg);
        }

        // Nyquist bin if the array size is even
        if (frequencySpectrum->GetIsSizeEven())
        {
            (*newSpectrum)(0)[0] = (*frequencySpectrum)(0)[0];
            (*newSpectrum)(0)[1] = (*frequencySpectrum)(0)[1];
            //KTDEBUG(gnlog, "binneg = 0");
        }

        // Then scale the bins within the scaling range
        Double_t scaling = 1.;
        for (UInt_t iBinPos=fMinBin, iBinNeg=dcBin - (fMinBin-dcBin), iBin=0; iBinPos < fMaxBin+1; iBinPos++, iBinNeg--, iBin++)
        {
            scaling = 1. / (*splineImp)(iBin);
            (*newSpectrum)(iBinPos)[0] = (*frequencySpectrum)(iBinPos)[0] * scaling;
            (*newSpectrum)(iBinPos)[1] = (*frequencySpectrum)(iBinPos)[1] * scaling;
            (*newSpectrum)(iBinNeg)[0] = (*frequencySpectrum)(iBinNeg)[0] * scaling;
            (*newSpectrum)(iBinNeg)[1] = (*frequencySpectrum)(iBinNeg)[1] * scaling;
            //KTDEBUG(gnlog, "binpos = " << iBinPos << "    binneg = " << iBinNeg);
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
            if (newData != NULL)
            {
                KTEvent* event = fsData->GetEvent();
                newData->SetEvent(event);
                if (event != NULL)
                    event->AddData(newData);
                fFSSignal(newData);
            }
            return;
        }

        const KTFrequencySpectrumDataFFTW* fsDataFFTW = event->GetData< KTFrequencySpectrumDataFFTW >(fFSInputDataName);
        if (fsDataFFTW != NULL)
        {
            KTFrequencySpectrumDataFFTW* newData = Normalize(fsDataFFTW, gvData);
            if (newData != NULL)
            {
                KTEvent* event = fsDataFFTW->GetEvent();
                newData->SetEvent(event);
                if (event != NULL)
                    event->AddData(newData);
                fFSFFTWSignal(newData);
            }
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
