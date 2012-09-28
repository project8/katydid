/*
 * Katydid.cc
 *
 *  Created on: Sep 28, 2012
 *      Author: nsoblath
 */

#include "KTApplication.hh"
#include "KTLogger.hh"
#include "KTProcessorToolbox.hh"

#include <string>

using namespace std;
using namespace Katydid;

KTLOGGER(katydidlog, "katydid.applications.main");


int main(int argc, char** argv)
{
    KTApplication app(argc, argv);
    app.ReadConfigFile();

    // Create and configure the processor toolbox.
    // This will create all of the requested processors, connect their signals and slots, and fill the run queue.
    string appConfigName("katydid");
    KTProcessorToolbox procTB;
    if (! app.Configure(&procTB, appConfigName))
    {
        KTERROR(katydidlog, "Unable to configure processor toolbox. Aborting.");
        return -1;
    }

    // Configure the processors
    if (! procTB.ConfigureProcessors(app.GetNode(appConfigName)))
    {
        KTERROR(katydidlog, "Unable to configure processors. Aborting.");
        return -2;
    }

    Bool_t success = procTB.Run();

    if (! success) return -3;
    return 0;
}
