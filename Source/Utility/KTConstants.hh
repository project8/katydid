/*
 * KTConstants.hh
 *
 *  Created on: Mar 26, 2015
 *      Author: nsoblath
 */

#ifndef KTCONSTANTS_HH_
#define KTCONSTANTS_HH_

#include <inttypes.h>


namespace Katydid
{
    //*************************
    // Data formats
    //*************************
    // Data format types (values matched to Monarch3's data format types)
    static const uint32_t sInvalidFormat = UINT32_MAX;
    static const uint32_t sDigitizedUS = 0;
    static const uint32_t sDigitizedS = 1;
    static const uint32_t sAnalog = 2;

} /* namespace Katydid */

#endif /* KTCONSTANTS_HH_ */