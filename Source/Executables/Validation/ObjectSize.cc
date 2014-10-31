/*
 * ObjectSize.cc
 *
 *  Created on: Aug 12, 2014
 *      Author: nsoblath
 */

#include "KTLogger.hh"

#include "complexpolar.hh"

#define GETSIZEOF(TYPE) \
    { \
        TYPE obj; \
        KTPROG(testlog, "Size of " << STRINGIFY(TYPE) <<": " << sizeof(obj) << " bytes"); \
    }

KTLOGGER(testlog, "ObjectSize");

using namespace Katydid;

int main()
{
    GETSIZEOF(complexpolar<double>);

    return 0;
}

