/*
 * KTCut.cc
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#include "KTCut.hh"

#include "KTExtensibleStructFactory.hh"
#include "KTLogger.hh"

namespace Katydid
{
    KTLOGGER(cutlog, "KTCut");

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

    bool KTMasterCut::AddCut(const std::string& cutName, bool state, bool doUpdateSummary)
    {
        if (! HasCut(cutName))
        {
            KTExtensibleStructFactory< KTCutCore >* factory = KTExtensibleStructFactory< KTCutCore >::GetInstance();
            KTExtensibleStruct< KTCutCore >* newCut = factory->Create(cutName, fCuts.get());
            if (newCut == NULL)
            {
                KTERROR(cutlog, "Could not create cut of type <" << cutName << ">");
                return false;
            }
            newCut->SetState(state);

            if (doUpdateSummary) UpdateSummary();
            return true;
        }
        return false;

    }

    bool KTMasterCut::HasCut(const std::string& cutName) const
    {
        if (GetCut(cutName) == NULL) return false;
        return true;

    }

    bool KTMasterCut::GetCutState(const std::string& cutName) const
    {
        const KTCutCore* cut = GetCut(cutName);
        if (cut == NULL) return false;
        return cut->GetState();
    }

    const KTCutCore* KTMasterCut::GetCut(const std::string& cutName) const
    {
        const KTCutCore* cut = fCuts.get()->Next(); // skip over KTTopCut
        while (cut != NULL)
        {
            if (cut->Name() == cutName) return cut;
        }
        return false;
    }

    KTCutCore* KTMasterCut::GetCut(const std::string& cutName)
    {
        KTCutCore* cut = fCuts.get()->Next(); // skip over KTTopCut
        while (cut != NULL)
        {
            if (cut->Name() == cutName) return cut;
            cut = cut->Next();
        }
        return false;
    }

    bool KTMasterCut::SetCutState(const std::string& cutName, bool state, bool doUpdateSummary=true)
    {
        KTCutCore* cut = GetCut(cutName);
        if (cut == NULL)
        {
            KTWARN(cutlog, "Cut <" << cutName << "> not found");
            return false;
        }
        cut->SetState(state);

        if (doUpdateSummary) UpdateSummary();
        return true;
    }

    /*
    void KTMasterCut::RemoveCut(const std::string& cutName, bool doUpdateSummary)
    {
        KTCutCore* cut = fCuts.get(); // don't skip over KTTopCut
        KTCutCore* nextCut = cut->Next();
        while (nextCut != NULL)
        {
            if (nextCut->Name() == cutName)
            {
                // problem: can't pass nextCut->Next() to cut->Next()
                if (doUpdateSummary) UpdateSummary();
            }
        }
        return;
    }
    */

} /* namespace Katydid */
