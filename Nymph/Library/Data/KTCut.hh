/*
 * KTCut.hh
 *
 *  Created on: Sept 19, 2014
 *      Author: nsoblath
 */

#ifndef KTCUT_HH_
#define KTCUT_HH_

#include "KTConfigurable.hh"
#include "KTCutResult.hh"
#include "KTData.hh"
#include "KTExtensibleStructFactory.hh"
#include "KTNOFactory.hh"
#include "KTMemberVariable.hh"

namespace Nymph
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
             return false;
         }
         return Apply(dataPtr->Of< KTData >(), dataPtr->Of< KTSomeData >());
     }

    */

    class KTCut : public KTConfigurable
    {
        public:
            KTCut(const std::string& name = "default-cut-name");
            virtual ~KTCut();

            virtual bool Apply(KTDataPtr) = 0;
    };


    // this macro enforces the existence of cut_class::Result and cut_class::Result::sName at compile time
#define KT_REGISTER_CUT(cut_class) \
        static KTNORegistrar< KTCut, cut_class > sCut##cut_class##Registrar(cut_class::Result::sName); \
        static KTExtensibleStructRegistrar< KTCutResultCore, cut_class::Result > sCut##cut_class##ResultRegistrar(cut_class::Result::sName);

} /* namespace Nymph */

#endif /* KTCUT_HH_ */
