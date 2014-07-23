/*
 * KTSmooth.hh
 *
 *  Created on: Jul 23, 2014
 *      Author: nsoblath
 */

#ifndef KTSMOOTH_HH_
#define KTSMOOTH_HH_

#include "KTPhysicalArray.hh"

namespace Katydid
{
    class KTSmooth
    {
        public:
            KTSmooth();
            virtual ~KTSmooth();

            template< typename XDataType >
            static bool Smooth(KTPhysicalArray< 2, XDataType >* array)
            {
                return true;
            }
    };

} /* namespace Katydid */

#endif /* KTSMOOTH_HH_ */
