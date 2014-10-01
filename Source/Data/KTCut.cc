/*
 * KTCut.cc
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#include "KTCut.hh"

#include "KTLogger.hh"

namespace Katydid
{
    KTLOGGER(cutlog, "KTCut");

    /*  // THIS IS JUST AN EXAMPLE; ALSO SEE TestCut.cc
    const std::string KTExampleCut::Result::sName = "example-cut";

    KT_REGISTER_CUT(KTExampleCut, KTExampleCut::Result::sName);

    KTExampleCut::KTExampleCut(const std::string& name) :
            KTCut(name),
            fAwesomenessThreshold(1000000.)
    {}

    KTExampleCut::~KTExampleCut()
    {}

    bool KTExampleCut::Configure(const KTParamNode* node)
    {
        if (node == NULL) return true;
        SetAwesomenessThreshold(node->GetValue("awesomeness", GetAwesomenessThreshold()));
        return true;
    }

    bool KTExampleCut::Apply(KTData& data, KTSomeData& someData)
    {
        bool isCut = someData.Awesomeness() > fAwesomenessThreshold;
        data.GetCutStatus().AddCutResult< KTExampleCut::Result >(isCut);
        return isCut;
    }

    void Apply(KTDataPtr dataPtr)
    {
        if (! dataPtr->Has< KTSomeData >())
        {
            KTERROR(exlog, "Data type <KTSomeData> was not present");
            return;
        }
        Apply(dataPtr->Of< KTData >(), dataPtr->Of< KTSomeData >());
        return;
    }
    */

    KTCut::KTCut(const std::string& name) :
            KTConfigurable(name)
    {
    }

    KTCut::~KTCut()
    {
    }


    KTCutStatus::KTCutStatus() :
            fCutResults(new KTCutResultHandle()),
            fSummary()
    {
    }

    KTCutStatus::KTCutStatus(const KTCutStatus& orig) :
            fCutResults(dynamic_cast< KTCutResultHandle* >(orig.fCutResults->Clone())),
            fSummary()
    {
        UpdateStatus();
    }

    KTCutStatus::~KTCutStatus()
    {}

    KTCutStatus& KTCutStatus::operator=(const KTCutStatus& rhs)
    {
        fCutResults.reset(dynamic_cast< KTCutResultHandle* >(rhs.fCutResults->Clone()));
        UpdateStatus();
        return *this;
    }

    void KTCutStatus::UpdateStatus()
    {
        KTDEBUG(cutlog, "Updating cut summary");
        KTCutResult* cut = fCutResults.get()->Next(); // skip over KTCutResultHandle
        if (cut == NULL)
        {
            KTDEBUG(cutlog, "No cuts");
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
        KTDEBUG(cutlog, nCuts << " cuts");
        fSummary.resize(nCuts, false);
        // loop through again to set cuts
        cut = fCutResults.get()->Next(); // skip over KTCutResultHandle
        for (unsigned iCut = 0; iCut < nCuts; ++iCut)
        {
            fSummary[iCut] = cut->GetState();
            cut = cut->Next();
        }
        KTDEBUG(cutlog, "Cut summary bitset: " << fSummary);
        return;
    }

    bool KTCutStatus::AddCutResult(const std::string& cutName, bool state, bool doUpdateStatus)
    {
        if (! HasCutResult(cutName))
        {
            KTExtensibleStructFactory< KTCutResultCore >* factory = KTExtensibleStructFactory< KTCutResultCore >::GetInstance();
            KTCutResult* newCut = factory->Create(cutName, fCutResults.get());
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
            cut = cut->Next();
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
                KTExtensibleCutResult< KTCutStatus::KTCutResultHandle >()
    {
        fState = false;
    }
    KTCutStatus::KTCutResultHandle::~KTCutResultHandle()
    {}

    const std::string KTCutStatus::KTCutResultHandle::sName("top");


} /* namespace Katydid */
