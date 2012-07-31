/*
 * KTTestConfigurable.cc
 *
 *  Created on: Jul 25, 2012
 *      Author: nsoblath
 */

#include "KTLogger.hh"
#include "KTTestConfigurable.hh"

using std::string;

namespace Katydid
{

    KTLOGGER(testparamlog, "katydid.applications.validation");

    KTTestConfigurable::KTTestConfigurable() :
            fIntData(-9),
            fDoubleData(-99.),
            fStringData("not configured")
    {
    }

    KTTestConfigurable::~KTTestConfigurable()
    {
    }

    Bool_t KTTestConfigurable::Configure(const KTPStoreNode* node)
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

} /* namespace Katydid */
