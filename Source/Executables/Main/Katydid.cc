/*
 * Katydid.cc
 *
 *  Created on: Sep 28, 2012
 *      Author: N.S. Oblath
 *  Updated on: Jan 24, 2024
 *      Author: N.S. Oblath
 *
 *  This program will run any processor-based code in Katydid.
 *  All of the action is setup with a config file.
 *  See Nymph::ProcessorToolbox for details on the configuration option.
 */

#include "RunNymph.hh"

#include "application.hh"
#include "logger.hh"
#include "param_codec.hh"
#include "signal_handler.hh"

LOGGER( katydidlog, "Katydid" );

int main( int argc, char** argv )
{
    KTPROG( katydidlog, "Welcome to Katydid!" );
    LDEBUG( katydidlog,
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

    // Start handling signals
    scarab::signal_handler t_sig_hand;

    // Create the application
    scarab::main_app the_main;

    // Set the default verbosity
    the_main.set_global_verbosity(scarab::logger::ELevel::eDebug);

    // add the typical CL options
    Nymph::AddRunNymphOptions( the_main );

    //Runs RunNymph() and sets  the_return based on its return value
    int the_return = -1;
    auto t_callback = [&](){
        the_return = Nymph::RunNymph( the_main.primary_config() );
    };

    the_main.callback( t_callback );

    // Parse CL options and run the application
    CLI11_PARSE( the_main, argc, argv );

	return the_return;
    
}
