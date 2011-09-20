/*
 * KTFFT.hh
 *
 *  Created on: Sep 16, 2011
 *      Author: nsoblath
 */

#ifndef KTFFT_HH_
#define KTFFT_HH_

#include "Rtypes.h"

class TArray;

namespace Katydid
{
    class KTEvent;

    class KTFFT
    {
        public:
            KTFFT();
            virtual ~KTFFT();

            virtual void TakeData(const KTEvent* event) = 0;
            virtual void TakeData(const TArray* data) = 0;

            virtual void Transform() = 0;

    };

} /* namespace Katydid */
#endif /* KTFFT_HH_ */
