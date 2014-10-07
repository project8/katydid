/*
 * KTCutStatus.hh
 *
 *  Created on: Sept 19, 2014
 *      Author: nsoblath
 */

#ifndef KTCUTSTATUS_HH_
#define KTCUTSTATUS_HH_


#include "KTCutResult.hh"

#include <boost/dynamic_bitset.hpp>
#include <boost/scoped_ptr.hpp>

#include <string>

namespace Katydid
{
    /*!
     @class KTCutStatus
     @author N. S. Oblath

     @brief Provides easy access to cut information.

     @details
     KTCutStatus is typically used as a member variable of KTData, the top-level data object.

     KTCutStatus owns the set of cut results that have been added to a data object.
     It also owns a summary of those cuts (implemented with boost::dynamic_bitset).

     You can check if the data has been cut with the IsCut functions.
     - IsCut() returns true if any cut results are true;
     - IsCut(const bitset_type& mask), IsCut(unsigned int mask), and IsCut(const std::string& mask) allow you to specify
       a cut mask, and return true if any of the cut results specified by the mask are true.

     When specifying a cut mask, bits set to true specify cuts that should be used:
     - bitset_type is boost::dynamic_bitset;
     - unsigned integer masks use the bits of the integer;
     - std::string masks are strings with each character either a 0 or 1.

     With KTCutStatus you can interact with individual cut results in the following ways:
     - Add cut results to a data object with AddCutResult,
     - Check to see if a particular cut result is present using HasCutResult,
     - Get the value of a cut result with GetCutState,
     - Set the value of a cut result with SetCutState,
     - Directly access the cut result with GetCutResult, and
     - Remove a cut result with RemoveCutResult.

     For all except KTCutStatus::RemoveCutResult, the cut result can be identified by type or string name.
     */

    class KTCutStatus
    {
        private:
            typedef boost::dynamic_bitset< > bitset_type;

            // private class KTCutStatus::KTCutResultHandle
            // purposefully not registered with the cut factory
            class KTCutResultHandle : public KTExtensibleCutResult< KTCutResultHandle >
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
            friend std::ostream& operator<<(std::ostream& out, const KTCutStatus& status);

            boost::scoped_ptr< KTCutResultHandle > fCutResults;

            bitset_type fSummary;

        public:
            bool IsCut() const;
            bool IsCut(const bitset_type& mask) const;
            bool IsCut(unsigned long long mask) const;
            bool IsCut(const std::string& mask) const;

    };

    std::ostream& operator<<(std::ostream& out, const KTCutStatus& status);


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

#endif /* KTCUTSTATUS_HH_ */
