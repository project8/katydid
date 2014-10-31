/*
 * KTNNFilter.cc
 *
 *  Created on: Sept 2, 2014
 *      Author: nsoblath
 */

#include "KTNNFilter.hh"

#include "KTKDTreeData.hh"
#include "KTLogger.hh"
#include "KTParam.hh"

using std::string;


namespace Katydid
{
    KTLOGGER(flog, "KTNNFilter");

    KT_REGISTER_PROCESSOR(KTNNFilter, "nn-filter");

    KTNNFilter::KTNNFilter(const std::string& name) :
            KTProcessor(name),
            fMaxDist(1.0),
            fRadius(1.0),
            fMinInRadius(1),
            fRemoveNoiseFlag(false),
            fKDTreeSignal("kd-tree-out", this),
            fKDTreeNNSlot("kdt-nn", this, &KTNNFilter::FilterByNNDist, &fKDTreeSignal),
            fKDTreeRadiusSlot("kdt-rad", this, &KTNNFilter::FilterByMinInRadius, &fKDTreeSignal)
    {
    }

    KTNNFilter::~KTNNFilter()
    {
    }

    bool KTNNFilter::Configure(const KTParamNode* node)
    {
        if (node == NULL) return false;

        SetMaxDist(node->GetValue("max-dist", GetMaxDist()));
        SetRadius(node->GetValue("radius", GetRadius()));
        SetMinInRadius(node->GetValue("min-in-radius", GetMinInRadius()));
        SetRemoveNoiseFlag(node->GetValue("remove-noise", GetRemoveNoiseFlag()));

        return true;
    }

    bool KTNNFilter::FilterByNNDist(KTKDTreeData& kdTreeData)
    {
        KTINFO(flog, "Performing filtering by nearest-neighbor distance on k-d tree data");
        unsigned nComponents = kdTreeData.GetNComponents();
        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            const KTTreeIndex< double >* kdTree = kdTreeData.GetTreeIndex(iComponent);
            const std::vector< KTKDTreeData::Point >& setOfPoints = kdTreeData.GetSetOfPoints(iComponent);
            std::vector< size_t > noisePoints;
            if (! FilterByNNDist(kdTree, setOfPoints, noisePoints))
            {
                KTERROR(flog, "Filtering by nearest-neighbor distance failed");
                return false;
            }
            if (fRemoveNoiseFlag)
            {
                KTDEBUG(flog, "Filtering by nearest-neighbor distance is removing " << noisePoints.size() << " points");
                kdTreeData.RemovePoints(noisePoints); // also rebuilds k-d tree index
            }
            else
            {
                KTDEBUG(flog, "Filtering by nearest-neighbor distance is flagging " << noisePoints.size() << " points");
                kdTreeData.FlagPoints(noisePoints); // does NOT rebuild k-d tree index
            }
        }
        return true;
    }

    bool KTNNFilter::FilterByMinInRadius(KTKDTreeData& kdTreeData)
    {
        KTINFO(flog, "Performing filtering by minimum in radius on k-d tree data");
        unsigned nComponents = kdTreeData.GetNComponents();
        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            const KTTreeIndex< double >* kdTree = kdTreeData.GetTreeIndex(iComponent);
            const std::vector< KTKDTreeData::Point >& setOfPoints = kdTreeData.GetSetOfPoints(iComponent);
            std::vector< size_t > noisePoints;
            if (! FilterByMinInRadius(kdTree, setOfPoints, noisePoints))
            {
                KTERROR(flog, "Filtering by minimum in radius failed");
                return false;
            }
            if (fRemoveNoiseFlag)
            {
                KTDEBUG(flog, "Filtering by minimum in radius is removing " << noisePoints.size() << " points");
                kdTreeData.RemovePoints(noisePoints); // also rebuilds k-d tree index
            }
            else
            {
                KTDEBUG(flog, "Filtering by minimum in radius is flagging " << noisePoints.size() << " points");
                kdTreeData.FlagPoints(noisePoints); // does NOT rebuild k-d tree index
            }
        }
        return true;
    }

    bool KTNNFilter::FilterByNNDist(const KTTreeIndex< double >* kdTree, const KTKDTreeData::SetOfPoints& setOfPoints, std::vector< size_t >& noiseIndices)
    {
        double maxDistSq = fMaxDist * fMaxDist;
        unsigned nPoints = kdTree->size();
        noiseIndices.clear();
        double timeDelta, frequencyDelta;
        for (unsigned iPoint = 0; iPoint < nPoints; ++iPoint)
        {
            KTTreeIndex< double >::Neighbors ne = kdTree->NearestNeighborsByNumber(iPoint, 2);
            timeDelta = setOfPoints[ne[1]].fCoords[0] - setOfPoints[iPoint].fCoords[0];
            frequencyDelta = setOfPoints[ne[1]].fCoords[1] - setOfPoints[iPoint].fCoords[1];
            if (timeDelta*timeDelta + frequencyDelta*frequencyDelta > maxDistSq)
            {
                noiseIndices.push_back(iPoint);
            }
        }
        return true;
    }

    bool KTNNFilter::FilterByMinInRadius(const KTTreeIndex< double >* kdTree, const KTKDTreeData::SetOfPoints& setOfPoints, std::vector< size_t >& noiseIndices)
    {   
        unsigned nPoints = kdTree->size();
        noiseIndices.clear();
        double timeDelta, frequencyDelta;
        unsigned nPtsInRadiusThreshold = fMinInRadius + 1; // since the neighbors returned by the k-d tree search function always includes the point itself
        for (unsigned iPoint = 0; iPoint < nPoints; ++iPoint)
        {
            KTTreeIndex< double >::Neighbors ne = kdTree->NearestNeighborsByRadius(iPoint, fRadius);
            if (ne.size() < nPtsInRadiusThreshold)
            {
                noiseIndices.push_back(iPoint);
            }
        }
        return true;
    }

} /* namespace Katydid */
