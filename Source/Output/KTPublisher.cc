/*
 * KTPublisher.cc
 *
 *  Created on: Sep 19, 2012
 *      Author: nsoblath
 */

#include "KTPublisher.hh"

namespace Katydid
{

    KTPublisher::KTPublisher() :
            KTConfigurable(),
            KTProcessor(),
            KTFactory< KTWriter >()
    {
        fConfigName = "writer";

    }

    KTPublisher::~KTPublisher()
    {
    }

    Bool_t KTPublisher::Configure(const KTPStoreNode* node)
    {
        return true;
    }


} /* namespace Katydid */
