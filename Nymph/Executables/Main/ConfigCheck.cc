/*
 * ConfigCheck.cc
 *
 *  Created on: Jan 4, 2013
 *      Author: nsoblath
 *
 */

#include "KTApplication.hh"
#include "KTLogger.hh"

using namespace Nymph;

KTLOGGER(cclog, "ConfigCheck");

int main(int argc, char** argv)
{
    try
    {
        KTApplication app(argc, argv);
    }
    catch( std::exception& e )
    {
        KTERROR( cclog, "Something went wrong while processing the command line:\n" << e.what() );
        return -1;
    }

    return 0;
}
