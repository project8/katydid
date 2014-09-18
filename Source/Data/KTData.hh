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
    class KTNamedData
    {
        public:
            KTNamedData() {}
            virtual ~KTNamedData() {}

            virtual const std::string& Name() const = 0;

            virtual KTNamedData* Next() const = 0;
    };

    template< class XDerivedType >
    class KTExtensibleData : public KTExtensibleStruct< XDerivedType, KTNamedData >
    {
        public:
            KTExtensibleData() {}
            virtual ~KTExtensibleData() {}

            const std::string& Name() const;

            KTNamedData* Next() const;
    };

    template< class XDerivedType >
    inline const std::string& KTExtensibleData< XDerivedType >::Name() const
    {
        return XDerivedType::sName;
    }

    template< class XDerivedType >
    inline KTNamedData* KTExtensibleData< XDerivedType >::Next() const
    {
        return KTExtensibleStructCore< KTNamedData >::fNext;
    }



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
