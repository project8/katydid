/*
 * KTConfigurable.cc
 *
 *  Created on: Aug 21, 2012
 *      Author: nsoblath
 */

#include "KTConfigurable.hh"

#include "KTCommandLineHandler.hh"

namespace Katydid
{

    KTConfigurable::KTConfigurable() :
            fCLHandler(KTCommandLineHandler::GetInstance()),
            fConfigName("default_name")
    {
    }

    KTConfigurable::~KTConfigurable()
    {
    }

} /* namespace Katydid */
