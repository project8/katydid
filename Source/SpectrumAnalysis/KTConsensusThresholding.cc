/*
 * KTConsensusThresholding.cc
 *
 *  Created on: Aug 7, 2014
 *      Author: nsoblath
 */

#include "KTConsensusThresholding.hh"

#include "KTKDTreeData.hh"
#include "logger.hh"

using std::string;


namespace Katydid
{
    LOGGER(ctlog, "KTConsensusThresholding");

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
                LDEBUG(ctlog, "Setting slope function to \"nearest-neighbor\"");
                fFindDeltasPtr = &KTConsensusThresholding::FindDeltasNearestNeighbor;
            }
            else if (fdValue == "radius")
            {
                LDEBUG(ctlog, "Setting slope function to \"radius\"");
                fFindDeltasPtr = &KTConsensusThresholding::FindDeltasNeighborsInRadius;
            }
            else
            {
                LERROR(ctlog, "Invalid value for \"find-deltas\": <" << fdValue << ">");
                return false;
            }
        }

        return true;
    }

    bool KTConsensusThresholding::ConsensusVote(KTKDTreeData& kdTreeData)
    {
        LINFO(ctlog, "Performing conensus thresholding on k-d tree data");
        unsigned nComponents = kdTreeData.GetNComponents();
        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            const KTTreeIndex< double >* kdTree = kdTreeData.GetTreeIndex(iComponent);
            const std::vector< KTKDTreeData::Point >& setOfPoints = kdTreeData.GetSetOfPoints(iComponent);
            std::vector< size_t > noisePoints;
            if (! ConsensusVoteComponent(kdTree, setOfPoints, noisePoints))
            {
                LERROR(ctlog, "Consensus thresholding failed");
                return false;
            }
            LDEBUG(ctlog, "Consensus thresholding is removing " << noisePoints.size() << " points");
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
            LDEBUG(ctlog, "checking point (" << iPoint << "/" << nPoints << "); coords: (" << setOfPoints[iPoint].fCoords[0] << ", " << setOfPoints[iPoint].fCoords[1] << ")");
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
                    LDEBUG(ctlog, "x = "<< test_pt[0] << " = " << setOfPoints[iPoint].fCoords[0] << " + " << k << " * " << timeDelta <<"\t" << "y = "<< test_pt[1] << " = " << setOfPoints[iPoint].fCoords[1] << " + " << k << " * " << frequencyDelta );

                    kdTree->RadiusSearch(test_pt, fMembershipRadius, indicesDist, nanoflann::SearchParams(32, 0, true));
                    LDEBUG(ctlog, "Number of points in radius search: " << indicesDist.size());
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
                LDEBUG(ctlog, "Changing direction");

                k = -1.0;
                while (closeEnough)
                {
                    test_pt[0] = setOfPoints[iPoint].fCoords[0] + k * timeDelta;
                    test_pt[1] = setOfPoints[iPoint].fCoords[1] + k * frequencyDelta;
                    LDEBUG(ctlog, "x = "<< test_pt[0] << " = " << setOfPoints[iPoint].fCoords[0] << " + " << k << " * " << timeDelta <<"\t" << "y = "<< test_pt[1] << " = " << setOfPoints[iPoint].fCoords[1] << " + " << k << " * " << frequencyDelta );

                    kdTree->RadiusSearch(test_pt, fMembershipRadius, indicesDist, nanoflann::SearchParams(32, 0, true));
                    LDEBUG(ctlog, "Number of points in radius search: " << indicesDist.size());
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
        // Find the neighbors within a circle around a given point (#pid), and use them to extract a slope/trend to be returned
        KTTreeIndex< double >::Neighbors ne = kdTree->NearestNeighborsByRadius(pid, fMembershipRadius);
        //LWARN(ctlog, "Find neighbors in radius around pid = " << pid);
        //std::stringstream neistr;
        //for (unsigned iNe = 0; iNe < ne.size(); ++iNe)
        //{
        //    neistr << ne[iNe] << ", ";
        //}
        //LWARN(ctlog, "Neighbors: " << neistr.str());
        unsigned nNeighbors = ne.size();

        if (nNeighbors > 2)
        {
            double xCoord = 0, yCoord = 0;
            double sumX = 0, sumY = 0, sumX2 = 0, sumXY = 0;
            double minY = std::numeric_limits< double >::max(), maxY = std::numeric_limits< double >::min();
            for (unsigned iNe = 0; iNe < nNeighbors; ++iNe)
            {
                xCoord = setOfPoints[ne[iNe]].fCoords[0];
                yCoord = setOfPoints[ne[iNe]].fCoords[1];
                //LWARN(ctlog, "pidNeighbor: " <<  pidNeighbor << "   xCoord = " << xCoord << ",   yCoord = " << yCoord);
                sumX += xCoord;
                //LWARN(ctlog, "sumX: " << sumX << " -- added " << xCoord);
                sumY += yCoord;
                //LWARN(ctlog, "sumY: " << sumY << " -- added " << yCoord);
                sumX2 += xCoord * xCoord;
                //LWARN(ctlog, "sumX2: " << sumX2 << " -- added " << xCoord * xCoord);
                sumXY += xCoord * yCoord;
                //LWARN(ctlog, "sumXY: " << sumXY << " -- added " << xCoord * yCoord);
                if (yCoord > maxY) maxY = yCoord;
                if (yCoord < minY) minY = yCoord;
                //LWARN(ctlog, "minY: " << minY << "    maxY: " << maxY);
            }
            //LWARN(ctlog, "### sumXY = " << sumXY << "   sumX = " << sumX << "   sumY = " << sumY << "   sumX2 = " << sumX2 << "   double(nNeighbors) = " << double(nNeighbors));
            //LWARN(ctlog, "### sumXY = " << sumXY << "   sumX * sumY = " << sumX * sumY << "   (sumXY - sumX * sumY / double(nNeighbors) = " << (sumXY - sumX * sumY / double(nNeighbors)));
            //LWARN(ctlog, "### sumX2 = " << sumX2 << "   sumX * sumX = " << sumX * sumX << "   (sumX2 - sumX * sumX / double(nNeighbors) = " << (sumX2 - sumX * sumX / double(nNeighbors)));
            //LWARN(ctlog, "### (sumXY - sumX * sumY / double(nNeighbors)) / (sumX2 - sumX * sumX / double(nNeighbors)) = " << (sumXY - sumX * sumY / double(nNeighbors)) / (sumX2 - sumX * sumX / double(nNeighbors)));
            if (double(nNeighbors) * sumX2 != sumX * sumX)
            {
                double slope = (sumXY - sumX * sumY / double(nNeighbors)) / (sumX2 - sumX * sumX / double(nNeighbors));
                //LWARN(ctlog, "sumX2 - sumX*sumX = " << sumX2 - sumX*sumX << "    double(nNeighbors) = " << double(nNeighbors));
                // Pythagorean theorem to get deltaTime: radius^2 = delta-t^2 + delta-f^2
                deltaTime = sqrt(fMembershipRadius * fMembershipRadius / (1. + slope*slope));
                deltaFreq = slope * deltaTime;
                //LWARN(ctlog, "slope = " << slope << "    deltaTime = " << deltaTime << "    deltaFreq = " << deltaFreq);
            }
            else
            {
                deltaTime = 0.;
                deltaFreq = maxY - minY;
                //LWARN(ctlog, "vertical    deltaTime = " << deltaTime << "    deltaFreq = " << deltaFreq);
            }
        }
        else if (nNeighbors == 2)
        {
            // in this case there isn't a group of neighbors to use to get a trend, so we'll use the same method as in FindDeltasNearestNeighbor
            deltaTime = setOfPoints[ne[1]].fCoords[0] - setOfPoints[ne[0]].fCoords[0];
            deltaFreq = setOfPoints[ne[1]].fCoords[1] - setOfPoints[ne[0]].fCoords[1];
            //LWARN(ctlog, "(2 points) -- deltaTime = " << deltaTime << "    deltaFreq = " << deltaFreq);
        }
        else // nNeighbors = 0 or 1
        {
            // 0 should never happen
            // 1 means nothing else is within the radius
            deltaTime = 0;
            deltaFreq = 0;
        }

        return;
    }


} /* namespace Katydid */
