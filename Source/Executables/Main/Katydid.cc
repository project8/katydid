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

#include "KTKatydidApp.hh"
#include "logger.hh"
#include "KTRunNymph.hh"

#include <memory>

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

    try
    {
        std::shared_ptr< Katydid::KTKatydidApp > app = std::make_shared< Katydid::KTKatydidApp >( argc, argv );
        return Nymph::RunNymph( std::dynamic_pointer_cast< Nymph::KTApplication >( app ) );
    }
    catch( std::exception& e )
    {
        LERROR( katydidlog, "Exception caught:\n" << e.what() );
        return -1;
    }

}
