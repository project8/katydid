/*
 * KTProcessorTemplate.cc
 *
 *  Created on: [date]
 *      Author: [name]
 */

#include "KTProcessorTemplate.hh"

#include "KTEggHeader.hh"
#include "KTLogger.hh"
#include "KTNOFactory.hh"
#include "KTPStoreNode.hh"

using boost::shared_ptr;

namespace Katydid
{
    KTLOGGER([logger], "katydid.fft");

    // The name of the registrar object must be unique within Katydid
    static KTDerivedNORegistrar< KTProcessor, KTProcessorTemplate > sProcTempRegistrar("[config-name]");

    KTComplexFFTW::KTComplexFFTW(const std::string& name) :
            KTProcessor(name),
            // initialize all member variables
            // initialize signals:
            // f[SomeName]Signal("[signal-name]", this)
            // initialize slots:
            // fHeaderSlot("header", this, &KTComplexFFTW::[function to call with header]),
            // f[SomeName]Slot("[slot-name]", this, &KTProcessorTemplate::[function to call], &f[SomeName]Signal)
    {
        SetupInternalMaps();
    }

    KTComplexFFTW::~KTComplexFFTW()
    {
    }

    Bool_t KTComplexFFTW::Configure(const KTPStoreNode* node)
    {
        if (node == NULL) return false;

        // Configure parameters

        return true;
    }

    // All the normal stuff goes here

} /* namespace Katydid */