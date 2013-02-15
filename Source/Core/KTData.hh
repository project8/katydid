/*
 * KTData.hh
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#ifndef KTDATA_HH_
#define KTDATA_HH_

#include "KTExtensibleStruct.hh"

#include "Rtypes.h"

#include <string>

namespace Katydid
{
    struct KTDataGroup {};

    template< class XDerivedType >
    struct KTExtensibleData : KTExtensibleStruct< XDerivedType, KTDataGroup >
    {};

    struct KTData : KTExtensibleData< KTData >
    {
        UInt_t fCounter;
        Bool_t fLastData;
        KTData() :
            fCounter(0),
            fLastData(false)
        {};
    };

} /* namespace Katydid */
#endif /* KTDATA_HH_ */
