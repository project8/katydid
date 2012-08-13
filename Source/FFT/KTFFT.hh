/*
 * KTFFT.hh
 *
 *  Created on: Sep 16, 2011
 *      Author: nsoblath
 */

#ifndef KTFFT_HH_
#define KTFFT_HH_

#include "Rtypes.h"

#include <vector>
using std::vector;

//class TArray;

namespace Katydid
{
    class KTEvent;

    class KTFFT
    {
        public:
            KTFFT();
            virtual ~KTFFT();

            virtual Bool_t TakeData(const KTEvent* event) = 0;
            virtual Bool_t TakeData(const vector< Double_t >& data) = 0;
            //virtual Bool_t TakeData(const TArray* data) = 0;

            virtual Bool_t Transform() = 0;

            ClassDef(KTFFT, 2);
    };

} /* namespace Katydid */
#endif /* KTFFT_HH_ */
