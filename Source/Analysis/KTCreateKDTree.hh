/**
 @file KTCreateKDTree.hh
 @brief Contains KTCreateKDTree
 @details Creates a KD-Tree
 @author: N. S. Oblath
 @date: Aug 7, 2014
 */

#ifndef KTCREATEKDTREE_HH_
#define KTCREATEKDTREE_HH_

#include "KTProcessor.hh"

#include "KTKDTreeData.hh"
#include "KTMemberVariable.hh"
#include "KTSlot.hh"


namespace Katydid
{
    class KTDiscriminatedPoints1DData;
    class KTParamNode;
    class KTSliceHeader;

   /*!
     @class KTCreateKDTree
     @author N. S. Oblath

     @brief Creates a KD-Tree

     @details
 
     Available configuration values:
     - "use-neg-freqs": bool -- If true [default], corresponding negative and positive frequency bins are summed; if false, the negative frequency bins are dropped.

     Slots:
     - "disc-1d": void (KTDataPtr) -- Adds points to the KT-Tree; Requires KTDiscriminatedPoints1DData
     - "make-tree": void () -- Creates a tree with the existing set of points; Creates data with KTKDTreeData; Emits signal kd-tree

     Signals:
     - "kd-tree": void (KTDataPtr) emitted upon completion of a KD-Tree; Guarantees KTKDTreeData
    */

    class KTCreateKDTree : public KTProcessor
    {
        public:
            KTCreateKDTree(const std::string& name = "create-kd-tree");
            virtual ~KTCreateKDTree();

            bool Configure(const KTParamNode* node);

            MEMBERVARIABLE(KTKDTreeData::DistanceMethod, DistanceMethod);
            MEMBERVARIABLE(unsigned, MaxLeafSize);

        public:
            bool AddPoints(KTSliceHeader& slHeader, KTDiscriminatedPoints1DData& discPoints);

            bool MakeTree();

        private:
            KTDataPtr fDataPtr;
            KTKDTreeData& fTreeData;

            //***************
            // Signals
            //***************

        private:
            KTSignalData fKDTreeSignal;

            //***************
            // Slots
            //***************

        private:
            KTSlotDataTwoTypes< KTSliceHeader, KTDiscriminatedPoints1DData > fDiscPointsSlot;

            void MakeTreeSlot();

    };

} /* namespace Katydid */
#endif /* KTCREATEKDTREE_HH_ */
