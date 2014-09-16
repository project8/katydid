/*
 * KTData.hh
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#ifndef KTDATA_HH_
#define KTDATA_HH_

#include "KTExtensibleStruct.hh"

#include "KTMemberVariable.hh"

#include <boost/shared_ptr.hpp>

#include <string>

namespace Katydid
{
    struct KTDataGroup {};

    template< class XDerivedType >
    struct KTExtensibleData : KTExtensibleStruct< XDerivedType, KTDataGroup >
    {};

    class KTData : public KTExtensibleData< KTData >
    {
        public:
            KTData();
            ~KTData();

            MEMBERVARIABLE(unsigned, Counter);
            MEMBERVARIABLE(bool, LastData);

            static const std::string sName;
    };

    typedef boost::shared_ptr< KTData > KTDataPtr;

} /* namespace Katydid */
#endif /* KTDATA_HH_ */
