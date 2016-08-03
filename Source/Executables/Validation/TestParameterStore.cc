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
#include "param.hh"
#include "KTTestConfigurable.hh"

#include <string>

using namespace Katydid;
using namespace std;

KTLOGGER(testparamlog, "TestParameterStore");

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        KTERROR(testparamlog, "You did not provide a configuration file as the first command-line argument.");
        return -1;
    }
    string configFilename(argv[1]);

    KTParameterStore* store = KTParameterStore::GetInstance();
    if (! store->ReadConfigFile(configFilename))
    {
        KTERROR(testparamlog, "Unable to read config file");
        return -1;
    }

    // Testing a configurable
    KTINFO(testparamlog, "Testing a configurable");

    KTTestConfigurable* testObj = new KTTestConfigurable();

    KTPStoreNode configNode = store->GetNode(testObj->GetConfigName());
    if (! configNode.IsValid())
    {
        KTERROR(testparamlog, "Top-level node <" << testObj->GetConfigName() << "> was not found");
        return -1;
    }

    if (testObj->Configure(&configNode))
    {
        KTINFO(testparamlog, "Configuration complete");
    }
    else
    {
        KTERROR(testparamlog, "Something went wrong during the configuration");
    }

    delete testObj;


    // Testing nested data
    KTINFO(testparamlog, "Testing nested data");

    KTPStoreNode topNode = store->GetNode("nested-data");
    if (! topNode.IsValid())
    {
        KTERROR(testparamlog, "Top-level node <nested-data> was not found");
        return -1;
    }

    KTINFO(testparamlog, "There are " << topNode.CountNodes("single-data") << " instances of <single-data> in <nested-data>");
    KTINFO(testparamlog, "There are " << topNode.CountNodes("multi-data") << " instances of <multi-data> in <nested-data>");
    KTINFO(testparamlog, "topNode->HasData(\"single-data\") = " << topNode.HasData("single-data"));
    KTINFO(testparamlog, "topNode->HasData(\"multi-data\") = " << topNode.HasData("multi-data"));

    KTPStoreNode::const_sorted_iterator iter = topNode.Find("single-data");
    if (iter != topNode.NotFound())
    {
        KTINFO(testparamlog, "Single data from <nested-data> via an iterator: " << iter->second.get_value< string >());
    }

    const KTPStoreNode subNode = topNode.GetChild("sub-data");
    if (! subNode.IsValid())
    {
        KTERROR(testparamlog, "Sub-level node <sub-data> was not found in <nested-data>");

        return -1;
    }

    KTINFO(testparamlog, "There are " << subNode.CountNodes("single-data") << " instances of <single-data> in <sub-data>");
    KTINFO(testparamlog, "There are " << subNode.CountNodes("multi-data") << " instances of <multi-data> in <sub-data>");
    KTINFO(testparamlog, "subNode->HasData(\"single-data\") = " << subNode.HasData("single-data"));
    KTINFO(testparamlog, "subNode->HasData(\"multi-data\") = " << subNode.HasData("multi-data"));

    iter = subNode.Find("single-data");
    if (iter != topNode.NotFound())
    {
        KTINFO(testparamlog, "Single data from <sub-data> via an iterator: " << iter->second.get_value< string >());
    }

    KTPStoreNode::csi_pair itPair = subNode.EqualRange("multi-data");
    for (KTPStoreNode::const_sorted_iterator citer = itPair.first; citer != itPair.second; citer++)
    {
        KTINFO(testparamlog, "Multi data from <sub-data> via an iterator: " << citer->second.get_value< string >());
    }


    return 0;
}
