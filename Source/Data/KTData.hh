/*
 * KTData.hh
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#ifndef KTDATA_HH_
#define KTDATA_HH_

#include "KTExtensibleStruct.hh"

#include "KTCut.hh"
#include "KTMemberVariable.hh"

#include <boost/shared_ptr.hpp>

#include <string>

namespace Katydid
{
    class KTDataCore
    {
        public:
            KTDataCore() {}
            virtual ~KTDataCore() {}

            virtual const std::string& Name() const = 0;

            virtual KTCutStatus* GetCuts() = 0;
            virtual const KTCutStatus* GetCuts() const = 0;
    };

    template< class XDerivedType >
    class KTExtensibleData : public KTExtensibleStruct< XDerivedType, KTDataCore >
    {
        public:
            KTExtensibleData() {}
            virtual ~KTExtensibleData() {}

            const std::string& Name() const;

            KTCutStatus* GetCuts();
            const KTCutStatus* GetCuts() const;
    };

    template< class XDerivedType >
    inline const std::string& KTExtensibleData< XDerivedType >::Name() const
    {
        return XDerivedType::sName;
    }

    // GetCuts() is implemented below


    class KTData : public KTExtensibleData< KTData >
    {
        public:
            KTData();
            KTData(const KTData& orig);
            ~KTData();

            MEMBERVARIABLE(unsigned, Counter);
            MEMBERVARIABLE(bool, LastData);

            MEMBERVARIABLEREF_NOSET(KTCutStatus, CutStatus);
            // additional non-const get function
            KTCutStatus& GetCutStatus();

        public:
            static const std::string sName;
    };

    inline KTCutStatus& KTData::GetCutStatus()
    {
        return fCutStatus;
    }

    typedef boost::shared_ptr< KTData > KTDataPtr;

    template< class XDerivedType >
    inline const KTCutStatus* KTExtensibleData< XDerivedType >::GetCuts() const
    {
        const KTData* data = dynamic_cast< const KTData* >(this);
        if (data != NULL) return &(data->GetCutStatus());
        return NULL;
    }

    template< class XDerivedType >
    inline KTCutStatus* KTExtensibleData< XDerivedType >::GetCuts()
    {
        KTData* data = dynamic_cast< KTData* >(this);
        if (data != NULL) return &(data->GetCutStatus());
        return NULL;
    }

} /* namespace Katydid */
#endif /* KTDATA_HH_ */
