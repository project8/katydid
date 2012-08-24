/*
 * KTFFT.hh
 *
 *  Created on: Sep 16, 2011
 *      Author: nsoblath
 */

#ifndef KTFFT_HH_
#define KTFFT_HH_

#include "Rtypes.h"

namespace Katydid
{
    class KTTimeSeriesData;

    class KTFFT
    {
        public:
            KTFFT();
            virtual ~KTFFT();

            virtual Bool_t TransformData(const KTTimeSeriesData* tsData) = 0;


            ClassDef(KTFFT, 2);
    };

} /* namespace Katydid */
#endif /* KTFFT_HH_ */
