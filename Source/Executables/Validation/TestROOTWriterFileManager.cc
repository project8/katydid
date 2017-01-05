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

#include "TFile.h"
#include "TH1.h"


using namespace Katydid;

KTLOGGER(testlog, "TestROOTWriterFileManager");


int main()
{
    KTROOTWriterFileManager* rwfm = KTROOTWriterFileManager::get_instance();

    //******************************************************************************

    KTPROG(testlog, "Test 1: writing a single file");
    // Intended outcome: file1.root containing a single histogram, h1, with a random gaussian histogram

    TFile* file1 = rwfm->OpenFile(nullptr, "file1.root", "RECREATE");
    file1->cd();
    TH1D* h1 = new TH1D("h1", "Hist from a Gaussian", 100, -3, 3);
    h1->FillRandom("gaus", 10000);

    if (! rwfm->FinishFile(nullptr, "file1.root"))
    {
        KTERROR(testlog, "Unable to finish the file for test 1");
    }

    //******************************************************************************

    KTPROG(testlog, "Test 2: writing a single file, then updating that file");
    // Intended outcome: file2.root containing 2 histograms:
    //  - h2p1 with a random gaussian histogram
    //  - h2p2 with a random exponential histogram

    TFile* file2p1 = rwfm->OpenFile(nullptr, "file2.root", "RECREATE");
    file2p1->cd();
    TH1D* h2p1 = new TH1D("h2p1", "Hist from a Gaussian", 100, -3, 3);
    h2p1->FillRandom("gaus", 10000);

    if (! rwfm->FinishFile(nullptr, "file2.root"))
    {
        KTERROR(testlog, "Unable to finish the file for test 1");
    }

    TFile* file2p2 = rwfm->OpenFile(nullptr, "file2.root", "UPDATE");
    file2p2->cd();
    TH1D* h2p2 = new TH1D("h2p2", "Hist from an exponential", 100, 0, 6);
    h2p2->FillRandom("expo", 10000);

    if (! rwfm->FinishFile(nullptr, "file2.root"))
    {
        KTERROR(testlog, "Unable to finish the file for test 1");
    }

    return 0;
}


