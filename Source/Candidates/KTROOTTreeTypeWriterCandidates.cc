/*
 * KTROOTTreeTypeWriterCandidates.cc
 *
 *  Created on: Jan 23, 2013
 *      Author: nsoblath
 */

#include "KTROOTTreeTypeWriterCandidates.hh"

#include "KTFrequencyCandidate.hh"
#include "KTFrequencyCandidateData.hh"
#include "KTTIFactory.hh"
//#include "KTLogger.hh"

#include "TFrequencyCandidateData.hh"

#include "TFile.h"
#include "TTree.h"

#include <sstream>

using std::stringstream;
using std::string;

namespace Katydid
{
    //KTLOGGER(publog, "katydid.output");

    static KTDerivedTIRegistrar< KTROOTTreeTypeWriter, KTROOTTreeTypeWriterCandidates > sRTTWCRegistrar;

    KTROOTTreeTypeWriterCandidates::KTROOTTreeTypeWriterCandidates() :
            KTROOTTreeTypeWriter(),
            //KTTypeWriterCandidates()
            fFreqCandidateTree(NULL),
            fFreqCandidateData(NULL)
    {
    }

    KTROOTTreeTypeWriterCandidates::~KTROOTTreeTypeWriterCandidates()
    {
        //delete fFreqCandidateTree;
        delete fFreqCandidateData;
    }


    void KTROOTTreeTypeWriterCandidates::RegisterSlots()
    {
        fWriter->RegisterSlot("frequency-candidates", this, &KTROOTTreeTypeWriterCandidates::WriteFrequencyCandidates, "void (const KTFrequencyCandidateData*)");
        return;
    }


    //*********************
    // Frequency Candidates
    //*********************

    void KTROOTTreeTypeWriterCandidates::WriteFrequencyCandidates(const KTFrequencyCandidateData* data)
    {
        if (! fWriter->OpenAndVerifyFile()) return;

        if (fFreqCandidateTree == NULL) SetupFrequencyCandidateTree();

        // Load() also clears any existing data
        fFreqCandidateData->Load(*data);

        fFreqCandidateTree->Fill();

        return;
    }

    void KTROOTTreeTypeWriterCandidates::SetupFrequencyCandidateTree()
    {
        fFreqCandidateTree = new TTree("freqCand", "Frequency Candidates");
        fWriter->AddTree(fFreqCandidateTree);

        fFreqCandidateData = new TFrequencyCandidateData();

        fFreqCandidateTree->Branch("freqCandidates", "Katydid::TFrequencyCandidateData", &fFreqCandidateData);

        return;
    }

} /* namespace Katydid */



