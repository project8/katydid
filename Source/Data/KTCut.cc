/*
 * KTCut.cc
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#include "KTCut.hh"

namespace Katydid
{
    KTMasterCut::KTMasterCut() :
            fCuts(NULL),
            fSummary()
    {
    }

    KTMasterCut::~KTMasterCut()
    {}

    void KTMasterCut::UpdateSummary()
    {
        KTCutCore* cut = fCuts.get();
        if (cut == NULL)
        {
            fSummary.resize(1, false);
            return;
        }

        // loop through once to count cuts
        unsigned nCuts = 0;
        while (cut != NULL)
        {
            ++nCuts;
            cut = cut->Next();
        }
        fSummary.resize(nCuts, false);
        // loop through again to set cuts
        cut = fCuts.get();
        for (unsigned iCut = 0; iCut < nCuts; ++iCut)
        {
            fSummary[iCut] = cut->GetState();
            cut = cut->Next();
        }
        return;
    }


} /* namespace Katydid */
