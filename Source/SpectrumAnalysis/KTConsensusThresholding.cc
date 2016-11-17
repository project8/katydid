/*
 * KTConsensusThresholding.cc
 *
 *  Created on: Aug 7, 2014
 *      Author: nsoblath
 */

#include "KTConsensusThresholding.hh"

#include "KTKDTreeData.hh"
#include "KTLogger.hh"

using std::string;


namespace Katydid
{
    KTLOGGER(ctlog, "KTConsensusThresholding");

    KT_REGISTER_PROCESSOR(KTConsensusThresholding, "consensus-thresholding");

    KTConsensusThresholding::KTConsensusThresholding(const std::string& name) :
            KTProcessor(name),
            fMembershipRadius(1.0),
            fMinNumberVotes(1),
            fRemoveNoiseFlag(false),
            fFindDeltasPtr(&KTConsensusThresholding::FindDeltasNeighborsInRadius),
            fKDTreeSignal("kd-tree-out", this),
            fKDTreeSlot("kd-tree-in", this, &KTConsensusThresholding::ConsensusVote, &fKDTreeSignal)
    {
    }

    KTConsensusThresholding::~KTConsensusThresholding()
    {
    }

    bool KTConsensusThresholding::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return false;

        SetMembershipRadius(node->get_value("membership-radius", GetMembershipRadius()));
        SetMinNumberVotes(node->get_value("min-number-votes", GetMinNumberVotes()));
        SetRemoveNoiseFlag(node->get_value("remove-noise", GetRemoveNoiseFlag()));

        if (node->has("slope-algorithm"))
        {
            string fdValue(node->get_value("slope-algorithm"));
            if (fdValue == "nearest-neighbor")
            {
                KTDEBUG(ctlog, "Setting slope function to \"nearest-neighbor\"");
                fFindDeltasPtr = &KTConsensusThresholding::FindDeltasNearestNeighbor;
            }
            else if (fdValue == "radius")
            {
                KTDEBUG(ctlog, "Setting slope function to \"radius\"");
                fFindDeltasPtr = &KTConsensusThresholding::FindDeltasNeighborsInRadius;
            }
            else
            {
                KTERROR(ctlog, "Invalid value for \"find-deltas\": <" << fdValue << ">");
                return false;
            }
        }

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
            if (fRemoveNoiseFlag)
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
        double timeDelta, frequencyDelta;
        for (unsigned iPoint = 0; iPoint < nPoints; ++iPoint)
        {
            KTDEBUG(ctlog, "checking point (" << iPoint + 1 << "/" << nPoints << ")");
            //FindDeltasFirstNeighbor(kdTree, setOfPoints, iPoint, timeDelta, frequencyDelta);
            (this->*fFindDeltasPtr)(kdTree, setOfPoints, iPoint, timeDelta, frequencyDelta);

            unsigned voteCount = 0;
            if (! (timeDelta == 0))
            {
                //double slope = frequencyDelta / timeDelta;
                //double intercept = setOfPoints[iPoint].fCoords[1] - slope * setOfPoints[iPoint].fCoords[0];

                bool closeEnough = true;
                double test_pt[2];
                std::vector< std::pair< size_t, double > > indicesDist;
                double k = 1.0;
                while (closeEnough)
                {
                    test_pt[0] = setOfPoints[iPoint].fCoords[0] + k * timeDelta;
                    test_pt[1] = setOfPoints[iPoint].fCoords[1] + k * frequencyDelta;
                    KTDEBUG(ctlog, "x = "<< test_pt[0] <<"\t" << "y = "<< test_pt[1]);

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
                KTDEBUG(ctlog, "Changing direction");

                k = -1.0;
                while (closeEnough)
                {
                    test_pt[0] = setOfPoints[iPoint].fCoords[0] + k * timeDelta;
                    test_pt[1] = setOfPoints[iPoint].fCoords[1] + k * frequencyDelta;
                    KTDEBUG(ctlog, "x = "<< test_pt[0] <<"\t" << "y = "<< test_pt[1]);
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

    void KTConsensusThresholding::FindDeltasNearestNeighbor(const KTTreeIndex< double >* kdTree, const KTKDTreeData::SetOfPoints& setOfPoints, unsigned pid, double& deltaTime, double& deltaFreq)
    {
        // Find the nearest neighbor to a given point (#pid) and returns the spacing in time and frequency between these points (signed Deltas -> slope)
        KTTreeIndex< double >::Neighbors ne = kdTree->NearestNeighborsByNumber(pid, 2);
        deltaTime = setOfPoints[ne[1]].fCoords[0] - setOfPoints[pid].fCoords[0];
        deltaFreq = setOfPoints[ne[1]].fCoords[1] - setOfPoints[pid].fCoords[1];
        return;
    }

    void KTConsensusThresholding::FindDeltasNeighborsInRadius(const KTTreeIndex< double >* kdTree, const KTKDTreeData::SetOfPoints& setOfPoints, unsigned pid, double& deltaTime, double& deltaFreq)
    {
        // Find the neighboors withinin a circle around a given point (#pid), and use them to extract a slope/trend to be returned
        KTTreeIndex< double >::Neighbors ne = kdTree->NearestNeighborsByRadius(pid, fMembershipRadius);
        unsigned nNeighbors = ne.size();
        double sumX, sumY, sumX2, sumXY;
        sumX = 0;
        sumY = 0;
        sumX2 = 0;
        sumXY = 0;
        for (unsigned iNe = 0; iNe < nNeighbors; ++iNe)
        {
            sumX += setOfPoints[ne[iNe]].fCoords[0];
            sumY += setOfPoints[ne[iNe]].fCoords[1];
            sumX2 += setOfPoints[ne[iNe]].fCoords[0] * setOfPoints[ne[iNe]].fCoords[0];
            sumXY += setOfPoints[ne[iNe]].fCoords[0] * setOfPoints[ne[iNe]].fCoords[1];
        }
        
        double slope = 0;
        if (nNeighbors!=0){ slope = (sumXY - sumX * sumY / (double)nNeighbors) / (sumX2 - sumX * sumX / (double)nNeighbors); }

        deltaTime = sqrt(fMembershipRadius * fMembershipRadius / (1. + slope*slope));
        deltaFreq = slope * deltaTime;
        return;
    }


} /* namespace Katydid */
