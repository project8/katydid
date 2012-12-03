/*
 * TestThroughputProfiler.cc
 *
 *  Created on: Dec 2, 2012
 *      Author: nsoblath
 */

#include "KTLogger.hh"
#include "KTThroughputProfiler.hh"

using namespace Katydid;

KTLOGGER(testlog, "katydid.applications.validation");

int main()
{
    KTThroughputProfiler prof;

    UInt_t nIterations = 1000000;
    UInt_t sum = 0;
    KTINFO(testlog, "Test 1: " << nIterations << " iterations");
    prof.Start();
    for (UInt_t i=0; i<nIterations; i++)
    {
        sum++;
    }
    prof.Stop();
    timespec diff = prof.Elapsed();
    KTINFO(testlog, "Elapsed time: " << diff.tv_sec << " sec and " << diff.tv_nsec << " nsec");

    nIterations = 5000000;
    sum = 0;
    KTINFO(testlog, "Test 2: " << nIterations << " iterations");
    prof.Start();
    for (UInt_t i=0; i<nIterations; i++)
    {
        sum++;
    }
    prof.Stop();
    diff = prof.Elapsed();
    KTINFO(testlog, "Elapsed time: " << diff.tv_sec << " sec and " << diff.tv_nsec << " nsec");

    return 0;
}

