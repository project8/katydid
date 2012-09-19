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
    class KTCorrelationData;

    class KTPublisher : public KTProcessor, public KTConfigurable
    {
        public:
            KTPublisher();
            virtual ~KTPublisher();

            virtual void Publish(const KTWriteableData* data) = 0;

            virtual void Write(const KTWriteableData* data) = 0;

            virtual void Write(const KTFrequencySpectrumData* data) = 0;

            virtual void Write(const KTCorrelationData* data) = 0;

    };

} /* namespace Katydid */
#endif /* KTPUBLISHER_HH_ */
