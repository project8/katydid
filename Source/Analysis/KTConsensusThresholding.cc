/*
 * KTConsensusThresholding.cc
 *
 *  Created on: Aug 7, 2014
 *      Author: nsoblath
 */

#include "KTConsensusThresholding.hh"

#include "KTKDTreeData.hh"
#include "KTLogger.hh"
#include "KTParam.hh"
#include "KTSliceHeader.hh"

using std::string;


namespace Katydid
{
    KTLOGGER(kdlog, "KTConsensusThresholding");

    KT_REGISTER_PROCESSOR(KTConsensusThresholding, "consensus-thresholding");

    KTConsensusThresholding::KTConsensusThresholding(const std::string& name) :
            KTProcessor(name),
            fKDTreeSignal("kd-tree-out", this),
            fKDTreeSlot("kd-tree-in", this, &KTConsensusThresholding::AddPoints)
    {
    }

    KTConsensusThresholding::~KTConsensusThresholding()
    {
        RegisterSlot("make-tree", this, &KTConsensusThresholding::MakeTreeSlot);
    }

    bool KTConsensusThresholding::Configure(const KTParamNode* node)
    {
        if (node == NULL) return false;

        return true;
    }


} /* namespace Katydid */
