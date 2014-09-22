/*
 * KTCut.hh
 *
 *  Created on: Sept 19, 2014
 *      Author: nsoblath
 */

#ifndef KTCUT_HH_
#define KTCUT_HH_

#include "KTExtensibleStruct.hh"

#include "KTMemberVariable.hh"

#include <boost/dynamic_bitset.hpp>
#include <boost/scoped_ptr.hpp>

#include <string>

namespace Katydid
{
    class KTCutCore
    {
        public:
            KTCutCore() :
                    fState(false)
            {}
            virtual ~KTCutCore() {}

            virtual const std::string& Name() const = 0;

            virtual KTCutCore* Next() const = 0;

            MEMBERVARIABLE(bool, State);
    };

    template< class XDerivedType >
    class KTExtensibleCut : public KTExtensibleStruct< XDerivedType, KTCutCore >
    {
        public:
            KTExtensibleCut() {}
            virtual ~KTExtensibleCut() {}

            const std::string& Name() const;

            KTCutCore* Next() const;
    };

    template< class XDerivedType >
    inline const std::string& KTExtensibleCut< XDerivedType >::Name() const
    {
        return XDerivedType::sName;
    }

    template< class XDerivedType >
    inline KTCutCore* KTExtensibleCut< XDerivedType >::Next() const
    {
        return KTExtensibleStructCore< KTCutCore >::fNext;
    }


    class KTMasterCut
    {
        private:
            typedef boost::dynamic_bitset< > bitset_type;

        public:
            KTMasterCut();
            ~KTMasterCut();

            const boost::scoped_ptr< KTCutCore >& Cuts() const;
            boost::scoped_ptr< KTCutCore >& Cuts();

            void UpdateSummary();

        private:
            boost::scoped_ptr< KTCutCore > fCuts;

            bitset_type fSummary;

        public:
            bool IsCut() const;
            bool IsCut(const bitset_type& mask) const;
            bool IsCut(unsigned long long mask) const;
            bool IsCut(const std::string& mask) const;

    };

    inline const boost::scoped_ptr< KTCutCore >& KTMasterCut::Cuts() const
    {
        return fCuts;
    }

    inline boost::scoped_ptr< KTCutCore >& KTMasterCut::Cuts()
    {
        return fCuts;
    }

    inline bool KTMasterCut::IsCut() const
    {
        return fSummary.any();
    }

    inline bool KTMasterCut::IsCut(const bitset_type& mask) const
    {
        return (fSummary | mask).any();
    }

    inline bool KTMasterCut::IsCut(unsigned long long mask) const
    {
        return IsCut(bitset_type(mask));
    }

    inline bool KTMasterCut::IsCut(const std::string& mask) const
    {
        return IsCut(bitset_type(mask));
    }

} /* namespace Katydid */

#endif /* KTCUT_HH_ */
