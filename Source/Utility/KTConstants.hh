/*
 * KTConstants.hh
 *
 *  Created on: Mar 26, 2015
 *      Author: nsoblath
 */

#ifndef KTCONSTANTS_HH_
#define KTCONSTANTS_HH_

#include <inttypes.h>
#include <limits>


namespace Katydid
{
    //*************************
    // Data formats
    //*************************
    // Data format types (values matched to Monarch3's data format types)
    static const uint32_t sInvalidFormat = std::numeric_limits< uint32_t >::max();
    static const uint32_t sDigitizedUS = 0;
    static const uint32_t sDigitizedS = 1;
    static const uint32_t sAnalog = 2;

    /// Specifies whether the meaningful bits for each sample are aligned to the left or right in each sample word
    /// e.g. For 14-bit digitizer bit depth, represented by a 16-bit sample word, are the 14 bits aligned to the left or right in the 16-bit word?
    static const uint32_t sBitsAlignedLeft = 0;
    static const uint32_t sBitsAlignedRight = 1;


} /* namespace Katydid */

#endif /* KTCONSTANTS_HH_ */
