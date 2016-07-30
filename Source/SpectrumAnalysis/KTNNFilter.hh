/**
 @file KTNNFilter.hh
 @brief Contains KTNNFilter
 @details Filters points from a K-D tree using nearest-neighbor attributes
 @author: N. S. Oblath
 @date: Sept 2, 2014
 */

#ifndef KTNNFILTER_HH_
#define KTNNFILTER_HH_

#include "KTProcessor.hh"

#include "KTKDTreeData.hh"
#include "KTMemberVariable.hh"
#include "KTSlot.hh"


namespace Katydid
{
    using namespace Nymph;
    class KTKDTreeData;

   /*!
     @class KTNNFilter
     @author N. S. Oblath

     @brief Filters sparse-waterfall data with the nearest-neighbor attributes

     @details
     There are two filtering options:
     1) Filter points by maximum nearest-neighbor distance.  If a point's nearest neighbor is farther than fMaxDist, then it is flagged/removed as noise.
     2) Filter points by minimum within a radius. If a point has too few neighbors within fRadius, then it is flagged/removed as noise.
 
     Configuration name: "nn-filter"

     Available configuration values:
     - "nn-max-dist": double -- Distance used as the nearest-neighbor maximum for filtering option (1).
     - "radius": double -- Radius used to count neighbors for filtering option (2).
     - "min-in-radius": unsigned -- Minimum number of neighbors required for filtering option (2).
     - "remove-noise": bool -- Flag that determines whether noise points are removed (true) or flagged (false; default)

     Slots:
     - "kdt-nn": void (KTDataPtr) -- Filters k-d tree data based on the maximum nearest-neighbor distance; Requires KTKDTreeData; existing data is modified
     - "kdt-rad": void (KTDataPtr) -- Filters k-d tree data based on the minimum number of neighbors within a radius; Requires KTKDTreeData; existing data is modified

     Signals:
     - "kd-tree-out": void (KTDataPtr) emitted upon completion of the filtering
    */

    class KTNNFilter : public KTProcessor
    {
        public:
            KTNNFilter(const std::string& name = "nn-filter");
            virtual ~KTNNFilter();

            bool Configure(const scarab::param_node* node);

            MEMBERVARIABLE(double, MaxDist);
            MEMBERVARIABLE(double, Radius);
            MEMBERVARIABLE(unsigned, MinInRadius);
            MEMBERVARIABLE(bool, RemoveNoiseFlag);

        public:
            bool FilterByNNDist(KTKDTreeData& kdTreeData);
            bool FilterByMinInRadius(KTKDTreeData& kdTreeData);

            bool FilterByNNDist(const KTTreeIndex< double >* kdTree, const KTKDTreeData::SetOfPoints& setOfPoints, std::vector< size_t >& noisePoints);
            bool FilterByMinInRadius(const KTTreeIndex< double >* kdTree, const KTKDTreeData::SetOfPoints& setOfPoints, std::vector< size_t >& noisePoints);

        private:

            //***************
            // Signals
            //***************

        private:
            KTSignalData fKDTreeSignal;

            //***************
            // Slots
            //***************

        private:
            KTSlotDataOneType< KTKDTreeData > fKDTreeNNSlot;
            KTSlotDataOneType< KTKDTreeData > fKDTreeRadiusSlot;

    };

} /* namespace Katydid */
#endif /* KTNNFILTER_HH_ */
