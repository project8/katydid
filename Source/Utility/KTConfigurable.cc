/*
 * KTConfigurable.cc
 *
 *  Created on: Jul 24, 2012
 *      Author: nsoblath
 */

#include "KTConfigurable.hh"

#include "KTParameterStore.hh"

namespace Katydid
{

    KTConfigurable::KTConfigurable() :
            fParameterStore(KTParameterStore::GetInstance())
    {
    }

    KTConfigurable::~KTConfigurable()
    {
    }

} /* namespace Katydid */
