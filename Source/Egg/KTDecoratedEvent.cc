/*
 * KTDecoratedEvent.cc
 *
 *  Created on: August 24, 2012
 *      Author: nsoblath
 */

#include "KTDecoratedEvent.hh"


namespace Katydid
{

    KTDecoratedEvent::KTDecoratedEvent(KTAbstractEvent* event) :
            KTAbstractEvent(),
            fEvent(event)
    {
    }

    KTDecoratedEvent::KTDecoratedEvent() :
            KTAbstractEvent(),
            fEvent(NULL)
    {
    }

    KTDecoratedEvent::~KTDecoratedEvent()
    {
    }

#ifdef ROOT_FOUND
    TH1C* KTDecoratedEvent::CreateEventHistogram(unsigned channelNum) const
    {
        return fEvent->CreateEventHistogram(channelNum);
    }

    TH1I* KTDecoratedEvent::CreateAmplitudeDistributionHistogram(unsigned channelNum) const
    {
        return CreateAmplitudeDistributionHistogram(channelNum);
    }
#endif

} /* namespace Katydid */
