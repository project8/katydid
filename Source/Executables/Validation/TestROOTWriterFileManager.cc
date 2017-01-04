/*
 * TestROOTWriterFileManager.cc
 *
 *  Created on: Jan 4, 2017
 *      Author: N.S. Oblath
 *
 *  Usage: ./TestROOTWriterFileManager
 *
 */

#include "KTROOTWriterFileManager.hh"

#include "KTLogger.hh"

#include "TH1.h"


using namespace Katydid;

KTLOGGER(testlog, "TestROOTWriterFileManager");


int main()
{
    KTROOTWriterFileManager* rwfm = KTROOTWriterFileManager::get_instance();

    // Write a single file
    KTINFO(testlog, "Test 1: writing a single file");

    TFile* file0 = rwfm->OpenFile(nullptr, "file0.root", "RECREATE");
    TH1D* h1 = new TH1D("h1", "Hist from a Gaussian", 100, -3, 3);
    h1->FillRandom("gaus", 10000);
    h1->Write();

    if (! rwfm->FinishFile(nullptr, "file0.root"))
    {
        KTERROR(testlog, "Unable to finish the file for test 1");
    }

    return 0;
}


