/*
 * TestPrintData.cc
 *
 *  Created on: Oct 07, 2014
 *      Author: nsoblath
 */

#include "KTCut.hh"
#include "KTLogger.hh"
#include "KTMemberVariable.hh"
#include "KTParam.hh"
#include "KTPrintDataStructure.hh"

namespace Nymph
{
    KTLOGGER(testlog, "TestPrintData");

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


using namespace Nymph;
using namespace std;

int main()
{
    KTDataPtr dataPtr(new KTData());
    KTData& data = dataPtr->Of< KTData >();
    KTTestData& testData = dataPtr->Of< KTTestData >();

    KTINFO(testlog, "Applying awesome cut");
    KTAwesomeCut cut;
    cut.Apply(data, testData);

    KTINFO(testlog, "Applying not-awesome cut");
    KTNotAwesomeCut naCut;
    naCut.Apply(data, testData);

    KTPrintDataStructure printer;

    KTINFO(testlog, "Printing data structure");
    printer.PrintDataStructure(dataPtr);

    KTINFO(testlog, "Printing cut structure");
    printer.PrintCutStructure(dataPtr);

    return 0;
}
