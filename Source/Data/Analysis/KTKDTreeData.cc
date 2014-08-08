/*
 * KTKDTreeData.cc
 *
 *  Created on: Aug 8, 2014
 *      Author: nsoblath
 */

#include "KTKDTreeData.hh"

namespace Katydid
{
    KTKDTreeData::KTKDTreeData() :
            KTExtensibleData< KTKDTreeData >(),
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

    void KTKDTreeData::CreateIndex(KTKDTreeData::DistanceMethod dist, unsigned maxLeafSize, unsigned component)
    {
        delete fComponentData[component].fTreeIndex;
        if (dist == kManhattan)
        {
            fComponentData[component].fTreeIndex = new KTTreeIndexManhattan< double, PointCloudAdaptor >(2, PointCloudAdaptor(fComponentData[component].fCloud), nanoflann::KDTreeSingleIndexAdaptorParams(maxLeafSize));
        }
        else
        {
            fComponentData[component].fTreeIndex = new KTTreeIndexEuclidean< double, PointCloudAdaptor >(2, PointCloudAdaptor(fComponentData[component].fCloud), nanoflann::KDTreeSingleIndexAdaptorParams(maxLeafSize));
        }
        return;
    }

} /* namespace Katydid */

