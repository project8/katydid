/*
 * KTGainNormalization.cc
 *
 *  Created on: Jan 24, 2012
 *      Author: nsoblath
 */

#include "KTGainNormalization.hh"

#include "KTFactory.hh"
#include "KTFrequencySpectrum.hh"
#include "KTFrequencySpectrumData.hh"
#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTFrequencySpectrumFFTW.hh"
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


    void KTGainNormalization::ProcessEvent(shared_ptr<KTEvent> event)
    {
        const KTGainVariationData* gvData = dynamic_cast< KTGainVariationData* >(event->GetData(fGVInputDataName));
        if (gvData == NULL)
        {
            KTWARN(gnlog, "No gain variation data named <" << fGVInputDataName << "> was available in the event");
            return;
        }

        const KTFrequencySpectrumData* fsData = dynamic_cast< KTFrequencySpectrumData* >(event->GetData(fFSInputDataName));
        if (fsData != NULL)
        {
            KTGainVariationData* newData = Normalize(fsData, gvData);
            event->AddData(newData);
            return;
        }

        /*
        const KTFrequencySpectrumDataFFTW* fsDataFFTW = dynamic_cast< KTFrequencySpectrumDataFFTW* >(event->GetData(fFSInputDataName));
        if (fsDataFFTW != NULL)
        {
            KTGainVariationData* newData = Normalize(fsDataFFTW, gvData);
            event->AddData(newData);
            return;
        }
        */
        /*
        const KTSlidingWindowFSData* swfsData = dynamic_cast< KTSlidingWindowFSData* >(event->GetData(fFSInputDataName));
        if (swfsData != NULL)
        {
            KTGainVariationData* newData = Normalize(swfsData, gvData);
            event->AddData(newData);
            return;
        }
        */
        /*
        const KTSlidingWindowFSDataFFTW* swfsDataFFTW = dynamic_cast< KTSlidingWindowFSDataFFTW* >(event->GetData(fFSInputDataName));
        if (swfsDataFFTW != NULL)
        {
            KTGainVariationData* newData = Normalize(swfsDataFFTW, gvData);
            event->AddData(newData);
            return;
        }
        */

        KTWARN(gnlog, "No time series data named <" << fFSInputDataName << "> was available in the event");
        return;
    }


} /* namespace Katydid */
