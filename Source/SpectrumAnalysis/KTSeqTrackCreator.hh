/**
 @file KTSeqTrackCreator.hh
 @brief Contains KTSeqTrackCreator
 @details Creates a track
 @author: Christine
 @date: Sep 15, 2016
 */

#ifndef KTSEQTRACKCREATOR_HH_
#define KTSEQTRACKCREATOR_HH_

#include "KTProcessor.hh"

#include "KTKDTreeData.hh"
#include "KTMemberVariable.hh"
#include "KTSlot.hh"



namespace Katydid
{

    class KTKDTreeData;


    class KTSeqTrackCreator : public Nymph::KTProcessor
    {
        public:
            KTSeqTrackCreator(const std::string& name = "seq-clustering");
            virtual ~KTSeqTrackCreator();

            bool Configure(const scarab::param_node* node);

            MEMBERVARIABLE(double, FrequencyRadius);
            MEMBERVARIABLE(int, ComponentDistance);
            MEMBERVARIABLE(unsigned, MinNumberOfBins);

        public:
            bool PointLineAssignment(KTKDTreeData& kdTreeData);
			void LineEventAssignment(KTKDTreeData& kdTreeData);
			void WeightedAverage(KTScoredSpectrumData& slice);
            bool SearchArea(KTScoredSpectrumData& slice);

        private:
            bool LoopOverDisciminatedPoints(KTKDTreeData& kdTreeData, std::vector< KTKDTreeData::Point >& setOfPoints, KTScoredSpectrumData& slice);


            //***************
            // Signals
            //***************

        private:
            Nymph::KTSignalData fSeqTrackSignal;

            //***************
            // Slots
            //***************

        private:
            Nymph::KTSlotDataOneType< KTKDTreeData > fKDTreeSlot;

    };

} /* namespace Katydid */
#endif /* KTSEQTRACKCREATOR_HH_ */
