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

    void KTKDTreeData::CreateIndex(KTKDTreeData::DistanceMethod dist, unsigned maxLeafSize, unsigned component)
    {
        delete fComponentData[component].fTreeIndex;
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

} /* namespace Katydid */

