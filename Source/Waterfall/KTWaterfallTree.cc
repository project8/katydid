/*
 * KTWaterfallTree.cc
 *
 *  Created on: May 4, 2012
 *      Author: nsoblath
 */

#include "KTLogger.hh"
#include "KTWaterfallTree.hh"

using std::string;

ClassImp(Katydid::KTWaterfallTree);

namespace Katydid
{

    KTLOGGER(logger, "katydid.waterfall")

    KTWaterfallTree::KTWaterfallTree() :
            TTree(),
            fCandidate(NULL)
    {
        TBranch* candBranch = this->Branch("Candidate", &fCandidate, 32000, 1);
    }

    KTWaterfallTree::KTWaterfallTree(const string& name, const string& title) :
            TTree(name.c_str(), title.c_str()),
            fCandidate(NULL)
    {
        this->Branch("Candidate", &fCandidate, 32000, 1);
    }

    KTWaterfallTree::~KTWaterfallTree()
    {
        delete fCandidate;
    }

    Bool_t KTWaterfallTree::AddCandidate(KTWaterfallCandidate* candidate)
    {
        SetCandidatePointer(candidate);
        Int_t returnVal = Fill();
        if (returnVal == -1)
        {
            KTERROR(logger, "IO error while attempting to add entry to tree.");
            return kFALSE;
        }
        if (returnVal == 0)
        {
            KTWARN(logger, "No data transferred while adding entry to tree; branch may be disabled.");
            return kFALSE;
        }
        return kTRUE;
    }

    KTWaterfallCandidate* KTWaterfallTree::GetCandidate(Long64_t index)
    {
        Int_t returnVal = GetEntry(index);
        if (returnVal == -1)
        {
            KTERROR(logger, "IO error while attempting to retrieve entry " << index);
            return NULL;
        }
        if (returnVal == 0)
        {
            KTWARN(logger, "No data transferred upon retrieval of entry " << index << "; entry does not exist in tree.");
            return NULL;
        }
        return fCandidate;
    }

    KTWaterfallCandidate* KTWaterfallTree::ReleaseCandidate()
    {
        KTWaterfallCandidate* temp = fCandidate;
        fCandidate = NULL;
        return temp;
    }


} /* namespace Katydid */
