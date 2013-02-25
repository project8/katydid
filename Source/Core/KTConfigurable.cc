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

    KTConfigurable::KTConfigurable(const std::string& name) :
            fCLHandler(KTCommandLineHandler::GetInstance()),
            fConfigName(name)
    {
    }

    KTConfigurable::~KTConfigurable()
    {
    }

} /* namespace Katydid */
