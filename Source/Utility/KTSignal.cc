/*
 * KTSignal.cc
 *
 *  Created on: Aug 8, 2012
 *      Author: nsoblath
 */

#include "KTSignal.hh"

namespace Katydid
{

    KTSignal::KTSignal() :
            fSignal(NULL)
    {
    }

    KTSignal::~KTSignal()
    {
        delete fSignal;
    }



} /* namespace Katydid */
