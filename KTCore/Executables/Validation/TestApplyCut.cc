/*
 * TestApplyCut.cc
 *
 *  Created on: Oct 07, 2014
 *      Author: nsoblath
 */

#include "KTApplyCut.hh"
#include "KTCut.hh"
#include "KTLogger.hh"
#include "KTMemberVariable.hh"
#include "KTParam.hh"

namespace Katydid
{
    KTLOGGER(testlog, "TestApplyCut");

    class KTTestData : public KTExtensibleData< KTTestData >
    {
        public:
            KTTestData() :
                KTExtensibleData< KTTestData >(),
                fIsAwesome(false)
            {}
            virtual ~KTTestData() {}

            MEMBERVARIABLE(bool, IsAwesome);

        public:
            static const std::string sName;

    };

    // Cuts data that is NOT awesome
    class KTAwesomeCut : public KTCut
    {
        public:
            struct Result : KTExtensibleCutResult< Result >
            {
                static const std::string sName;
            };

        public:
            KTAwesomeCut(const std::string& name = "default-awesome-cut") :
                KTCut(name)
            {}
            ~KTAwesomeCut() {}

            bool Configure(const KTParamNode* node)
            {return true;}

            bool Apply(KTData& data, KTTestData& testData)
            {
                bool isCut = ! testData.GetIsAwesome();
                KTDEBUG(testlog, "Is data awesome? " << testData.GetIsAwesome());
                KTDEBUG(testlog, "Is data cut? " << isCut);
                data.GetCutStatus().AddCutResult< KTAwesomeCut::Result >(isCut);
                return isCut;
            }

            bool Apply(KTDataPtr dataPtr)
            {
                if (! dataPtr->Has< KTTestData >())
                {
                    KTERROR(testlog, "Data type <KTTestData> was not present");
                    return false;
                }
                return Apply(dataPtr->Of< KTData >(), dataPtr->Of< KTTestData >());
            }
    };

    // Cuts data that is IS awesome
    class KTNotAwesomeCut : public KTCut
    {
        public:
            struct Result : KTExtensibleCutResult< Result >
            {
                static const std::string sName;
            };

        public:
            KTNotAwesomeCut(const std::string& name = "default-not-awesome-cut") :
                KTCut(name)
            {}
            ~KTNotAwesomeCut() {}

            bool Configure(const KTParamNode* node)
            {return true;}

            bool Apply(KTData& data, KTTestData& testData)
            {
                bool isCut = testData.GetIsAwesome();
                KTDEBUG(testlog, "Is data awesome? " << testData.GetIsAwesome());
                KTDEBUG(testlog, "Is data cut? " << isCut);
                // use the name-based AddCutResult
                data.GetCutStatus().AddCutResult("not-awesome-cut", isCut);
                return isCut;
            }

            bool Apply(KTDataPtr dataPtr)
            {
                if (! dataPtr->Has< KTTestData >())
                {
                    KTERROR(testlog, "Data type <KTTestData> was not present");
                    return false;
                }
                return Apply(dataPtr->Of< KTData >(), dataPtr->Of< KTTestData >());
            }
    };


    const std::string KTTestData::sName = "test-data";

    const std::string KTAwesomeCut::Result::sName = "awesome-cut";
    const std::string KTNotAwesomeCut::Result::sName = "not-awesome-cut";

    KT_REGISTER_CUT(KTAwesomeCut);
    KT_REGISTER_CUT(KTNotAwesomeCut);
}


using namespace Katydid;
using namespace std;

int main()
{
    KTDataPtr dataPtr(new KTData());
    KTTestData& testData = dataPtr->Of< KTTestData >();

    KTCutStatus& cutStatus = dataPtr->GetCutStatus();
    KTINFO(testlog, "Initial cut state: " << cutStatus.IsCut());

    KTApplyCut applyCut;

    KTINFO(testlog, "Applying awesome cut");
    applyCut.SetCut(new KTAwesomeCut());
    applyCut.ApplyCut(dataPtr);

    KTINFO(testlog, "Has cut result \"awesome-cut\"? " << cutStatus.HasCutResult("awesome-cut"));
    KTINFO(testlog, "Has cut result <KTAwesomeCut::Result>? " << cutStatus.HasCutResult< KTAwesomeCut::Result >());
    KTINFO(testlog, "Cut state of \"awesome-cut\" is: " << cutStatus.GetCutState("awesome-cut"));
    KTINFO(testlog, "Cut state of <KTAwesomeCut::Result> is: " << cutStatus.GetCutState< KTAwesomeCut::Result >());
    KTINFO(testlog, "Is cut (all results)? " << cutStatus.IsCut());
    KTINFO(testlog, "Is cut (with mask \"0\")? " << cutStatus.IsCut("0"));

    KTINFO(testlog, "Applying not-awesome cut");
    applyCut.SelectCut("not-awesome-cut");
    applyCut.ApplyCut(dataPtr);

    KTINFO(testlog, "Has cut result \"awesome-cut\"? " << cutStatus.HasCutResult("awesome-cut"));
    KTINFO(testlog, "Has cut result <KTAwesomeCut::Result>? " << cutStatus.HasCutResult< KTAwesomeCut::Result >());
    KTINFO(testlog, "Cut state of \"awesome-cut\" is: " << cutStatus.GetCutState("awesome-cut"));
    KTINFO(testlog, "Cut state of <KTAwesomeCut::Result> is: " << cutStatus.GetCutState< KTAwesomeCut::Result >());
    KTINFO(testlog, "Has cut result \"not-awesome-cut\"? " << cutStatus.HasCutResult("not-awesome-cut"));
    KTINFO(testlog, "Has cut result <KTNotAwesomeCut::Result>? " << cutStatus.HasCutResult< KTNotAwesomeCut::Result >());
    KTINFO(testlog, "Cut state of \"not-awesome-cut\" is: " << cutStatus.GetCutState("not-awesome-cut"));
    KTINFO(testlog, "Cut state of <KTNotAwesomeCut::Result> is: " << cutStatus.GetCutState< KTNotAwesomeCut::Result >());
    KTINFO(testlog, "Is cut (all results)? " << cutStatus.IsCut());
    KTINFO(testlog, "Is cut with mask \"00\"? " << cutStatus.IsCut("00"));
    KTINFO(testlog, "Is cut with mask \"01\"? " << cutStatus.IsCut("01"));
    KTINFO(testlog, "Is cut with mask \"10\"? " << cutStatus.IsCut("10"));
    KTINFO(testlog, "Is cut with mask \"11\"? " << cutStatus.IsCut("11"));
    KTINFO(testlog, "Is cut with mask 0? " << cutStatus.IsCut(0));
    KTINFO(testlog, "Is cut with mask 1? " << cutStatus.IsCut(1));
    KTINFO(testlog, "Is cut with mask 2? " << cutStatus.IsCut(2));
    KTINFO(testlog, "Is cut with mask 3? " << cutStatus.IsCut(3));

    return 0;
}
