/*
 * KTCut.hh
 *
 *  Created on: Sept 19, 2014
 *      Author: nsoblath
 */

#ifndef KTCUT_HH_
#define KTCUT_HH_

#include "KTExtensibleStruct.hh"

#include "KTConfigurable.hh"
#include "KTExtensibleStructFactory.hh"
#include "KTNOFactory.hh"
#include "KTMemberVariable.hh"

#include <boost/dynamic_bitset.hpp>
#include <boost/scoped_ptr.hpp>

#include <string>

namespace Katydid
{
    /*!
     @class KTCut
     @author N. S. Oblath

     @brief Base class for applying a cut to data.

     @details
     A fully implemented cut MUST have the following:
     - Public nested class called Result, inheriting from KTCutResult, and containing a public static std::string name sName.
     - Cut registration using the macro KT_REGISTER_CUT([class name])
     - Implementation of bool Configure(const KTParamNode*)
     - Implementation of void Apply(KTDataPtr)

     The existence of [class name]::Result and [class name]::Result::sName are enforces at compile time by the KT_REGISTER_CUT macro.

     The functions bool Configure(const KTParamNode*) and void Apply(KTDataPtr) are abstract in KTCut, and therefore must be implemented.

     --------------------------------------
     ------- Example Cut Definition -------
     --------------------------------------

     class KTSomeData;
     class KTExampleCut : public KTCut
     {
         public:
             struct Result : KTCutResult
             {
                 static const std::string sName;
             };

         public:
             KTExampleCut(const std::string& name = "default-example-cut");
             ~KTExampleCut();

             bool Configure(const KTParamNode* node);

             MEMBERVARIABLE(double, AwesomenessThreshold);

         public:
             bool Apply(KTData& data, KTSomeData& data);

             void Apply(KTDataPtr dataPtr);
     };

     --------------------------------------
     ------- Example Implementation -------
     --------------------------------------

     const std::string KTExampleCut::Result::sName = "example-cut";

     KT_REGISTER_CUT(KTExampleCut, KTExampleCut::Result::sName);

     KTExampleCut::KTExampleCut(const std::string& name) :
             KTCut(name),
             fAwesomenessThreshold(1000000.)
     {}

     KTExampleCut::~KTExampleCut()
     {}

     bool KTExampleCut::Configure(const KTParamNode* node)
     {
         if (node == NULL) return true;
         SetAwesomenessThreshold(node->GetValue("awesomeness", GetAwesomenessThreshold()));
         return true;
     }

     bool KTExampleCut::Apply(KTData& data, KTSomeData& someData)
     {
         bool isCut = someData.Awesomeness() > fAwesomenessThreshold;
         data.GetCutStatus().AddCutResult< KTExampleCut::Result >(isCut);
         return isCut;
     }

     void Apply(KTDataPtr dataPtr)
     {
         if (! dataPtr->Has< KTSomeData >())
         {
             KTERROR(exlog, "Data type <KTSomeData> was not present");
             return;
         }
         Apply(dataPtr->Of< KTData >(), dataPtr->Of< KTSomeData >());
         return;
     }

    */

    class KTCut : public KTConfigurable
    {
        public:
            KTCut(const std::string& name = "default-cut-name");
            virtual ~KTCut();

            virtual void Apply(KTDataPtr) = 0;
    };

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
    class KTExtensibleCutResult : public KTExtensibleStruct< XDerivedType, KTCutResultCore >
    {
        public:
            KTExtensibleCutResult() {}
            virtual ~KTExtensibleCutResult() {}

            const std::string& Name() const;
    };

    template< class XDerivedType >
    inline const std::string& KTExtensibleCutResult< XDerivedType >::Name() const
    {
        return XDerivedType::sName;
    }


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

    // this macro enforces the existence of cut_class::Result and cut_class::Result::sName at compile time
#define KT_REGISTER_CUT(cut_class) \
        static KTNORegistrar< KTCut, cut_class > sCut##cut_class##Registrar(cut_class::Result::sName); \
        static KTExtensibleStructRegistrar< KTCutResultCore, cut_class::Result > sCut##cut_class##ResultRegistrar(cut_class::Result::sName);

} /* namespace Katydid */

#endif /* KTCUT_HH_ */
