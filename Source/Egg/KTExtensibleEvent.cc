/*
 * KTExtensibleEvent.cc
 *
 *  Created on: August 24, 2012
 *      Author: nsoblath
 */

#include "KTExtensibleEvent.hh"


namespace Katydid
{

    KTExtensibleEvent::KTExtensibleEvent(KTAbstractEvent* event) :
            KTDecoratedEvent(event)
    {
    }

    KTExtensibleEvent::KTExtensibleEvent() :
            KTDecoratedEvent(NULL)
    {
    }

    KTExtensibleEvent::~KTExtensibleEvent()
    {
    }



} /* namespace Katydid */
