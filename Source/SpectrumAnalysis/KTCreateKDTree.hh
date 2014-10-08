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

#include <limits>

namespace Katydid
{
    class KTDiscriminatedPoints1DData;
    class KTParamNode;
    class KTProcessedTrackData;
    class KTSliceHeader;
    class KTSparseWaterfallCandidateData;

   /*!
     @class KTCreateKDTree
     @author N. S. Oblath

     @brief Creates a KD-Tree

     @details
 
     Configuration name: "create-kd-tree"

     Available configuration values:
     - "window-size": unsigned --
     - "window-overlap": unsigned --
     - "distance-method": string -- Method used to calculate distances between points; Available options are "manhattan" and "euclidean"
     - "max-leaf-size": unsigned -- Maximum number of points to assign to each leaf node of the k-d tree. Typically should be 10-50. See https://github.com/jlblancoc/nanoflann#21-kdtreesingleindexadaptorparamsleaf_max_size for more details.
     - "time-radius:" double -- Scaling applied to the time axis before adding the point to the tree. Scaled coordinate value = coordinate value / scaling
     - "freq-radius:" double -- Scaling applied to the frequency axis before adding the point to the tree. Scaled coordinate value = coordinate value / scaling

     Slots:
     - "disc-1d": void (KTDataPtr) -- Adds points to the K-D Tree; Requires KTDiscriminatedPoints1DData and KTSliceHeader
     - "swfc-and-track": void (KTDataPtr) -- Adds points to the K-D Tree only if the track is not cut; Requires KTSparseWaterfallCandidateData and KTProcessedTrackData
     - "swfc": void (KTDataPtr) -- Adds points to the K-D Tree; Requires KTSparseWaterfallCandidateData
     - "make-tree": void () -- Creates a tree with the existing set of points; Creates data with KTKDTreeData; Emits signal kd-tree
     - "done": void () -- same as "make-tree"; Emits signal kd-tree then signal done

     Signals:
     - "kd-tree": void (KTDataPtr) emitted upon completion of a KD-Tree; Guarantees KTKDTreeData
     - "done": void (void) emitted when "done" slot is called to indicate that no more k-d trees will be produced
    */

    class KTCreateKDTree : public KTProcessor
    {
        public:
            KTCreateKDTree(const std::string& name = "create-kd-tree");
            virtual ~KTCreateKDTree();

            bool Configure(const KTParamNode* node);

            MEMBERVARIABLE(unsigned, WindowSize);
            MEMBERVARIABLE(unsigned, WindowOverlap);

            MEMBERVARIABLE(KTKDTreeData::DistanceMethod, DistanceMethod);
            MEMBERVARIABLE(unsigned, MaxLeafSize);

            MEMBERVARIABLE_NOSET(double, TimeRadius);
            void SetTimeRadius(double radius);
            MEMBERVARIABLE_NOSET(double, FreqRadius);
            void SetFreqRadius(double radius);

            static const unsigned fNDimensions;

        public:
            bool AddPoints(KTSliceHeader& slHeader, KTDiscriminatedPoints1DData& discPoints);
            bool AddPoints(KTSparseWaterfallCandidateData& swfcData, KTProcessedTrackData& ptData);
            bool AddPoints(KTSparseWaterfallCandidateData& swfcData);

            bool MakeTree(bool willContinue);

            bool ClearTree(bool willContinue, uint64_t firstSliceKept = std::numeric_limits<uint64_t>::max());

            KTDataPtr GetDataPtr() const;
            const KTKDTreeData& GetKDTreeData() const;

        private:
            KTDataPtr fDataPtr;
            KTKDTreeData& fTreeData;

            unsigned fSliceInWindowCount;

            MEMBERVARIABLE(double, InvScalingX);
            MEMBERVARIABLE(double, InvScalingY);

            MEMBERVARIABLE(bool, HaveNewData);

            //***************
            // Signals
            //***************

        private:
            KTSignalData fKDTreeSignal;
            KTSignalDone fDoneSignal;

            //***************
            // Slots
            //***************

        private:
            KTSlotDataTwoTypes< KTSliceHeader, KTDiscriminatedPoints1DData > fDiscPointsSlot;
            KTSlotDataTwoTypes< KTSparseWaterfallCandidateData, KTProcessedTrackData > fSWFCAndPTSlot;
            KTSlotDataOneType< KTSparseWaterfallCandidateData > fSWFCSlot;
            KTSlotDone fDoneSlot;

            void MakeTreeSlot();

    };

    inline void KTCreateKDTree::SetTimeRadius(double radius)
    {
        fTimeRadius = radius;
        fInvScalingX = 1. / fTimeRadius;
        return;
    }

    inline void KTCreateKDTree::SetFreqRadius(double radius)
    {
        fFreqRadius = radius;
        fInvScalingY = 1. / fFreqRadius;
        return;
    }

    inline KTDataPtr KTCreateKDTree::GetDataPtr() const
    {
        return fDataPtr;
    }

    inline const KTKDTreeData& KTCreateKDTree::GetKDTreeData() const
    {
        return fTreeData;
    }

} /* namespace Katydid */
#endif /* KTCREATEKDTREE_HH_ */
