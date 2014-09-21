/*
 * KTKDTreeData.cc
 *
 *  Created on: Aug 8, 2014
 *      Author: nsoblath
 */

#include "KTKDTreeData.hh"

#include "KTLogger.hh"

namespace Katydid
{
    KTLOGGER(kdtlog, "KTKDTreeData");

    const unsigned KTKDTreeData::fNDimensions = 2;

    KTKDTreeData::KTKDTreeData() :
            KTExtensibleData< KTKDTreeData >(),
            fDataWillContinue(false),
            fLastSlice(0),
            fXScaling(1.),
            fYScaling(1.),
            fComponentData(1)
    {
    }

    KTKDTreeData::~KTKDTreeData()
    {
        for (unsigned iComponent = 0; iComponent < fComponentData.size(); ++iComponent)
        {
            delete fComponentData[iComponent].fTreeIndex;
        }
    }

    void KTKDTreeData::BuildIndex(unsigned component)
    {
        BuildIndex(fComponentData[component].fDistanceMethod, fComponentData[component].fMaxLeafSize, component);
    }

    void KTKDTreeData::BuildIndex(KTKDTreeData::DistanceMethod dist, unsigned maxLeafSize, unsigned component)
    {
        delete fComponentData[component].fTreeIndex;
        fComponentData[component].fDistanceMethod = dist;
        fComponentData[component].fMaxLeafSize = maxLeafSize;
        if (dist == kManhattan)
        {
            KTDEBUG(kdtlog, "Creating index with Manhattan distance metric");
            fComponentData[component].fTreeIndex = new KTTreeIndexManhattan< double, KTPointCloud< Point > >(fNDimensions, fComponentData[component].fCloud, nanoflann::KDTreeSingleIndexAdaptorParams(maxLeafSize));
            fComponentData[component].fTreeIndex->BuildIndex();
        }
        else
        {
            KTDEBUG(kdtlog, "Creating index with Euclidean distance metric");
            fComponentData[component].fTreeIndex = new KTTreeIndexEuclidean< double, KTPointCloud< Point > >(fNDimensions, fComponentData[component].fCloud, nanoflann::KDTreeSingleIndexAdaptorParams(maxLeafSize));
            fComponentData[component].fTreeIndex->BuildIndex();
        }
        return;
    }

    void KTKDTreeData::RemovePoints(const std::vector< size_t >& points, unsigned component)
    {
#ifndef NDEBUG
        size_t origSize = fComponentData[component].fCloud.fPoints.size();
#endif
        for (std::vector< size_t >::const_reverse_iterator pIt = points.rbegin(); pIt != points.rend(); ++pIt)
        //for (size_t iPoint=0; iPoint < points.size(); ++iPoint)
        {
            //index = points.back();
            fComponentData[component].fCloud.fPoints.erase(fComponentData[component].fCloud.fPoints.begin() + *pIt);
            //points.pop_back();
        }
        KTDEBUG(kdtlog, "Removing " << points.size() << " points; original size: " << origSize << "; new size: " << fComponentData[component].fCloud.fPoints.size());
        BuildIndex();
        return;
    }

    void KTKDTreeData::ClearPoints(unsigned component)
    {
        ClearIndex(component);
        fComponentData[component].fCloud.fPoints.clear();
        return;
    }

    void KTKDTreeData::ClearIndex(unsigned component)
    {
        delete fComponentData[component].fTreeIndex;
        fComponentData[component].fTreeIndex = NULL;
        return;
    }

    void KTKDTreeData::FlagPoints(const std::vector< size_t >& points, unsigned component, bool flag)
    {
        for (std::vector< size_t >::const_reverse_iterator pIt = points.rbegin(); pIt != points.rend(); ++pIt)
        {
            fComponentData[component].fCloud.fPoints[*pIt].fNoiseFlag = flag;
        }
        return;
    }


} /* namespace Katydid */

