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
    class KTEggHeader;
    class KTProcessedTrackData;
    class KTSliceHeader;
    class KTSparseWaterfallCandidateData;

   /*!
     @class KTCreateKDTree
     @author N. S. Oblath

     @brief Creates a KD-Tree

     @details
 
     Notes on setting the time and frequency radius:
     These can be set by the egg header using the "header" slot if they haven't been set already.
     To accomplish this, each of those variables has an extra boolean flag that indicates whether it's been set with the corresponding Set function in the life of the class.
     The Set functions have an extra boolean parameter that can, if true, override the has-been-set flag.
     For normal Set-function operation, where you expect the value to change when you call the Set function, leave the boolean parameter as its default, true.

     Configuration name: "create-kd-tree"

     Available configuration values:
     - "window-size": unsigned --
     - "window-overlap": unsigned --
     - "distance-method": string -- Method used to calculate distances between points; Available options are "manhattan" and "euclidean"
     - "max-leaf-size": unsigned -- Maximum number of points to assign to each leaf node of the k-d tree. Typically should be 10-50. See https://github.com/jlblancoc/nanoflann#21-kdtreesingleindexadaptorparamsleaf_max_size for more details.
     - "time-radius:" double -- Scaling applied to the time axis before adding the point to the tree. Scaled coordinate value = coordinate value / scaling. Using this will prevent it from being set by the egg header.
     - "freq-radius:" double -- Scaling applied to the frequency axis before adding the point to the tree. Scaled coordinate value = coordinate value / scaling. Using this will prevent it from being set by the egg header.

     Slots:
     - "header": void (Nymph::KTDataPtr) -- Sets the time and frequency radii if they have not already been set; Requires KTEggHeader
     - "disc-1d": void (Nymph::KTDataPtr) -- Adds points to the K-D Tree; Requires KTDiscriminatedPoints1DData and KTSliceHeader
     - "swfc-and-track": void (Nymph::KTDataPtr) -- Adds points to the K-D Tree only if the track is not cut; Requires KTSparseWaterfallCandidateData and KTProcessedTrackData
     - "swfc": void (Nymph::KTDataPtr) -- Adds points to the K-D Tree; Requires KTSparseWaterfallCandidateData
     - "make-tree": void () -- Creates a tree with the existing set of points; Creates data with KTKDTreeData; Emits signal kd-tree
     - "done": void () -- same as "make-tree"; Emits signal kd-tree then signal done

     Signals:
     - "kd-tree": void (Nymph::KTDataPtr) emitted upon completion of a KD-Tree; Guarantees KTKDTreeData
     - "done": void (void) emitted when "done" slot is called to indicate that no more k-d trees will be produced
    */

    class KTCreateKDTree : public Nymph::KTProcessor
    {
        public:
            KTCreateKDTree(const std::string& name = "create-kd-tree");
            virtual ~KTCreateKDTree();

            bool Configure(const scarab::param_node* node);

            bool ReconfigureWithHeader(KTEggHeader& header);

            void SetFreqRadius(double radius, bool overrideSet = true);
            void SetTimeRadius(double radius, bool overrideSet = true);

            MEMBERVARIABLE(unsigned, WindowSize);
            MEMBERVARIABLE(unsigned, WindowOverlap);

            MEMBERVARIABLE(KTKDTreeData::DistanceMethod, DistanceMethod);
            MEMBERVARIABLE(unsigned, MaxLeafSize);

            MEMBERVARIABLE_NOSET(double, TimeRadius);
            MEMBERVARIABLE_NOSET(double, FreqRadius);

            MEMBERVARIABLE_NOSET(bool, TimeRadiusAlreadySet);
            MEMBERVARIABLE_NOSET(bool, FreqRadiusAlreadySet);

        private:
            static const unsigned fNDimensions;

        public:
            bool AddPoints(KTSliceHeader& slHeader, KTDiscriminatedPoints1DData& discPoints);
            bool AddPoints(KTSparseWaterfallCandidateData& swfcData, KTProcessedTrackData& ptData);
            bool AddPoints(KTSparseWaterfallCandidateData& swfcData);

            bool MakeTree(bool willContinue);

            bool ClearTree(bool willContinue, uint64_t firstSliceKept = std::numeric_limits<uint64_t>::max());

            Nymph::KTDataPtr GetDataPtr() const;
            const KTKDTreeData& GetKDTreeData() const;

        private:
            Nymph::KTDataPtr fDataPtr;
            KTKDTreeData& fTreeData;

            unsigned fSliceInWindowCount;

            MEMBERVARIABLE(double, InvScalingX);
            MEMBERVARIABLE(double, InvScalingY);

            MEMBERVARIABLE(bool, HaveNewData);

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
            Nymph::KTSlotDataOneType< KTEggHeader > fHeaderSlot;
            Nymph::KTSlotDataTwoTypes< KTSliceHeader, KTDiscriminatedPoints1DData > fDiscPointsSlot;
            Nymph::KTSlotDataTwoTypes< KTSparseWaterfallCandidateData, KTProcessedTrackData > fSWFCAndPTSlot;
            Nymph::KTSlotDataOneType< KTSparseWaterfallCandidateData > fSWFCSlot;
            Nymph::KTSlotDone fDoneSlot;

            void MakeTreeSlot();

    };

    inline void KTCreateKDTree::SetTimeRadius(double radius, bool overrideSet)
    {
        if (overrideSet || ! fTimeRadiusAlreadySet)
        {
            fTimeRadius = radius;
            fInvScalingX = 1. / fTimeRadius;
            fTimeRadiusAlreadySet = true;
        }
        return;
    }

    inline void KTCreateKDTree::SetFreqRadius(double radius, bool overrideSet)
    {
        if (overrideSet || ! fFreqRadiusAlreadySet)
        {
            fFreqRadius = radius;
            fInvScalingY = 1. / fFreqRadius;
            fFreqRadiusAlreadySet = true;
        }
        return;
    }

    inline Nymph::KTDataPtr KTCreateKDTree::GetDataPtr() const
    {
        return fDataPtr;
    }

    inline const KTKDTreeData& KTCreateKDTree::GetKDTreeData() const
    {
        return fTreeData;
    }

} /* namespace Katydid */
#endif /* KTCREATEKDTREE_HH_ */
