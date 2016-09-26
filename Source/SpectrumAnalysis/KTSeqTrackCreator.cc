/*
 * KTConsensusThresholding.cc
 *
 *  Created on: Sep 15, 2016
 *      Author: Christine
 */

#include "KTSeqTrackCreator.hh"

#include "KTKDTreeData.hh"
#include "KTLogger.hh"
#include "KTParam.hh"

#include <cmath>;

using std::string;


namespace Katydid
{
    KTLOGGER(ctlog, "KTConsensusThresholding");

    KT_REGISTER_PROCESSOR(KTSeqTrackCreator, "seq-clustering");

    KTSeqTrackCreator::KTSeqTrackCreator(const std::string& name) :
            KTProcessor(name),
            fFrequencyRadius(2*pow(10,6)),
            fComponentDistance(2),
            fMinNumberOfBins(10),
            fSeqTrackSignal("kd-tree-out", this),
			fKDTreeSlot("kd-tree-in", this, &KTSeqTrackCreator::PointLineAssignment, &fSeqTrackSignal)
    {
    }

    KTSeqTrackCreator::~KTSeqTrackCreator()
    {
    }

    bool KTSeqTrackCreator::Configure(const KTParamNode* node)
    {
        if (node == NULL) return false;

        SetFrequencyRadius(node->GetValue("frequency-radius", GetFrequencyRadius()));
        SetComponentDistance(node->GetValue("max-bin-distance", GetComponentDistance()));
        SetMinNumberOfBins(node->GetValue("min-number-of-bins", GetMinNumberOfBins()));


        return true;
    }

    bool KTSeqTrackCreator::PointLineAssignment(KTKDTreeData& kdTreeData, KTScoredSpectrumData& slice)
    {
        KTINFO(ctlog, "Performing point line assignment on k-d tree data");
        unsigned nComponents = kdTreeData.GetNComponents();
        unsigned nLines = kdLines.GetNLines();

        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            const KTTreeIndex< double >* kdTree = kdTreeData.GetTreeIndex(iComponent);
            const std::vector< KTKDTreeData::Point >& setOfPoints = kdTreeData.GetSetOfPoints(iComponent);
            if (!KTSeqTrackCreator::LoopOverDiscriminatedPoints(kdTree, setOfPoints, slice))
            {
            	KTERROR(ctlog, "Sequential track creation failed");
            	return false;
            }

            KTDEBUG(ctlog, "Currently building" << KTLines.size() << " lines");

        }
        return true;
    }

    bool KTSeqTrackCreator::LoopOverDiscriminatedPoints(const KTTreeIndex< double >* kdTree, KTScoredSpectrumData& slice, const KTKDTreeData::SetOfPoints& setOfPoints, std::vector< size_t >& noiseIndices)
    {
        unsigned nPoints = kdTree->size();
        double timeDelta, frequencyDelta;
        for (unsigned iPoint = 0; iPoint < nPoints; ++iPoint)
        {
            KTDEBUG(ctlog, "checking point (" << iPoint + 1 << "/" << nPoints << ")");

            (this->*fFindDeltasPtr)(kdTree, setOfPoints, iPoint, timeDelta, frequencyDelta);

            unsigned voteCount = 0;
            if (! (timeDelta == 0))
            {
                //double slope = frequencyDelta / timeDelta;
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

    void KTConsensusThresholding::FindDeltasNearestNeighbor(const KTTreeIndex< double >* kdTree, const KTKDTreeData::SetOfPoints& setOfPoints, unsigned pid, double& deltaTime, double& deltaFreq)
    {
        KTTreeIndex< double >::Neighbors ne = kdTree->NearestNeighborsByNumber(pid, 2);
        deltaTime = setOfPoints[ne[1]].fCoords[0] - setOfPoints[pid].fCoords[0];
        deltaFreq = setOfPoints[ne[1]].fCoords[1] - setOfPoints[pid].fCoords[1];
        return;
    }

    void KTConsensusThresholding::FindDeltasNeighborsInRadius(const KTTreeIndex< double >* kdTree, const KTKDTreeData::SetOfPoints& setOfPoints, unsigned pid, double& deltaTime, double& deltaFreq)
    {
        KTTreeIndex< double >::Neighbors ne = kdTree->NearestNeighborsByRadius(pid, fMembershipRadius);
        unsigned nNeighbors = ne.size();
        double sumX, sumY, sumX2, sumXY;
        for (unsigned iNe = 0; iNe < nNeighbors; ++iNe)
        {
            sumX += setOfPoints[ne[iNe]].fCoords[0];
            sumY += setOfPoints[ne[iNe]].fCoords[1];
            sumX2 += setOfPoints[ne[iNe]].fCoords[0] * setOfPoints[ne[iNe]].fCoords[0];
            sumXY += setOfPoints[ne[iNe]].fCoords[0] * setOfPoints[ne[iNe]].fCoords[1];
        }
        //double xMean = sumX / (double)nNeighbors;
        //double yMean = sumY / (double)nNeighbors;
        double slope = (sumXY - sumX * sumY / (double)nNeighbors) / (sumX2 - sumX * sumX / (double)nNeighbors);
        //double intercept = yMean - slope * xMean;
        deltaTime = sqrt(fMembershipRadius * fMembershipRadius / (1. + slope*slope));
        deltaFreq = slope * deltaTime;
        //deltaFreq = deltaTime * (sumXY - sumX * sumY / (double)nNeighbors) / (sumX2 - sumX * deltaTime); // a.k.a. deltaTime * slope
        return;
    }


} /* namespace Katydid */
