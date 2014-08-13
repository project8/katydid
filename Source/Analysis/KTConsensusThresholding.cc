/*
 * KTConsensusThresholding.cc
 *
 *  Created on: Aug 7, 2014
 *      Author: nsoblath
 */

#include "KTConsensusThresholding.hh"

//#include "KTKDTreeData.hh"
//#include "KTKDTree.hh"
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
            fMembershipRadius(1.0),
            fKDTreeSignal("kd-tree-out", this),
            fKDTreeSlot("kd-tree-in", this, &KTConsensusThresholding::ConsensusVote)
    {
    }

    KTConsensusThresholding::~KTConsensusThresholding()
    {
    }

    bool KTConsensusThresholding::Configure(const KTParamNode* node)
    {
        if (node == NULL) return false;
        if (node->Has("membership-radius"));
        {
            SetMembershipRadius(node->GetValue< double >("membership-radius"));
        }

        return true;
    }

    bool KTConsensusThresholding::ConsensusVote(KTKDTreeData& kdTreeData)
    {
        unsigned nComponents = kdTreeData.GetNComponents();
        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            const KTTreeIndex< double >* kdTree = kdTreeData.GetTreeIndex(iComponent);
        }
    }

    bool KTConsensusThresholding::ConsensusVoteComponent(KTTreeIndex< double >& kdTree)
    {   
        int nPoints = 0;//this needs to come from the kdtree
        //nPoints = kdTree->kdtree_get_point_count()
        int currentClusterID = 0;

        int* votes[nPoints][nPoints];
        int* membership[nPoints];
        
        for (unsigned iPoint = 0; iPoint < nPoints; ++iPoint)
        {
            size_t nearestID = kdTree.knnSearch(iPoint, 2).GetIndicesAndDists()[1].second;
        }
    }

} /* namespace Katydid */
