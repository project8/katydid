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

    KTCutStatus::KTCutStatus() :
            fCutResults(new KTCutResultHandle()),
            fSummary()
    {
    }

    KTCutStatus::~KTCutStatus()
    {}

    void KTCutStatus::UpdateStatus()
    {
        KTCutResult* cut = fCutResults.get()->Next(); // skip over KTCutResultHandle
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
        cut = fCutResults.get()->Next(); // skip over KTCutResultHandle
        for (unsigned iCut = 0; iCut < nCuts; ++iCut)
        {
            fSummary[iCut] = cut->GetState();
            cut = cut->Next();
        }
        return;
    }

    bool KTCutStatus::AddCutResult(const std::string& cutName, bool state, bool doUpdateStatus)
    {
        if (! HasCutResult(cutName))
        {
            KTExtensibleStructFactory< KTCutResult >* factory = KTExtensibleStructFactory< KTCutResult >::GetInstance();
            KTExtensibleStructCore< KTCutResult >* newCut = factory->Create(cutName, fCutResults.get());
            if (newCut == NULL)
            {
                KTERROR(cutlog, "Could not create cut of type <" << cutName << ">");
                return false;
            }
            newCut->SetState(state);

            if (doUpdateStatus) UpdateStatus();
            return true;
        }
        return false;

    }

    bool KTCutStatus::HasCutResult(const std::string& cutName) const
    {
        if (GetCutResult(cutName) == NULL) return false;
        return true;

    }

    bool KTCutStatus::GetCutState(const std::string& cutName) const
    {
        const KTCutResult* cut = GetCutResult(cutName);
        if (cut == NULL) return false;
        return cut->GetState();
    }

    const KTCutResult* KTCutStatus::GetCutResult(const std::string& cutName) const
    {
        const KTCutResult* cut = fCutResults.get()->Next(); // skip over KTCutResultHandle
        while (cut != NULL)
        {
            if (cut->Name() == cutName) return cut;
        }
        return NULL;
    }

    KTCutResult* KTCutStatus::GetCutResult(const std::string& cutName)
    {
        KTCutResult* cut = fCutResults.get()->Next(); // skip over KTCutResultHandle
        while (cut != NULL)
        {
            if (cut->Name() == cutName) return cut;
            cut = cut->Next();
        }
        return NULL;
    }

    bool KTCutStatus::SetCutState(const std::string& cutName, bool state, bool doUpdateStatus)
    {
        KTCutResult* cut = GetCutResult(cutName);
        if (cut == NULL)
        {
            KTWARN(cutlog, "Cut <" << cutName << "> not found");
            return false;
        }
        cut->SetState(state);

        if (doUpdateStatus) UpdateStatus();
        return true;
    }

    /*
    void KTCutStatus::RemoveCutResult(const std::string& cutName, bool doUpdateStatus)
    {
        KTCutResult* cut = fCutResults.get(); // don't skip over KTCutResultHandle
        KTCutResult* nextCut = cut->Next();
        while (nextCut != NULL)
        {
            if (nextCut->Name() == cutName)
            {
                // problem: can't pass nextCut->Next() to cut->Next()
                if (doUpdateStatus) UpdateStatus();
            }
        }
        return;
    }
    */

    // private class KTCutStatus::KTCutResultHandle
    // purposefully not registered with the cut factory
    KTCutStatus::KTCutResultHandle::KTCutResultHandle() :
            KTExtensibleCut< KTCutStatus::KTCutResultHandle >()
    {
        fState = false;
    }
    KTCutStatus::KTCutResultHandle::~KTCutResultHandle()
    {}

    const std::string KTCutStatus::KTCutResultHandle::sName("top");


} /* namespace Katydid */
