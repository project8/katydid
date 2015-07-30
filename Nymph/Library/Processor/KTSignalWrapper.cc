/*
 * KTSignalWrapper.cc
 *
 *  Created on: Aug 8, 2012
 *      Author: nsoblath
 */

#include "KTSignalWrapper.hh"

namespace Nymph
{
    SignalException::SignalException (std::string const& why)
      : std::logic_error(why)
    {}

    KTSignalWrapper::KTSignalWrapper() :
            fSignalWrapper(NULL)
    {
    }

    KTSignalWrapper::~KTSignalWrapper()
    {
        delete fSignalWrapper;
    }

} /* namespace Nymph */
