/*
 * KTSlotWrapper.cc
 *
 *  Created on: Feb 10, 2012
 *      Author: nsoblath
 */

#include "KTSlotWrapper.hh"

namespace Nymph
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
        delete fSlotWrapper;
    }

} /* namespace Nymph */
