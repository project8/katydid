/*
 * KTData.hh
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#ifndef KTDATA_HH_
#define KTDATA_HH_

#include "KTExtensibleStruct.hh"

#include <boost/shared_ptr.hpp>

#include <string>

namespace Katydid
{
    struct KTDataGroup {};

    template< class XDerivedType >
    struct KTExtensibleData : KTExtensibleStruct< XDerivedType, KTDataGroup >
    {};

    struct KTData : public KTExtensibleData< KTData >
    {
            KTData();
            unsigned fCounter;
            bool fLastData;
    };

    typedef boost::shared_ptr< KTData > KTDataPtr;

} /* namespace Katydid */
#endif /* KTDATA_HH_ */
