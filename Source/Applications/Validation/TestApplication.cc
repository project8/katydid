/*
 * TestApplication.cc
 *
 *  Created on: Aug 1, 2012
 *      Author: nsoblath
 *
 *  To test the command-line interface:
 *    > TestApplication -s "blah"
 */


#include "KTCommandLineHandler.hh"
#include "KTTestConfigurable.hh"

using namespace Katydid;
using namespace std;

KTLOGGER(testapplog, "katydid.applications.validation");

int main(int argc, char** argv)
{
    KTCommandLineHandler* clHandler = KTCommandLineHandler::GetInstance();
    clHandler->TakeArguments(argc, argv);
    KTINFO(testapplog, "Application: " << clHandler->GetExecutableName());
    KTINFO(testapplog, "Config filename: " << clHandler->GetConfigFilename());

    clHandler->ProcessCommandLine();

    if (clHandler->IsCommandLineOptSet("string-data"))
    {
        KTINFO(testapplog, "String data option is set to value <" << clHandler->GetCommandLineValue< string >("string-data") << ">");
    }
    else
    {
        KTINFO(testapplog, "String data option was not set");
    }

    return 0;
}
