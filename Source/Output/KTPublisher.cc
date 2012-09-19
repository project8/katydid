/*
 * KTPublishingProcessor.cc
 *
 *  Created on: Sep 19, 2012
 *      Author: nsoblath
 */

#include "KTPublishingProcessor.hh"

namespace Katydid
{

    KTPublishingProcessor::KTPublishingProcessor() :
            KTConfigurable(),
            KTProcessor(),
            KTFactory< KTWriter >()
    {
        fConfigName = "writer";

    }

    KTPublishingProcessor::~KTPublishingProcessor()
    {
    }

    Bool_t KTPublishingProcessor::Configure(const KTPStoreNode* node)
    {
        return true;
    }


} /* namespace Katydid */
