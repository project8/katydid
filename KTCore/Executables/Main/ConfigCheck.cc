/*
 * ConfigCheck.cc
 *
 *  Created on: Jan 4, 2013
 *      Author: nsoblath
 *
 */

#include "KTApplication.hh"
#include "KTLogger.hh"

using namespace Katydid;

KTLOGGER(katydidlog, "ConfigCheck");

int main(int argc, char** argv)
{
    try
    {
        KTApplication app(argc, argv);
    }
    catch( std::exception& e )
    {
        KTERROR( katydidlog, "Something went wrong while processing the command line:\n" << e.what() );
        return -1;
    }

    return 0;
}
