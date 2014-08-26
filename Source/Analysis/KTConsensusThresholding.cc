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

using std::string;


namespace Katydid
{
    KTLOGGER(ctlog, "KTConsensusThresholding");

    KT_REGISTER_PROCESSOR(KTConsensusThresholding, "consensus-thresholding");

    KTConsensusThresholding::KTConsensusThresholding(const std::string& name) :
            KTProcessor(name),
            fMembershipRadius(1.0),
            fMinNumberVotes(1),
            fRemovePointsFlag(false),
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

        SetMembershipRadius(node->GetValue("membership-radius", GetMembershipRadius()));
        SetMinNumberVotes(node->GetValue("min-number-votes", GetMinNumberVotes()));
        SetRemovePointsFlag(node->GetValue("remove-points", GetRemovePointsFlag()));

        return true;
    }

    bool KTConsensusThresholding::ConsensusVote(KTKDTreeData& kdTreeData)
    {
        KTINFO(ctlog, "Performing conensus thresholding on k-d tree data");
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
            if (fRemovePointsFlag)
            {
                kdTreeData.RemovePoints(noisePoints); // also rebuilds k-d tree index
            }
            else
            {
                kdTreeData.FlagPoints(noisePoints); // does NOT rebuild k-d tree index
            }
        }
        return true;
    }

    bool KTConsensusThresholding::ConsensusVoteComponent(const KTTreeIndex< double >* kdTree, const KTKDTreeData::SetOfPoints& setOfPoints, std::vector< size_t >& noiseIndices)
    {   
        unsigned nPoints = kdTree->size();
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
                //double intercept = setOfPoints[iPoint].fCoords[1] - slope * setOfPoints[iPoint].fCoords[0];

                bool closeEnough = true;
                double test_pt[2];
                std::vector< std::pair< size_t, double > > indicesDist;
                double k = 2.0;
                while (closeEnough)
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
                        closeEnough = false;
                    }
                }
                closeEnough = true;
                k = -2.0;
                while (closeEnough)
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
                        closeEnough = false;
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
