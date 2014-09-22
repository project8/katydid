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
            fCuts(new KTTopCut()),
            fSummary()
    {
    }

    KTMasterCut::~KTMasterCut()
    {}

    void KTMasterCut::UpdateSummary()
    {
        KTCutCore* cut = fCuts.get()->Next(); // skip over KTTopCut
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
        cut = fCuts.get()->Next(); // skip over KTTopCut
        for (unsigned iCut = 0; iCut < nCuts; ++iCut)
        {
            fSummary[iCut] = cut->GetState();
            cut = cut->Next();
        }
        return;
    }

    bool KTMasterCut::AddCut(const std::string& cutName, bool state)
    {

    }

    bool KTMasterCut::HasCut(const std::string& cutName) const
    {

    }

    bool KTMasterCut::GetCutState(const std::string& cutName) const
    {

    }

    void KTMasterCut::RemoveCut(const std::string& cutName)
    {

    }


} /* namespace Katydid */
