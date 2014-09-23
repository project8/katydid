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

            MEMBERVARIABLE_PROTECTED(bool, State);
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

            // private class KTMasterCut::KTTopCut
            // purposefully not registered with the cut factory
            class KTTopCut : public KTExtensibleCut< KTTopCut >
            {
                public:
                    KTTopCut();
                    ~KTTopCut();

                    static const std::string sName;
            };

        public:
            KTMasterCut();
            ~KTMasterCut();

            const KTCutCore* Cuts() const;

            void UpdateSummary();

            template< typename XCutType >
            bool AddCut(bool state, bool doUpdateSummary=true);
            bool AddCut(const std::string& cutName, bool state, bool doUpdateSummary=true);
            template< typename XCutType >
            bool AddCut(const XCutType& cut, bool doUpdateSummary=true);

            template< typename XCutType >
            bool HasCut() const;
            bool HasCut(const std::string& cutName) const;

            template< typename XCutType >
            bool GetCutState() const;
            bool GetCutState(const std::string& cutName) const;

            template< typename XCutType >
            const KTCutCore* GetCut() const;
            const KTCutCore* GetCut(const std::string& cutName) const;

            template< typename XCutType >
            KTCutCore* GetCut();
            KTCutCore* GetCut(const std::string& cutName);

            template< typename XCutType >
            bool SetCutState(bool state, bool doUpdateSummary=true);
            bool SetCutState(const std::string& cutName, bool state, bool doUpdateSummary=true);

            template< typename XCutType >
            void RemoveCut(bool doUpdateSummary=true);
            // cannot currently update by cut name
            //void RemoveCut(const std::string& cutName, bool doUpdateSummary=true);

        private:
            boost::scoped_ptr< KTTopCut > fCuts;

            bitset_type fSummary;

        public:
            bool IsCut() const;
            bool IsCut(const bitset_type& mask) const;
            bool IsCut(unsigned long long mask) const;
            bool IsCut(const std::string& mask) const;

    };

    inline const KTCutCore* KTMasterCut::Cuts() const
    {
        return fCuts.get()->Next();
    }

    template< typename XCutType >
    bool KTMasterCut::AddCut(bool state, bool doUpdateSummary)
    {
        if (! HasCut< XCutType >())
        {
            fCuts.get()->Of< XCutType >().SetState(state);
            if (doUpdateSummary) UpdateSummary();
            return true;
        }
        return false;
    }

    template< typename XCutType >
    bool KTMasterCut::AddCut(const XCutType& cut, bool doUpdateSummary)
    {
        if (! HasCut< XCutType >())
        {
            fCuts.get()->Of< XCutType >() = cut;
            if (doUpdateSummary) UpdateSummary();
            return true;
        }
        return false;
    }

    template< typename XCutType >
    inline bool KTMasterCut::HasCut() const
    {
        return fCuts.get()->Has< XCutType >();
    }

    template< typename XCutType >
    bool KTMasterCut::GetCutState() const
    {
        if (HasCut< XCutType >())
        {
            return fCuts.get()->Of< XCutType >().GetState();
        }
        return false;
    }

    template< typename XCutType >
    const KTCutCore* KTMasterCut::GetCut() const
    {
        if (HasCut< XCutType >())
        {
            return &(fCuts.get()->Of< XCutType >());
        }
        return NULL;
    }

    template< typename XCutType >
    KTCutCore* KTMasterCut::GetCut()
    {
        if (HasCut< XCutType >())
        {
            return &(fCuts.get()->Of< XCutType >());
        }
        return NULL;
    }

    template< typename XCutType >
    inline void KTMasterCut::RemoveCut(bool doUpdateSummary)
    {
        delete fCuts.get()->Detatch< XCutType >();
        if (doUpdateSummary) UpdateSummary();
        return;
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
