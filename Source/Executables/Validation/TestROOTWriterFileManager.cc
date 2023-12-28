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

#include "logger.hh"

#include "TFile.h"
#include "TH1.h"


using namespace Katydid;

LOGGER(testlog, "TestROOTWriterFileManager");


int main()
{
    KTROOTWriterFileManager* rwfm = KTROOTWriterFileManager::get_instance();

    //******************************************************************************

    KTPROG(testlog, "Test 1: writing a single file");
    // Intended outcome: file1.root containing a single histogram, h1, with a random gaussian histogram

    TFile* file1 = rwfm->OpenFile(nullptr, "file1.root", "RECREATE");
    if (file1 == nullptr)
    {
        LERROR(testlog, "Unable to create file1.root");
    }
    else
    {
        file1->cd();
        TH1D* h1 = new TH1D("h1", "Hist from a Gaussian", 100, -3, 3);
        h1->FillRandom("gaus", 10000);

        if (! rwfm->FinishFile(nullptr, "file1.root"))
        {
            LERROR(testlog, "Unable to finish the file for test 1");
        }
    }

    //******************************************************************************

    KTPROG(testlog, "Test 2: writing a single file, then updating that file");
    // Intended outcome: file2.root containing 2 histograms:
    //  - h2p1 with a random gaussian histogram
    //  - h2p2 with a random exponential histogram

    TFile* file2p1 = rwfm->OpenFile(nullptr, "file2.root", "RECREATE");
    if (file2p1 == nullptr)
    {
        LERROR(testlog, "Unable to create file2.root, take 1");
    }
    else
    {
        file2p1->cd();
        TH1D* h2p1 = new TH1D("h2p1", "Hist from a Gaussian", 100, -3, 3);
        h2p1->FillRandom("gaus", 10000);

        if (! rwfm->FinishFile(nullptr, "file2.root"))
        {
            LERROR(testlog, "Unable to finish the file for test 2, take 1");
        }
        else
        {

            TFile* file2p2 = rwfm->OpenFile(nullptr, "file2.root", "UPDATE");
            if (file2p2 == nullptr)
            {
                LERROR(testlog, "Unable to create file2.root, take 2");
            }
            else
            {
                file2p2->cd();
                TH1D* h2p2 = new TH1D("h2p2", "Hist from an exponential", 100, 0, 6);
                h2p2->FillRandom("expo", 10000);

                if (! rwfm->FinishFile(nullptr, "file2.root"))
                {
                    LERROR(testlog, "Unable to finish the file for test 2, take 2");
                }
            }
        }
    }


    //******************************************************************************

    KTPROG(testlog, "Test 3: writing to the same file in \"parallel\"");
    // Intended outcome: file3.root containing 3 histograms:
    //  - h3p1 with a random gaussian histogram
    //  - h3p2 with a random exponential histogram

    TFile* file3p1 = rwfm->OpenFile(0x0, "file3.root", "RECREATE");
    if (file3p1 == nullptr)
    {
        LERROR(testlog, "Unable to create file3.root, take 1");
    }
    else
    {
        file3p1->cd();
        TH1D* h3p1 = new TH1D("h3p1", "Hist from a Gaussian", 100, -3, 3);
        h3p1->FillRandom("gaus", 10000);

        TFile* file3p2 = rwfm->OpenFile((Nymph::KTWriter*)0x1, "file3.root", "RECREATE");
        if (file3p2 == nullptr)
        {
            LERROR(testlog, "Unable to create file3.root, take 2");
        }
        else
        {
            file3p2->cd();
            TH1D* h3p2 = new TH1D("h3p2", "Hist from an exponential", 100, 0, 6);
            h3p2->FillRandom("expo", 10000);

            if (! rwfm->FinishFile(nullptr, "file3.root"))
            {
                LERROR(testlog, "Unable to finish the file for test 3, take 1");
            }

            if (! rwfm->FinishFile((Nymph::KTWriter*)0x1, "file3.root"))
            {
                LERROR(testlog, "Unable to finish the file for test 3, take 2");
            }
        }
    }

    return 0;
}


