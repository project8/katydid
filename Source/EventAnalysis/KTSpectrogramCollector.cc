/*
 * KTSpectrogramCollector.cc
 *
 *  Created on: Oct 9, 2015
 *      Author: nsoblath
 */

#include "KTSpectrogramCollector.hh"

#include "KTLogger.hh"
#include "KTNOFactory.hh"
#include "KTParam.hh"


namespace Nymph
{
    KTLOGGER(evlog, "KTSpectrogramCollector");

    // Register the processor
    KT_REGISTER_PROCESSOR(KTSpectrogramCollector, "spectrogram-collector");

    KTSpectrogramCollector::KTSpectrogramCollector(const std::string& name) :
            KTProcessor(name)
            // initialize all member variables
            // initialize signals:
            // f[SomeName]Signal("[signal-name]", this)
            // initialize slots:
            // fHeaderSlot("header", this, &KTSpectrogramCollector::[function to call with header]),
            // f[SomeName]Slot("[slot-name]", this, &KTSpectrogramCollector::[function to call], &f[SomeName]Signal)
    {
    }

    KTSpectrogramCollector::~KTSpectrogramCollector()
    {
    }

    bool KTSpectrogramCollector::Configure(const KTParamNode* node)
    {
        if (node == NULL) return false;

        // Configure parameters

        return true;
    }
