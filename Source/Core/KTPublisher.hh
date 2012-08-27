/*
 * KTPublisher.hh
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#ifndef KTPUBLISHER_HH_
#define KTPUBLISHER_HH_

#include "KTConfigurable.hh"
#include "KTProcessor.hh"

#include <iostream>

namespace Katydid
{
    class KTWriteableData;
    class KTFrequencySpectrumData;

    class KTPublisher : public KTProcessor, public KTConfigurable
    {
        public:
            KTPublisher();
            virtual ~KTPublisher();

            virtual void Publish(KTWriteableData* data) = 0;

            virtual void Write(KTWriteableData* data) = 0;

            virtual void Write(KTFrequencySpectrumData* data) = 0;

    };

} /* namespace Katydid */
#endif /* KTPUBLISHER_HH_ */
