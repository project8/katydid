/**
 @file KTMergeKDTree.hh
 @brief Contains KTMergeKDTree
 @details Merges k-d trees
 @author: N. S. Oblath
 @date: Aug 18, 2014
 */

#ifndef KTMERGEKDTREE_HH_
#define KTMERGEKDTREE_HH_

#include "KTProcessor.hh"

#include "KTKDTreeData.hh"
#include "KTSlot.hh"


namespace Katydid
{
    
    class KTKDTreeData;

   /*!
     @class KTMergeKDTree
     @author N. S. Oblath

     @brief Merges k-d trees together

     @details
 
     Configuration name: "merge-kd-tree"

     Available configuration values:

     Slots:
     - "kd-tree-in": void (Nymph::KTDataPtr) -- Merges the input data into the output tree; Requires KTKDTreeData
     - "finish-tree": void () -- Triggers finishing of existing tree data; emits signal "kd-tree-out"
     - "done": void () -- Finishes any existing tree data; emits signal "kd-tree-out", then signal "done"

     Signals:
     - "kd-tree-out": void (Nymph::KTDataPtr) emitted upon completion of a merged k-d tree
     - "done" void () emitted upon completion of all processing, as indicated by the "done" slot
    */

    class KTMergeKDTree : public Nymph::KTProcessor
    {
        public:
            KTMergeKDTree(const std::string& name = "merge-kd-tree");
            virtual ~KTMergeKDTree();

            bool Configure(const scarab::param_node* node);

        public:
            bool MergeTree(KTKDTreeData& kdTreeData);

            bool MergeTree(const KTKDTreeData::SetOfPoints& setOfPoints, KTKDTreeData::DistanceMethod, unsigned maxLeafSize, unsigned component);

            bool FinishTree();

            bool ClearTree();

            Nymph::KTDataPtr GetDataPtr() const;
            const KTKDTreeData& GetKDTreeData() const;

            MEMBERVARIABLE(bool, HaveNewData);

        private:
            Nymph::KTDataPtr fDataPtr;
            KTKDTreeData& fMergedTreeData;

            //***************
            // Signals
            //***************

        private:
            Nymph::KTSignalData fKDTreeSignal;
            Nymph::KTSignalDone fDoneSignal;

            //***************
            // Slots
            //***************

        private:
            Nymph::KTSlotDataOneType< KTKDTreeData > fKDTreeSlot;
            Nymph::KTSlotDone fDoneSlot;

            void FinishTreeSlot();
    };

    inline Nymph::KTDataPtr KTMergeKDTree::GetDataPtr() const
    {
        return fDataPtr;
    }

    inline const KTKDTreeData& KTMergeKDTree::GetKDTreeData() const
    {
        return fMergedTreeData;
    }
} /* namespace Katydid */
#endif /* KTMERGEKDTREE_HH_ */
