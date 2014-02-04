/*
 * KTAmplitudeCounter.cc
 *
 *  Created on: Jan 24, 2014
 *      Author: laroque
 */

#include "KTAmplitudeCounter.hh"

#include "KTEggHeader.hh"
#include "KTLogger.hh"
#include "KTTimeSeriesDist.hh"
#include "KTTimeSeriesDistData.hh"
#include "KTNOFactory.hh"
#include "KTPStoreNode.hh"


using boost::shared_ptr;

namespace Katydid
{
    KTLOGGER(avlog, "katydid.analysis");

    // The name of the registrar object must be unique within Katydid
    static KTNORegistrar< KTProcessor, KTAmplitudeCounter > sProcTempRegistrar("amp-counter");

    KTAmplitudeCounter::KTAmplitudeCounter(const std::string& name) :
            KTProcessor(name),
            // initialize all member variables
            fNumberOfBins(256),
            // initialize signals:
            fTSDistSignal("ts-dist", this),
            // initialize slots:
            // fHeaderSlot("header", this, &KTAmplitudeCounter::[function to call with header]),
            fTSSlot("raw-ts", this, &KTAmplitudeCounter::AddData, &fTSDistSignal)
            // f[SomeName]Slot("[slot-name]", this, &KTAmplitudeCounter::[function to call], &f[SomeName]Signal)
    {
    }

    KTAmplitudeCounter::~KTAmplitudeCounter()
    {
    }

    bool KTAmplitudeCounter::Configure(const KTPStoreNode* node)
    {
        // Configure parameters
        if (node != NULL)
        {
            SetNumberOfBins(node->GetData<unsigned>("num-bins", fNumberOfBins));
            return true;
        } else {
            return false;
        }
    }

    // All the normal stuff goes here

    bool KTAmplitudeCounter::AddData(KTRawTimeSeriesData& tsData)
    {
        unsigned nComponents = tsData.GetNComponents();
        KTTimeSeriesDistData& newData =tsData.Of< KTTimeSeriesDistData >().SetNComponents(nComponents);
        for (unsigned iComponent = 0; iComponent < nComponents; iComponent++)
        {
            const KTRawTimeSeries* iTS = tsData.GetTimeSeries(iComponent);
            KTTimeSeriesDist* iTSDist = newData.GetTimeSeriesDist(iComponent);
            this->CountTimeSeries(iTSDist, iTS);
        }
        return true;
    }

    bool KTAmplitudeCounter::CountTimeSeries(KTTimeSeriesDist* tsdist, const KTRawTimeSeries* ts)
    {
        unsigned nTimeBins = ts->size();
        for (unsigned iBin=0; iBin<nTimeBins; ++iBin)
        {
            unsigned val = (*ts)(iBin);
            (*tsdist)(val) += 1;
        }
        return true;
    }

} /* namespace Katydid */
