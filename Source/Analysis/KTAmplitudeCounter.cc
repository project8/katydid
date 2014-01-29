/*
 * KTAmplitudeCounter.cc
 *
 *  Created on: Jan 24, 2014
 *      Author: laroque
 */

#include "KTAmplitudeCounter.hh"

#include "KTEggHeader.hh"
#include "KTLogger.hh"
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
            fNumberBins(256)
            // initialize signals:
//            fTSDistSignal("ts-dist", this),
            // initialize slots:
            // fHeaderSlot("header", this, &KTAmplitudeCounter::[function to call with header]),
//            fTSSlot("ts", this, &KTAmplitudeCounter::AddData, &fTSDistDataSignal)
            // f[SomeName]Slot("[slot-name]", this, &KTAmplitudeCounter::[function to call], &f[SomeName]Signal)
    {
    }

    KTAmplitudeCounter::~KTAmplitudeCounter()
    {
    }

    bool KTAmplitudeCounter::Configure(const KTPStoreNode* node)
    {
        if (node == NULL) return false;

        // Configure parameters

        return true;
    }

    // All the normal stuff goes here

} /* namespace Katydid */
