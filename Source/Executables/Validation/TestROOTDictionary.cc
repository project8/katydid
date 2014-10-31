/*
 * TestROOTDictionary.cc
 *
 *  Created on: Aug 6, 2014
 *      Author: nsoblath
 */

#include "KTLogger.hh"
#include "KTROOTData.hh"

#include "TH1.h"
#include "TClass.h"
#include "TROOT.h"

using namespace Katydid;

KTLOGGER(testlog, "TestROOTDictionary");

#define TESTCLASS(NAME) \
    KTINFO(testlog, "Checking for " << NAME); \
    testClass = TClass::GetClass(NAME, false); \
    if (testClass == NULL) \
    { \
        KTERROR(testlog, "Did not find " << NAME); \
    } \
    else \
    { \
        KTINFO(testlog, "Found " << NAME); \
    }

int main()
{
    TProcessedTrackData data;
    KTWARN(testlog, "proc data class name: " << data.ClassName());

    TH1D hist("hist", "hist", 10, 0., 1.);
    hist.GetNbinsX();
    hist.ClassName();

    gROOT->GetListOfClasses()->Print();

    TClass* testClass;

    TESTCLASS("TClass");
    TESTCLASS("TH1D");
    TESTCLASS("Katydid::TProcessedTrackData");
    TESTCLASS("Katydid::TMultiTrackEventData");

    return 0;
}

