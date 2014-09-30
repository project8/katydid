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
    /*  // THIS IS JUST AN EXAMPLE; ALSO SEE TestCut.cc
    class KTSomeData;
    class KTExampleCut
    {
        public:
            struct Result : KTCutResult
            {
                static const std::string sName;
            };

        public:
            KTExampleCut();
            ~KTExampleCut();

            bool Apply(KTSomeData& data);
    };
    */

    class KTCutResultCore
    {
        public:
            KTCutResultCore() :
                    fState(false)
            {}
            virtual ~KTCutResultCore() {}

            virtual const std::string& Name() const = 0;

            MEMBERVARIABLE_PROTECTED(bool, State);
    };

    typedef KTExtensibleStructCore< KTCutResultCore > KTCutResult;

    template< class XDerivedType >
    class KTExtensibleCut : public KTExtensibleStruct< XDerivedType, KTCutResultCore >
    {
        public:
            KTExtensibleCut() {}
            virtual ~KTExtensibleCut() {}

            const std::string& Name() const;
    };

    template< class XDerivedType >
    inline const std::string& KTExtensibleCut< XDerivedType >::Name() const
    {
        return XDerivedType::sName;
    }


    class KTCutStatus
    {
        private:
            typedef boost::dynamic_bitset< > bitset_type;

            // private class KTCutStatus::KTCutResultHandle
            // purposefully not registered with the cut factory
            class KTCutResultHandle : public KTExtensibleCut< KTCutResultHandle >
            {
                public:
                    KTCutResultHandle();
                    ~KTCutResultHandle();

                    static const std::string sName;
            };

        public:
            KTCutStatus();
            KTCutStatus(const KTCutStatus& orig);
            ~KTCutStatus();

            KTCutStatus& operator=(const KTCutStatus& rhs);

            const KTCutResult* CutResults() const;

            void UpdateStatus();

            template< typename XCutType >
            bool AddCutResult(bool state, bool doUpdateStatus=true);
            bool AddCutResult(const std::string& cutName, bool state, bool doUpdateStatus=true);
            // overload for const char* to avoid specializing the templated function below
            bool AddCutResult(const char* cutName, bool state, bool doUpdateStatus=true);
            template< typename XCutType >
            bool AddCutResult(const XCutType& cut, bool doUpdateStatus=true);

            template< typename XCutType >
            bool HasCutResult() const;
            bool HasCutResult(const std::string& cutName) const;

            template< typename XCutType >
            bool GetCutState() const;
            bool GetCutState(const std::string& cutName) const;

            template< typename XCutType >
            const KTCutResult* GetCutResult() const;
            const KTCutResult* GetCutResult(const std::string& cutName) const;

            template< typename XCutType >
            KTCutResult* GetCutResult();
            KTCutResult* GetCutResult(const std::string& cutName);

            template< typename XCutType >
            bool SetCutState(bool state, bool doUpdateStatus=true);
            bool SetCutState(const std::string& cutName, bool state, bool doUpdateStatus=true);

            template< typename XCutType >
            void RemoveCutResult(bool doUpdateStatus=true);
            // cannot currently update by cut name
            //void RemoveCutResult(const std::string& cutName, bool doUpdateStatus=true);

        private:
            boost::scoped_ptr< KTCutResultHandle > fCutResults;

            bitset_type fSummary;

        public:
            bool IsCut() const;
            bool IsCut(const bitset_type& mask) const;
            bool IsCut(unsigned long long mask) const;
            bool IsCut(const std::string& mask) const;

    };

    inline const KTCutResult* KTCutStatus::CutResults() const
    {
        return fCutResults.get()->Next();
    }

    template< typename XCutType >
    bool KTCutStatus::AddCutResult(bool state, bool doUpdateStatus)
    {
        if (! HasCutResult< XCutType >())
        {
            fCutResults.get()->Of< XCutType >().SetState(state);
            if (doUpdateStatus) UpdateStatus();
            return true;
        }
        return false;
    }

    inline bool KTCutStatus::AddCutResult(const char* cutName, bool state, bool doUpdateStatus)
    {
        return AddCutResult(std::string(cutName), state, doUpdateStatus);
    }

    template< typename XCutType >
    bool KTCutStatus::AddCutResult(const XCutType& cut, bool doUpdateStatus)
    {
        if (! HasCutResult< XCutType >())
        {
            fCutResults.get()->Of< XCutType >() = cut;
            if (doUpdateStatus) UpdateStatus();
            return true;
        }
        return false;
    }

    template< typename XCutType >
    inline bool KTCutStatus::HasCutResult() const
    {
        return fCutResults.get()->Has< XCutType >();
    }

    template< typename XCutType >
    bool KTCutStatus::GetCutState() const
    {
        if (HasCutResult< XCutType >())
        {
            return fCutResults.get()->Of< XCutType >().GetState();
        }
        return false;
    }

    template< typename XCutType >
    const KTCutResult* KTCutStatus::GetCutResult() const
    {
        if (HasCutResult< XCutType >())
        {
            return &(fCutResults.get()->Of< XCutType >());
        }
        return NULL;
    }

    template< typename XCutType >
    KTCutResult* KTCutStatus::GetCutResult()
    {
        if (HasCutResult< XCutType >())
        {
            return &(fCutResults.get()->Of< XCutType >());
        }
        return NULL;
    }

    template< typename XCutType >
    inline void KTCutStatus::RemoveCutResult(bool doUpdateStatus)
    {
        delete fCutResults.get()->Detatch< XCutType >();
        if (doUpdateStatus) UpdateStatus();
        return;
    }


    inline bool KTCutStatus::IsCut() const
    {
        return fSummary.any();
    }

    inline bool KTCutStatus::IsCut(const bitset_type& mask) const
    {
        return (fSummary & mask).any();
    }

    inline bool KTCutStatus::IsCut(unsigned long long mask) const
    {
        return IsCut(bitset_type(fSummary.size(), mask));
    }

    inline bool KTCutStatus::IsCut(const std::string& mask) const
    {
        return IsCut(bitset_type(mask));
    }

} /* namespace Katydid */

#endif /* KTCUT_HH_ */
