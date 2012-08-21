/*
 * TestTestParameterStore.cc
 *
 *  Created on: Jul 2, 2012
 *      Author: nsoblath
 *
 *  Command-line arguments:
 *    1. Config filename (use TestParameterStoreConfig.json
 */

#include "KTLogger.hh"
#include "KTParameterStore.hh"
#include "KTPStoreNode.hh"
#include "KTTestConfigurable.hh"

#include <string>

using namespace Katydid;
using namespace std;

KTLOGGER(testparamlog, "katydid.applications.validation");

int main(int argc, char** argv)
{
    string configFilename(argv[1]);

    KTParameterStore* store = KTParameterStore::GetInstance();
    store->ReadConfigFile(configFilename);

    KTTestConfigurable* testObj = new KTTestConfigurable();

    KTPStoreNode* topNode = store->GetNode(testObj->GetConfigName());
    if (topNode == NULL)
    {
        KTERROR(testparamlog, "Top-level node <" << testObj->GetConfigName() << "> was not found");
        return -1;
    }

    if (testObj->Configure(topNode))
    {
        KTINFO(testparamlog, "Configuration complete");
    }
    else
    {
        KTERROR(testparamlog, "Something went wrong during the configuration");
    }

    delete testObj;

    return 0;
}
