/*
 * KTWriter.hh
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#ifndef KTWRITER_HH_
#define KTWRITER_HH_

#include "KTProcessor.hh"

#include <iostream>

namespace Katydid
{
    class KTWriteableData;
    class KTFrequencySpectrumData;
    class KTFrequencySpectrumDataFFTW;
    class KTCorrelationData;

    class KTWriter : public KTProcessor
    {
        public:
            KTWriter();
            virtual ~KTWriter();

            virtual void Publish(const KTWriteableData* data) = 0;

            virtual void Write(const KTWriteableData* data) = 0;

            virtual void Write(const KTFrequencySpectrumData* data) = 0;
            virtual void Write(const KTFrequencySpectrumDataFFTW* data) = 0;

            virtual void Write(const KTCorrelationData* data) = 0;

    };

} /* namespace Katydid */
#endif /* KTWRITER_HH_ */
