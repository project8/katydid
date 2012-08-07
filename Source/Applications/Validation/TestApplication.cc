/*
 * TestApplication.cc
 *
 *  Created on: Aug 1, 2012
 *      Author: nsoblath
 *
 *  To test the command-line interface:
 *    > TestApplication /path/to/test-config.json -t"blah"
 *    > TestApplication /path/to/test-config.json --test-opt="blah"
 */


#include "KTTestConfigurable.hh"
#include "KTApplication.hh"
#include "KTCommandLineHandler.hh"
#include "KTPStoreNode.hh"

using namespace Katydid;
using namespace std;

KTLOGGER(testapplog, "katydid.applications.validation");

int main(int argc, char** argv)
{
    KTApplication* app = new KTApplication(argc, argv);
    app->ProcessCommandLine();


    //****************************
    // Read the config file
    //****************************
    app->ReadConfigFile();


    //****************************
    // Check basic command-line reading
    //****************************

    KTINFO(testapplog, "Application: " << app->GetCommandLineHandler()->GetExecutableName());
    KTINFO(testapplog, "Config filename: " << app->GetCommandLineHandler()->GetConfigFilename());


    //****************************
    // Check reading of command-line options
    //****************************

    app->ProcessCommandLine();

    if (app->GetCommandLineHandler()->IsCommandLineOptSet("test-opt"))
    {
        KTINFO(testapplog, "Test option is set to value <" << app->GetCommandLineHandler()->GetCommandLineValue< string >("test-opt") << ">");
    }
    else
    {
        KTINFO(testapplog, "Test option was not set");
    }


    //****************************
    // Check reading of the config file
    //****************************

    KTPStoreNode* topNode = app->GetNode("TestConfigurable");
    if (topNode == NULL)
    {
        KTERROR(testapplog, "Top-level node was not found");
        return -1;
    }

    KTTestConfigurable* testObj = new KTTestConfigurable();
    if (testObj->Configure(topNode))
    {
        KTINFO(testapplog, "Configuration complete");
    }
    else
    {
        KTERROR(testapplog, "Something went wrong during the configuration");
    }

    delete testObj;

    return 0;
}
