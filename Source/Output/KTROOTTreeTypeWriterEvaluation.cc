/*
 * KTROOTTreeTypeWriterEvaluation.cc
 *
 *  Created on: May 31, 2013
 *      Author: nsoblath
 */

#include "KTROOTTreeTypeWriterEvaluation.hh"

#include "KTCCResults.hh"
#include "KTTIFactory.hh"
#include "KTLogger.hh"

#include "TFile.h"
#include "TTree.h"

#include <sstream>

using boost::shared_ptr;

using std::stringstream;
using std::string;

namespace Katydid
{
    KTLOGGER(publog, "katydid.output");

    static KTDerivedTIRegistrar< KTROOTTreeTypeWriter, KTROOTTreeTypeWriterEvaluation > sRTTWERegistrar;

    KTROOTTreeTypeWriterEvaluation::KTROOTTreeTypeWriterEvaluation() :
            KTROOTTreeTypeWriter(),
            fMetaCCLocustMCTree(NULL),
            fCCLocustMC()
    {
    }

    KTROOTTreeTypeWriterEvaluation::~KTROOTTreeTypeWriterEvaluation()
    {
    }


    void KTROOTTreeTypeWriterEvaluation::RegisterSlots()
    {
        fWriter->RegisterSlot("meta-cc-locust-mc", this, &KTROOTTreeTypeWriterEvaluation::WriteMetaCCLocustMC);
        return;
    }


    //*********************
    // Frequency Evaluation
    //*********************

    void KTROOTTreeTypeWriterEvaluation::WriteMetaCCLocustMC(shared_ptr< KTData > data)
    {
        KTCCResults& ccResults = data->Of< KTCCResults >();

        if (! fWriter->OpenAndVerifyFile()) return;

        if (fMetaCCLocustMCTree == NULL)
        {
            if (! SetupMetaCCLocustMCTree())
            {
                KTERROR(publog, "Something went wrong while setting up the frequency candidate tree! Nothing was written.");
                return;
            }
        }

        fCCLocustMC.fLength = ccResults.GetEventLength();
        fCCLocustMC.fdfdt = ccResults.Getdfdt();
        fCCLocustMC.fPower = ccResults.GetSignalPower();

        fCCLocustMC.fEfficiency = ccResults.GetEfficiency();
        fCCLocustMC.fFalseRate = ccResults.GetFalseRate();

        fMetaCCLocustMCTree->Fill();

        return;
    }

    Bool_t KTROOTTreeTypeWriterEvaluation::SetupMetaCCLocustMCTree()
    {
        fMetaCCLocustMCTree = new TTree("mccLocustMC", "Meta-Candidate Comparison -- Locust MC");
        if (fMetaCCLocustMCTree == NULL)
        {
            KTERROR(publog, "Tree was not created!");
            return false;
        }
        fWriter->AddTree(fMetaCCLocustMCTree);

        fMetaCCLocustMCTree->Branch("Length", &fCCLocustMC.fLength, "fLength/d");
        fMetaCCLocustMCTree->Branch("dfdt", &fCCLocustMC.fdfdt, "fdfdt/d");
        fMetaCCLocustMCTree->Branch("Power", &fCCLocustMC.fPower, "fPower/d");
        fMetaCCLocustMCTree->Branch("Efficiency", &fCCLocustMC.fEfficiency, "fEfficiency/d");
        fMetaCCLocustMCTree->Branch("FalseRate", &fCCLocustMC.fFalseRate, "fFalseRate/d");

        return true;
    }


} /* namespace Katydid */
