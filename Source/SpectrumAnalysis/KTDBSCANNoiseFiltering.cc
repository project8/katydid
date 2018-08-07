/*
 * KTDBSCANNoiseFiltering.cc
 *
 *  Created on: Aug 7, 2018
 *      Author: N.S. Oblath
 */

#include "KTDBSCANNoiseFiltering.hh"

#include "KTKDTreeData.hh"
#include "KTLogger.hh"
#include "KTMath.hh"

namespace Katydid
{
    KTLOGGER(dnflog, "KTDBSCANNoiseFiltering");

    KT_REGISTER_PROCESSOR(KTDBSCANNoiseFiltering, "dbscan-noise-filtering");

    const unsigned KTDBSCANNoiseFiltering::fNDimensions = 2;

    KTDBSCANNoiseFiltering::KTDBSCANNoiseFiltering(const std::string& name) :
            KTProcessor(name),
            //fRadii(fNDimensions),
            fMinPoints(3),
            fRadius(1.),
            fCandidates(),
            fDataCount(0),
            fFilteringDoneSignal("kd-tree", this),
            fKDTreeSlot("kd-tree", this, &KTDBSCANNoiseFiltering::DoFiltering)
    {
    }

    KTDBSCANNoiseFiltering::~KTDBSCANNoiseFiltering()
    {
    }

    bool KTDBSCANNoiseFiltering::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return false;

        SetMinPoints(node->get_value("min-points", GetMinPoints()));
        SetRadius(node->get_value("radius", GetRadius()));

        return true;
    }

    bool KTDBSCANNoiseFiltering::DoFiltering(KTKDTreeData& data)
    {
        KTPROG(dnflog, "Starting DBSCAN noise filtering");

        typedef KTDBSCAN< KTKDTreeData::TreeIndex > DBSCAN;

        DBSCAN dbscan;

        dbscan.SetRadius(fRadius);
        dbscan.SetMinPoints(fMinPoints);
        KTINFO(dnflog, "DBSCAN configured");

        for (unsigned iComponent = 0; iComponent < data.GetNComponents(); ++iComponent)
        {
            KTDEBUG(dnflog, "Clustering component " << iComponent);

            // do the clustering!
            KTINFO(dnflog, "Starting DBSCAN");
            DBSCAN::DBSResults results;
            if (! dbscan.DoClustering(*(data.GetTreeIndex(iComponent)), results))
            {
                KTERROR(dnflog, "An error occurred while clustering");
                return false;
            }
            KTDEBUG(dnflog, "DBSCAN finished");

            std::vector< KTKDTreeData::Point >& points = data.GetSetOfPoints(iComponent);
            if (points.size() != results.fNoise.size() )
            {
                KTERROR(dnflog, "Number of points doesn't equal the noise array size");
                return false;
            }

            // double for loop, all the way across the sky!
            for (auto itPair = std::make_pair(points.begin(), results.fNoise.begin());
                    itPair.first != points.end();
                    ++itPair.first, ++itPair.second)
            {
                itPair.first->fNoiseFlag = *itPair.second;
            }
        } // loop over components

        KTDEBUG(dnflog, "Filtering complete");
        fFilteringDoneSignal();

        return true;
    }

} /* namespace Katydid */
