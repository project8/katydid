/*
 * TestLogger.cc
 *
 *  Created on: May 01, 2012
 *      Author: nsoblath
 */

#include "KTLogger.hh"

using namespace Katydid;

KTLOGGER(logger, "TestLogger")

int main()
{
    KTDEBUG(logger, "This is a DEBUG message");
    KTINFO(logger, "This is an INFO message");
    KTPROG(logger, "This is a PROG message");
    KTWARN(logger, "This is a WARN message");
    KTERROR(logger, "This is an ERROR message");
    KTFATAL(logger, "This is a FATAL message");

    return 0;
}
