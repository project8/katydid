/*
 * TestLogger.cc
 *
 *  Created on: May 01, 2012
 *      Author: nsoblath
 */

#include "KTLogger.hh"

#include <exception>
#include <iostream>

using namespace Katydid;

KTLOGGER(logger, "TestLogger")

int main()
{
    try
    {
        KTDEBUG(logger, "This is a DEBUG message from Katydid");
        KTINFO(logger, "This is an INFO message from Katydid");
        KTPROG(logger, "This is a PROG message from Katydid");
        KTWARN(logger, "This is a WARN message from Katydid");
        KTERROR(logger, "This is an ERROR message from Katydid");
        KTFATAL(logger, "This is a FATAL message from Katydid");
    }
    catch (std::exception& e)
    {
        std::cout << "Exception caught: " << e.what() << std::endl;
    }

    return 0;
}
