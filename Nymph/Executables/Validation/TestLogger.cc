/*
 * TestLogger.cc
 *
 *  Created on: May 01, 2012
 *      Author: nsoblath
 */

#include "KTLogger.hh"

#include <exception>
#include <iostream>

using namespace Nymph;

KTLOGGER(logger, "TestLogger")

int main()
{
    try
    {
        KTDEBUG(logger, "This is a DEBUG message from Nymph");
        KTINFO(logger, "This is an INFO message from Nymph");
        KTPROG(logger, "This is a PROG message from Nymph");
        KTWARN(logger, "This is a WARN message from Nymph");
        KTERROR(logger, "This is an ERROR message from Nymph");
        KTFATAL(logger, "This is a FATAL message from Nymph");
    }
    catch (std::exception& e)
    {
        std::cout << "Exception caught: " << e.what() << std::endl;
    }

    return 0;
}
