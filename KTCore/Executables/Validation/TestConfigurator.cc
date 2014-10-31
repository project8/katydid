/*
 * TestConfigurator.cc
 *
 *  Created on: Feb 27, 2014
 *      Author: nsoblath
 *
 *  Command-line arguments:
 *    1. Config filename (use TestConfigurator.json)
 */

#include "KTConfigurator.hh"
#include "KTLogger.hh"
#include "KTParamInputJSON.hh"
#include "KTTestConfigurable.hh"

#include <string>

using namespace Katydid;
using namespace std;

KTLOGGER(conflog, "TestConfigurator");

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        KTERROR(conflog, "Please provide a configuration file as the first command-line argument.");
        return -1;
    }

    KTConfigurator* configurator = KTConfigurator::GetInstance();

    string configFilename(argv[1]);
    KTParamNode* t_config_from_file = KTParamInputJSON::ReadFile( configFilename );
    if( t_config_from_file == NULL )
    {
        KTERROR(conflog, "Unable to parse the config file <" << configFilename << ">");
        return -2;
    }

    KTINFO(conflog, "As parsed from file <" << configFilename << ">:\n" << *t_config_from_file);

    configurator->Merge( *t_config_from_file );
    delete t_config_from_file;

    const KTParamNode* topNode = configurator->Config();
    if (topNode == NULL)
    {
        KTERROR(conflog, "Top-level node was not found");
        return -3;
    }

    // Testing a configurable
    KTINFO(conflog, "Testing a configurable");

    KTTestConfigurable* testObj = new KTTestConfigurable();

    if (testObj->Configure(topNode->NodeAt(testObj->GetConfigName())))
    {
        KTINFO(conflog, "Configuration complete");
    }
    else
    {
        KTERROR(conflog, "Something went wrong during the configuration");
        delete testObj;
        return -4;
    }

    testObj->PrintConfig();

    delete testObj;


    // Testing nested data
    KTINFO(conflog, "Testing nested data");

    const KTParamNode* nestedNode = topNode->NodeAt("nested-data");
    if (nestedNode == NULL)
    {
        KTERROR(conflog, "Node <nested-data> was not found");
        return -5;
    }

    const KTParamNode* nestedNode2 = topNode->NodeAt("nested-data-2");
    if (nestedNode2 == NULL)
    {
        KTERROR(conflog, "Node <nested-data-2> was not found");
        return -5;
    }

    KTINFO(conflog, "Before merge:\n" << "Nested data:\n" << *nestedNode << "\n" << "Nested data 2:\n" << *nestedNode2);

    KTParamNode copyOfNestedNode(*nestedNode);
    copyOfNestedNode.Merge(*nestedNode2);

    KTINFO(conflog, "After merge:\n" << copyOfNestedNode);

    return 0;
}
