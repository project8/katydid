/*
 * KTProcessorTemplate.cc
 *
 *  Created on: [date]
 *      Author: [name]
 */

#include "KTRSAMatProcessor.hh"

#include "KTEggHeader.hh"
#include "KTLogger.hh"
#include "KTParam.hh"
#include "KTProcSummary.hh"

using std::string;

namespace Katydid
{
    KTLOGGER(egglog, "KTRSAMatProcessor");

    // The name of the registrar object must be unique within Katydid
    KT_REGISTER_PROCESSOR(KTRSAMatProcessor, "mat-processor");

    KTRSAMatProcessor::KTRSAMatProcessor(const std::string& name) :
            KTPrimaryProcessor(name)
            // initialize all member variables
            // initialize signals:
            // f[SomeName]Signal("[signal-name]", this)
            // initialize slots:
            // fHeaderSlot("header", this, &KTProcessorTemplate::[function to call with header]),
            // f[SomeName]Slot("[slot-name]", this, &KTProcessorTemplate::[function to call], &f[SomeName]Signal)
    {
    }

    KTRSAMatProcessor::~KTRSAMatProcessor()
    {
        // Nothing
    }

    bool KTRSAMatProcessor::Configure(const KTParamNode* node)
    {
        if (node == NULL) return false;

        // Configure parameters

        return true;
    }

    // All the normal stuff goes here

} /* namespace Katydid */
