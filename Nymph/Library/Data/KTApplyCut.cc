/*
 * KTApplyCut.cc
 *
 *  Created on: Oct 06, 2014
 *      Author: nsoblath
 */

#include "KTApplyCut.hh"

#include "KTCut.hh"
#include "KTParam.hh"

using std::string;


namespace Nymph
{
    KTLOGGER(cutlog, "KTApplyCut");

    KT_REGISTER_PROCESSOR(KTApplyCut, "apply-cut");

    KTApplyCut::KTApplyCut(const std::string& name) :
            KTProcessor(name),
            fCut(NULL),
            fAfterCutSignal("after-cut", this)
    {
        RegisterSlot("apply-cut", this, &KTApplyCut::ApplyCut);
    }

    KTApplyCut::~KTApplyCut()
    {
        delete fCut;
    }

    bool KTApplyCut::Configure(const KTParamNode* node)
    {
        // Config-file settings
        if (node == NULL) return false;

        for (KTParamNode::const_iterator nodeIt = node->Begin(); nodeIt != node->End(); ++nodeIt)
        {
            // first do configuration values we know about
            // as it happens, there aren't any

            // any remaining should be cut names
            // ignore any that don't work
            if (SelectCut(nodeIt->first))
            {
                if (nodeIt->second->IsNode())
                {
                    fCut->Configure(&nodeIt->second->AsNode());
                }
                continue;
            }
        }

        if (fCut == NULL)
        {
            KTERROR(cutlog, "No cut was selected");
            return false;
        }
        return true;
    }

    void KTApplyCut::SetCut(KTCut* cut)
    {
        delete fCut;
        fCut = cut;
        return;
    }

    bool KTApplyCut::SelectCut(const string& cutName)
    {
        KTCut* tempCut = KTNOFactory< KTCut >::GetInstance()->Create(cutName);
        if (tempCut == NULL)
        {
            KTERROR(cutlog, "Invalid cut name given: <" << cutName << ">.");
            return false;
        }
        SetCut(tempCut);
        return true;
    }


    void KTApplyCut::ApplyCut(KTDataPtr data)
    {
        if (fCut == NULL)
        {
            KTERROR(cutlog, "No cut was specified");
            return;
        }
        if (fCut->Apply(data))
        {
            fAfterCutSignal(data);
        }
        return;
    }



} /* namespace Nymph */
