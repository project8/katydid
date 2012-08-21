/*
 * KTTestConfigurable.cc
 *
 *  Created on: Jul 25, 2012
 *      Author: nsoblath
 */

#include "KTLogger.hh"
#include "KTPStoreNode.hh"
#include "KTTestConfigurable.hh"
#include "KTCommandLineOption.hh"

using std::string;

namespace Katydid
{
    static KTCommandLineOption< Int_t > sTestConfigStringCLO("Test Configurable", "Command-line option for testing", "int-data", 'i');

    KTLOGGER(testparamlog, "katydid.applications.validation");

    KTTestConfigurable::KTTestConfigurable() :
            fIntData(-9),
            fDoubleData(-99.),
            fStringData("not configured")
    {
        fConfigName = "test_configurable";
    }

    KTTestConfigurable::~KTTestConfigurable()
    {
    }

    Bool_t KTTestConfigurable::ConfigureFromPStore(const KTPStoreNode* node)
    {
        if (node->HasData("int_data"))
        {
            fIntData = node->GetData< Int_t >("int_data", -1);
            KTINFO(testparamlog, "Configured integer: " << fIntData);
        }
        if (node->HasData("double_data"))
        {
            fDoubleData = node->GetData< Double_t >("double_data", -11.);
            KTINFO(testparamlog, "Configured double: " << fDoubleData);
        }
        if (node->HasData("string_data"))
        {
            fStringData = node->GetData<string>("string_data", "no value");
            KTINFO(testparamlog, "Configured string: " << fStringData);
        }
        return true;
    }

    Bool_t KTTestConfigurable::ConfigureFromCL()
    {
        if (fCLHandler->IsCommandLineOptSet("int-data"))
        {
            fIntData = fCLHandler->GetCommandLineValue< Int_t >("int-data");
            KTINFO(testparamlog, "Configured integer from CL: " << fIntData);
        }
        return true;
    }


} /* namespace Katydid */
