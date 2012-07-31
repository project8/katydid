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

//#include <boost/property_tree/ptree.hpp>
//#include <boost/property_tree/json_parser.hpp>

#include <iostream>
#include <string>

using namespace Katydid;
using namespace std;

KTLOGGER(testparamlog, "katydid.applications.validation");

int main(int argc, char** argv)
{
    string configFilename(argv[1]);
/*
    using boost::property_tree::ptree;
    ptree pt;

    try
    {
        read_json(configFilename, pt);
    }
    catch (boost::property_tree::json_parser::json_parser_error& e)
    {
        cout << "Error reading config file: " << e.what() << endl;
        return -1;
    }

    return 0;
*/

    KTParameterStore* store = KTParameterStore::GetInstance();
    store->ReadConfigFile(configFilename);

    KTPStoreNode* topNode = store->GetNode("TestConfigurable");
    if (topNode == NULL)
    {
        KTERROR(testparamlog, "Top-level node was not found");
        return -1;
    }

    KTTestConfigurable* testObj = new KTTestConfigurable();
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
