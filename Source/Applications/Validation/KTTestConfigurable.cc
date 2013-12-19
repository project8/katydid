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
    static KTCommandLineOption< int > sTestConfigStringCLO("Test Configurable", "Command-line option for testing", "int-data", 'i');

    KTLOGGER(testparamlog, "katydid.applications.validation");

    KTTestConfigurable::KTTestConfigurable(const std::string& name) :
            KTConfigurable(name),
            fIntData(-9),
            fDoubleData(-99.),
            fStringData("not configured")
    {
    }

    KTTestConfigurable::~KTTestConfigurable()
    {
    }

    bool KTTestConfigurable::Configure(const KTPStoreNode* node)
    {
        // Config-file options
        if (node != NULL)
        {
            // option: check for data before getting it from the node
            if (node->HasData("int-data"))
            {
                fIntData = node->GetData< int >("int-data", fIntData);
                KTINFO(testparamlog, "Configured integer (= existing value if not provided): " << fIntData);
            }

            // option: don't check for data before getting it from the node; rely on the default if it's not there.
            fDoubleData = node->GetData< double >("double-data", fDoubleData);
            KTINFO(testparamlog, "Configured double (= existing value if not provided): " << fDoubleData);
            fStringData = node->GetData< string >("string-data", fStringData);
            KTINFO(testparamlog, "Configured string (= existing value if not provided): " << fStringData);
        }

        // Command-line options
        fIntData = fCLHandler->GetCommandLineValue< int >("int-data", fIntData);
        KTINFO(testparamlog, "Configured integer from CL (= existing value if not provided): " << fIntData);

        return true;
    }

} /* namespace Katydid */
