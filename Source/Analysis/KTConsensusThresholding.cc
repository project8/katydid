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
    KTLOGGER(ctlog, "KTConsensusThresholding");

    KT_REGISTER_PROCESSOR(KTConsensusThresholding, "consensus-thresholding");

    KTConsensusThresholding::KTConsensusThresholding(const std::string& name) :
            KTProcessor(name),
            fMembershipRadius(1.0),
            fMinNumberVotes(1),
            fKDTreeSignal("kd-tree-out", this),
            fKDTreeSlot("kd-tree-in", this, &KTConsensusThresholding::ConsensusVote, &fKDTreeSignal)
    {
    }

    KTConsensusThresholding::~KTConsensusThresholding()
    {
    }

    bool KTConsensusThresholding::Configure(const KTParamNode* node)
    {
        if (node == NULL) return false;
        //if (node->Has("membership-radius"))
        //{
            SetMembershipRadius(node->GetValue("membership-radius", GetMembershipRadius()));
        //}
        //if (node->Has("min-number-votes"))
        //{
            SetMinNumberVotes(node->GetValue< unsigned >("min-number-votes", GetMinNumberVotes()));
        //}

        return true;
    }

    bool KTConsensusThresholding::ConsensusVote(KTKDTreeData& kdTreeData)
    {
        unsigned nComponents = kdTreeData.GetNComponents();
        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            const KTTreeIndex< double >* kdTree = kdTreeData.GetTreeIndex(iComponent);
            const std::vector< KTKDTreeData::Point >& setOfPoints = kdTreeData.GetSetOfPoints(iComponent);
            std::vector< size_t > noisePoints;
            if (! ConsensusVoteComponent(kdTree, setOfPoints, noisePoints))
            {
                KTERROR(ctlog, "Consensus thresholding failed");
                return false;
            }
            KTDEBUG(ctlog, "Consensus thresholding is removing " << noisePoints.size() << " points");
            kdTreeData.RemovePoint(noisePoints); // also rebuilds k-d tree index
        }
        return true;
    }

    bool KTConsensusThresholding::ConsensusVoteComponent(const KTTreeIndex< double >* kdTree, const std::vector< KTKDTreeData::Point >& setOfPoints, std::vector< size_t >& noiseIndices)
    {   
        int nPoints = kdTree->size();
        noiseIndices.clear();
        for (unsigned iPoint = 0; iPoint < nPoints; ++iPoint)
        {
            //size_t nearestID = kdTree->knnSearch(iPoint, 2).GetIndicesAndDists()[1].second;
            KTTreeIndex< double >::Neighbors ne = kdTree->knnSearch(iPoint, 2);
            size_t nearestID = ne[1];

            double frequencyDelta = setOfPoints[nearestID].fCoords[1] - setOfPoints[iPoint].fCoords[1];
            double timeDelta = setOfPoints[nearestID].fCoords[0] - setOfPoints[iPoint].fCoords[0];
            unsigned voteCount = 0;
            if (! timeDelta == 0)
            {
                double slope = frequencyDelta / timeDelta;
                double intercept = setOfPoints[iPoint].fCoords[1] - slope * setOfPoints[iPoint].fCoords[0];

                bool close_enough = true;
                double test_pt[2];
                std::vector< std::pair< size_t, double > > indicesDist;
                double k = 2.0;
                while (close_enough)
                {
                    test_pt[0] = setOfPoints[iPoint].fCoords[0] + k * timeDelta;
                    test_pt[1] = setOfPoints[iPoint].fCoords[1] + k * frequencyDelta;
                    kdTree->RadiusSearch(test_pt, fMembershipRadius, indicesDist, nanoflann::SearchParams(32, 0, true));
                    if (indicesDist.size() > 0)
                    {
                        k += 1.0;
                        voteCount += 1;
                    }
                    else
                    {
                        close_enough = false;
                    }
                }
                close_enough = true;
                k = -2.0;
                while (close_enough)
                {
                    test_pt[0] = setOfPoints[iPoint].fCoords[0] + k * timeDelta;
                    test_pt[1] = setOfPoints[iPoint].fCoords[1] + k * frequencyDelta;
                    kdTree->RadiusSearch(test_pt, fMembershipRadius, indicesDist, nanoflann::SearchParams(32, 0, true));
                    if (indicesDist.size() > 0)
                    {
                        k -= 1.0;
                        voteCount += 1;
                    }
                    else
                    {
                        close_enough = false;
                    }
                }
            }
            if (voteCount < fMinNumberVotes)
            {
                noiseIndices.push_back(iPoint);
            }
        }
        return true;
    }

} /* namespace Katydid */
