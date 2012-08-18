/*
 * TestApplication.cc
 *
 *  Created on: Aug 1, 2012
 *      Author: nsoblath
 *
 *  To test the command-line interface:
 *    > TestApplication /path/to/test-config.json -t"blah"
 *    > TestApplication /path/to/test-config.json --test-opt="blah"
 *    > TestApplication /path/to/test-config.json --TestConfiguration.int-data=500
 */


#include "KTTestConfigurable.hh"
#include "KTApplication.hh"
#include "KTCommandLineOption.hh"
#include "KTPStoreNode.hh"

using namespace Katydid;
using namespace std;

KTLOGGER(testapplog, "katydid.applications.validation");

// Add an application-specific command-line option
static KTCommandLineOption< string > sTestAppOption("TestApplication", "Application-specific command-line option", "test-app-opt", 'a');


int main(int argc, char** argv)
{
    //****************************
    // Create the app; this performs the initial command-line processing
    //****************************

    KTApplication* app = new KTApplication(argc, argv);


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

    if (app->GetCommandLineHandler()->IsCommandLineOptSet("test-app-opt"))
    {
        KTINFO(testapplog, "Test application option is set to value <" << app->GetCommandLineHandler()->GetCommandLineValue< string >("test-app-opt") << ">");
    }
    else
    {
        KTINFO(testapplog, "Test application option was not set");
    }

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
