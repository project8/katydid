/**
 @file KTConsensusThresholding.hh
 @brief Contains KTConsensusThresholding
 @details Creates a KD-Tree
 @author: N. S. Oblath
 @date: Aug 7, 2014
 */

#ifndef KTCONSENSUSTHRESHOLDING_HH_
#define KTCONSENSUSTHRESHOLDING_HH_

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
     @class KTConsensusThresholding
     @author N. S. Oblath

     @brief Filters sparse-waterfall data with the Consensus Thresholding algorithm

     @details
 
     Configuration name: "consensus-thresholding"

     Available configuration values:
     - "use-neg-freqs": bool -- If true [default], corresponding negative and positive frequency bins are summed; if false, the negative frequency bins are dropped.

     Slots:
     - "disc-1d": void (KTDataPtr) -- Adds points to the KT-Tree; Requires KTDiscriminatedPoints1DData
     - "make-tree": void () -- Creates a tree with the existing set of points; Creates data with KTKDTreeData; Emits signal kd-tree

     Signals:
     - "kd-tree": void (KTDataPtr) emitted upon completion of a KD-Tree; Guarantees KTKDTreeData
    */

    class KTConsensusThresholding : public KTProcessor
    {
        public:
            KTConsensusThresholding(const std::string& name = "consensus-thresholding");
            virtual ~KTConsensusThresholding();

            bool Configure(const KTParamNode* node);

            bool ConsensusVote(KTKDTreeData& kdTreeData);
            std::vector< size_t > ConsensusVoteComponent(const KTTreeIndex< double >* kdTree, const std::vector< KTKDTreeData::Point >& setOfPoints);
            void VoteCore(bool doPositive, size_t pid, double* thisPoint, double* neighborPoint, double slope, double intercept);

            //MEMBERVARIABLE(KTKDTreeData::DistanceMethod, DistanceMethod);
            //MEMBERVARIABLE(unsigned, MaxLeafSize);
            MEMBERVARIABLE(double, MembershipRadius);
            MEMBERVARIABLE(unsigned, MinNumberVotes);

        public:


            //***************
            // Signals
            //***************

        private:
            KTSignalData fKDTreeSignal;

            //***************
            // Slots
            //***************

        private:
            KTSlotDataOneType< KTKDTreeData > fKDTreeSlot;

    };

} /* namespace Katydid */
#endif /* KTCONSENSUSTHRESHOLDING_HH_ */
