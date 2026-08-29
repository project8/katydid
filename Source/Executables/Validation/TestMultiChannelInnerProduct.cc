/*
 * TestTemplate.cc
 *
 *  Created on: Jan 26, 2021
 *      Author: N.S. Oblath
 *
 *  Tests performance of KTProcessorTemplate
 *
 *  Usage: > TestTemplate
 */

#include "KTProcessorTemplate.hh"

#include "KTDummyDataClass1.hh"
#include "KTDummyDataClass2.hh"
#include "KTDummyDataClass3.hh"

#include "KTNewDummyDataClass.hh"

#include "KTLogger.hh"

using namespace std;
using namespace Katydid;

KTLOGGER(testlog, "TestTemplate");

int main()
{
    // Create and setup processor
    KTProcessorTemplate tProc;

    // TODO: Set parameters in tProc


    // Test processing of KTDummyDataClass1

    KTDummyDataClass1 tDDC1;

    // TODO: Fill tDDC1

    tProc.AnalyzeDummyClass1(tDDC1);

    // Check Results
    KTNewDummyDataClass& tNDDC1 = tDDC1.Of< KTNewDummyDataClass >();

    // TODO: Verify that the contents of tNDDC1 are as expected


    // Test processing of KTDummyDataClass2

    KTDummyDataClass2 tDDC2;

    // TODO: Fill tDDC2

    tProc.AnalyzeDummyClass2(tDDC2);

    // Check Results
    KTNewDummyDataClass& tNDDC2 = tDDC2.Of< KTNewDummyDataClass >();

    // TODO: Verify that the contents of tNDDC2 are as expected
    

    // Test processing of KTDummyDataClass3

    KTDummyDataClass3 tDDC3;

    // TODO: Fill tDDC3

    tProc.AnalyzeDummyClass3(tDDC3);

    // Check Results
    KTNewDummyDataClass& tNDDC3 = tDDC3.Of< KTNewDummyDataClass >();

    // TODO: Verify that the contents of tNDDC3 are as expected
    

    return 0;
}
