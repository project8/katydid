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



using std::stringstream;
using std::string;

namespace Katydid
{
    KTLOGGER(publog, "KTROOTTreeTypeWriterEvaluation");

    static Nymph::KTTIRegistrar< KTROOTTreeTypeWriter, KTROOTTreeTypeWriterEvaluation > sRTTWERegistrar;

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
    // Meta CC Locust MC
    //*********************

    void KTROOTTreeTypeWriterEvaluation::WriteMetaCCLocustMC(Nymph::KTDataPtr data)
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

    bool KTROOTTreeTypeWriterEvaluation::SetupMetaCCLocustMCTree()
    {
        if( fWriter->GetAccumulate() )
        {
            fWriter->GetFile()->GetObject( "mccLocustMC", fMetaCCLocustMCTree );

            if( fMetaCCLocustMCTree != NULL )
            {
                KTINFO( publog, "Tree already exists; will add to it" );
                fWriter->AddTree( fMetaCCLocustMCTree );

                fMetaCCLocustMCTree->SetBranchAddress("Length", &fCCLocustMC.fLength);
                fMetaCCLocustMCTree->SetBranchAddress("dfdt", &fCCLocustMC.fdfdt);
                fMetaCCLocustMCTree->SetBranchAddress("Power", &fCCLocustMC.fPower);
                fMetaCCLocustMCTree->SetBranchAddress("Efficiency", &fCCLocustMC.fEfficiency);
                fMetaCCLocustMCTree->SetBranchAddress("FalseRate", &fCCLocustMC.fFalseRate);

                return true;
            }
        }

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
