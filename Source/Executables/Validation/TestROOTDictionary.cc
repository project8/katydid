/*
 * TestROOTDictionary.cc
 *
 *  Created on: Aug 6, 2014
 *      Author: nsoblath
 */

#include "KTLogger.hh"
#include "CROOTData.hh"

#include "TH1.h"
#include "TClass.h"
#include "TROOT.h"

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
    Cicada::TProcessedTrackData data;
    KTWARN(testlog, "proc data class name: " << data.ClassName());

    TH1D hist("hist", "hist", 10, 0., 1.);
    hist.GetNbinsX();
    hist.ClassName();

    gROOT->GetListOfClasses()->Print();

    TClass* testClass;

    TESTCLASS("TClass");
    TESTCLASS("TH1D");
    TESTCLASS("Cicada::TProcessedTrackData");
    TESTCLASS("Cicada::TMultiTrackEventData");

    return 0;
}

