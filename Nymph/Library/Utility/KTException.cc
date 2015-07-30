/*
 * KTException.cc
 *
 *  Created on: Feb 25, 2014
 *      Author: nsoblath
 */

#include "KTException.hh"

namespace Nymph
{

    KTException::KTException() :
            std::exception(),
            fException( "" )
    {
    }
    KTException::KTException( const KTException& an_exception ) :
            std::exception(),
            fException( an_exception.fException.str() )
    {
    }

    KTException::~KTException() throw ()
    {
    }

    const char* KTException::what() const throw ()
    {
        return fException.str().c_str();
    }

}
