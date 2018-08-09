/*
 * KTDBSCANNoiseFiltering.cc
 *
 *  Created on: Aug 7, 2018
 *      Author: N.S. Oblath
 */

#include "KTDBSCANNoiseFiltering.hh"

#include "KTDBSCAN.hh"
#include "KTException.hh"
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
            fDataCount(0),
            fDBSCAN(),
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

        fDBSCAN.SetRadius(fRadius);
        fDBSCAN.SetMinPoints(fMinPoints);
        KTINFO(dnflog, "DBSCAN configured");

        try
        {
            for (unsigned iComponent = 0; iComponent < data.GetNComponents(); ++iComponent)
            {
                KTDEBUG(dnflog, "Clustering component " << iComponent);

                DoFiltering(data.GetTreeIndex(iComponent), data.GetSetOfPoints(iComponent));
            } // loop over components
        }
        catch(Nymph::KTException& e)
        {
            KTERROR(dnflog, "Error running DBSCAN filtering: " << e.what());
            return false;
        }

        KTDEBUG(dnflog, "Filtering complete");
        fFilteringDoneSignal();

        return true;
    }

    void KTDBSCANNoiseFiltering::DoFiltering(KTKDTreeData::TreeIndex* treeIndex, KTKDTreeData::SetOfPoints& points)
    {
        // do the clustering!
        KTINFO(dnflog, "Starting DBSCAN");
        DBSCAN_KDTree::DBSResults results;
        if (! fDBSCAN.DoClustering(*treeIndex, results))
        {
            throw Nymph::KTException() << "An error occurred while clustering";
        }
        KTDEBUG(dnflog, "DBSCAN finished");

        if (points.size() != results.fNoise.size() )
        {
            throw Nymph::KTException() << "Number of points doesn't equal the noise array size";
        }

        // double for loop, all the way across the sky!
        for (auto itPair = std::make_pair(points.begin(), results.fNoise.begin());
                itPair.first != points.end();
                ++itPair.first, ++itPair.second)
        {
            itPair.first->fNoiseFlag = *itPair.second;
        }

        return;
    }

} /* namespace Katydid */
