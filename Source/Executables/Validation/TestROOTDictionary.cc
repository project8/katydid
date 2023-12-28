/*
 * TestROOTDictionary.cc
 *
 *  Created on: Aug 6, 2014
 *      Author: nsoblath
 */

#include "logger.hh"
#include "CROOTData.hh"

#include "TH1.h"
#include "TClass.h"
#include "TROOT.h"

LOGGER(testlog, "TestROOTDictionary");

#define TESTCLASS(NAME) \
    LINFO(testlog, "Checking for " << NAME); \
    testClass = TClass::GetClass(NAME, false); \
    if (testClass == NULL) \
    { \
        LERROR(testlog, "Did not find " << NAME); \
    } \
    else \
    { \
        LINFO(testlog, "Found " << NAME); \
    }

int main()
{
    Cicada::TProcessedTrackData data;
    LWARN(testlog, "proc data class name: " << data.ClassName());

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

