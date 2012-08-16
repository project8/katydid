/*
 * KTSlot.cc
 *
 *  Created on: Feb 10, 2012
 *      Author: nsoblath
 */

#include "KTSlot.hh"

#include "KTSignal.hh"

namespace Katydid
{
    SlotException::SlotException (std::string const& why)
      : std::logic_error(why)
    {}

    KTSlotWrapper::KTSlotWrapper() :
            fSlotWrapper(NULL),
            fConnection()
    {
    }

    KTSlotWrapper::~KTSlotWrapper()
    {
        this->Disconnect();
    }

} /* namespace Katydid */
