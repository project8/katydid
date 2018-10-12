/**
 @file KTDBSCANNoiseFiltering.hh
 @brief Contains KTDBSCANNoiseFiltering
 @details Noise filtering using DBSCAN
 @author: N.S. Oblath
 @date: Aug 7, 2018
 */

#ifndef KTDBSCANNOISEFILTERING_HH_
#define KTDBSCANNOISEFILTERING_HH_

#include "KTProcessor.hh"

#include "KTDBSCAN.hh"
#include "KTKDTreeData.hh"
#include "KTMemberVariable.hh"
#include "KTSlot.hh"
#include "KTData.hh"

#include <set>
#include <vector>


namespace Katydid
{

    /*!
     @class KTDBSCANNoiseFiltering
     @author N.S. Oblath

     @brief Noise filtering of sparse spectrograms using the DBSCAN algorithm

     @details
     Normalization of the axes:
     The DBSCAN algorithm expects expects that all of the dimensions that describe a points will have the same scale,
     such that a single radius parameter can describe a sphere in the parameter space that's used to cluster points together.

     Configuration name: "dbscan-noise-filtering"

     Available configuration values:
     - "radius": double -- double used to define the circle around points to be clustered together
     - "min-points": unsigned int -- minimum number of points required to have a cluster

     Slots:
     - "kd-tree": void (KTDataPtr) -- Performs clustering on a KDTree of data; Requires KTKDTreeData.

     Signals:
     - "kd-tree": void (KTDataPtr) -- Emitted when noise filtering is complete; Guarantees KTKDTreeData
    */

    class KTDBSCANNoiseFiltering : public Nymph::KTProcessor
    {
        public:
            const static unsigned fNDimensions;

            typedef KTDBSCAN< KTKDTreeData::TreeIndex > DBSCAN_KDTree;

        public:
            KTDBSCANNoiseFiltering(const std::string& name = "dbscan-noise-filtering");
            virtual ~KTDBSCANNoiseFiltering();

            bool Configure(const scarab::param_node* node);

            MEMBERVARIABLE(unsigned, MinPoints);
            MEMBERVARIABLE(double, Radius);
            MEMBERVARIABLE_NOSET(unsigned, DataCount);            
            //MEMBERVARIABLEREF(Point, Radii);

        public:
            bool DoFiltering(KTKDTreeData& data);

            void DoFiltering(KTKDTreeData::TreeIndex* treeIndex, KTKDTreeData::SetOfPoints& points);

            //***************
            // Signals
            //***************

        private:
            Nymph::KTSignalData fFilteringDoneSignal;

            //***************
            // Slots
            //***************

        private:
            Nymph::KTSlotDataOneType< KTKDTreeData > fKDTreeSlot;

    };

}
 /* namespace Katydid */
#endif /* KTDBSCANNOISEFILTERING_HH_ */
