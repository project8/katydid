/*
 * KTVarTypePhysicalArray.cc
 *
 *  Created on: Mar 23, 2015
 *      Author: nsoblath
 */

#include "KTVarTypePhysicalArray.hh"



namespace Katydid
{

    uint32_t ConvertMonarch3DataFormat( uint32_t m3DataFormat )
    {
        switch( m3DataFormat )
        {
            case monarch3::sDigitizedUS:
                return sDigitizedUS;
                break;
            case monarch3::sDigitizedS:
                return sDigitizedS;
                break;
            case monarch3::sAnalog:
                return sAnalog;
                break;
            default:
                return sInvalidFormat;
        }
    }



} /* namespace Katydid */
