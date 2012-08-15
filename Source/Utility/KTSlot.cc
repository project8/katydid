/*
 * KTSlot.cc
 *
 *  Created on: Feb 10, 2012
 *      Author: nsoblath
 */

#include "KTSlot.hh"

namespace Katydid
{
    SlotException::SlotException (std::string const& why)
      : std::logic_error(why)
    {}

    KTSlot::KTSlot() :
            fSlot(NULL),
            fConnection()
    {
    }

    KTSlot::~KTSlot()
    {
        this->Disconnect();
    }

} /* namespace Katydid */
