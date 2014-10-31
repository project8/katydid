/*
 * TestLogger.cc
 *
 *  Created on: May 01, 2012
 *      Author: nsoblath
 */

#include "KTLogger.hh"

#include "MonarchLogger.hpp"

#include <exception>
#include <iostream>

using namespace Katydid;
using namespace monarch;

KTLOGGER(logger, "TestLogger(katydid)")
MLOGGER(mlog, "TestLogger(monarch)");

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

        MDEBUG(mlog, "This is a DEBUG message from Monarch");
        MINFO(mlog, "This is an INFO message from Monarch");
        MWARN(mlog, "This is a WARN message from Monarch");
        MERROR(mlog, "This is an ERROR message from Monarch");
        MFATAL(mlog, "This is a FATAL message from Monarch");
    }
    catch (std::exception& e)
    {
        std::cout << "Exception caught: " << e.what() << std::endl;
    }

    return 0;
}
