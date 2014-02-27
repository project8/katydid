/*
 * Katydid.cc
 *
 *  Created on: Sep 28, 2012
 *      Author: nsoblath
 *
 *  This program will run any processor-based code in Katydid.
 *  All of the action is setup with a config file.
 *  See KTProcessorToolbox for details on the configuration option.
 */

#include "KTApplication.hh"
#include "KTLogger.hh"
#include "KTProcessorToolbox.hh"

#include <string>

using namespace std;
using namespace Katydid;

KTLOGGER(katydidlog, "Katydid");


int main(int argc, char** argv)
{
    KTPROG(katydidlog, "Welcome to Katydid!");
    KTDEBUG(katydidlog,
            "\n" <<
            "                                                                            ,=  \n" <<
            "                                                                           =    \n" <<
            "                                                                          7     \n" <<
            "                                       .:.                               I      \n" <<
            "                                          I?                           :~       \n" <<
            "       +$                                   ~+                        I,        \n" <<
            "        $Z        .,=IZ8NNND$INM,             :7.                   ,I          \n" <<
            "         7OONDO7+~  ,=7$Z88$Z?=,?N               $                 :=           \n" <<
            "             .,?NN?             ~IZMDO$?~         .7              =,            \n" <<
            "          :ZM7,                       ~7, +7Z$$7$Z8NI            $              \n" <<
            "        MN.                              Z=          D?,       .D               \n" <<
            "          +M                                787,     .Z?:   . 7$                \n" <<
            "            ,M$                                 ?Z    :D$   ,Z                  \n" <<
            "               ,M=                                 +7.:7OO?$                    \n" <<
            "                  7O,                                IZ78O: +M:                 \n" <<
            "                     +O8D8~                        .,   M     ?M:               \n" <<
            "                   ,M7     =:MNO,                 :DIDID:        M8M$           \n" <<
            "                .+M:             ,:IIZZZ7==  ,?OMI +O                           \n" <<
            "               7N                         IMO~      ZD.                         \n" <<
            "            =M=                          M,          ~M=                        \n" <<
            "          7D                            M             .IZ                       \n" <<
            "        8$                            .Z:                M                      \n" <<
            "    .ZN=                             .8I                . M=N                   \n" <<
            "MDNNZ                                D=                                         \n" <<
            "                                    $=                                          \n" <<
            "                                    =                                           \n");

    try
    {
        KTApplication app(argc, argv);
    }
    catch( std::exception& e )
    {
        KTERROR( katydidlog, "Something went wrong while processing the command line:\n" << e.what() );
        return -1;
    }
    //if (! app.ReadConfigFile())
    //{
    //    KTERROR(katydidlog, "Unable to read config file. Aborting.");
    //    return -1;
    //}

    const KTParamNode* parentConfigNode = app.GetConfigurator();

    // Create and configure the processor toolbox.
    // This will create all of the requested processors, connect their signals and slots, and fill the run queue.
    //string appConfigName("katydid");
    KTProcessorToolbox procTB;
    ;
    if ( ! procTB.Configure( parentConfigNode->NodeAt( procTB.GetConfigName() ) ) )
    {
        KTERROR(katydidlog, "Unable to configure processor toolbox. Aborting.");
        return -2;
    }

    // Configure the processors
    //KTPStoreNode node = app.GetNode(appConfigName);
    //if (! node.IsValid())
    //{
    //    KTERROR(katydidlog, "Configuration node <" << appConfigName << "> was not found. Aborting");
    //    return -3;
    //}
    if ( ! procTB.ConfigureProcessors( parentConfigNode ) )
    {
        KTERROR(katydidlog, "Unable to configure processors. Aborting.");
        return -4;
    }

    // Execute the run queue!
    bool success = procTB.Run();

    KTPROG(katydidlog, "That's all, folks!");

    if (! success) return -5;
    return 0;
}
