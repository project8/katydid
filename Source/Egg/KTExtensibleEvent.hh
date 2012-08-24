/**
 @file KTExtensibleEvent.hh
 @brief Contains KTExtensibleEvent
 @details This extension of KTEvent allows you to attach other types of data (i.e. from a particular analysis)
 The added data objects are stored in an unordered map.
 @author: N. S. Oblath
 @date: August 24, 2012
 */

#ifndef KTEXTENSIBLEEVENT_HH_
#define KTEXTENSIBLEEVENT_HH_

#include "KTDecoratedEvent.hh"

//#include <boost/unordered_map.hpp>

namespace Katydid
{
    class KTExtensibleEvent : public KTDecoratedEvent
    {
        public:
            KTExtensibleEvent(KTAbstractEvent* event);
            virtual ~KTExtensibleEvent();

        private:
            KTExtensibleEvent();



    };



} /* namespace Katydid */

#endif /* KTEXTENSIBLEEVENT_HH_ */
