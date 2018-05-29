/**
 @file KTIterativeTrackClustering.hh
 @brief Contains KTIterativeTrackClustering
 @details Groups collinear tracks into one
 @author: C. Claessens
 @date: August 7, 2017
 */

#ifndef KTITERATIVETRACKCLUSTERING_HH_
#define KTITERATIVETRACKCLUSTERING_HH_

#include "KTPrimaryProcessor.hh"

#include "KTSlot.hh"
#include "KTData.hh"
#include "KTMemberVariable.hh"
#include "KTProcessedTrackData.hh"
#include "KTSparseWaterfallCandidateData.hh"

#include <vector>

namespace Katydid
{
    
    /*!
     @class KTIterativeLineClustering
     @author C. Claessens

     @brief merges track segments until number of lines stops decreasing

     @details
     Checks whether track start/ends matches another track's extrapolation

     Configuration name: "iterative-track-clustering"

     Available configuration values:
     - "time-gap-tolerance": maximum time gap between tracks
     - "frequency-acceptance": maximum allowed distance from the slope extrapolation
     - "max-track-width": radius around a track where there cannot be another track
     - "apply-power-cut": default is false
     - "apply-power-density-cut": default is false
     - "power-threshold": total track power must be above this threshold
     - "power-density-threshold": total power per second threshold

     Slots:
     - "track": void (shared_ptr<KTData>) -- If this is a new acquisition; Adds tracks to the internally-stored set of points; Requires KTProcessedTrackData.
     - "do-clustering": void () -- Triggers clustering algorithm

     Signals:
     - "track": void (shared_ptr<KTData>) -- Emitted for each group found; Guarantees KTProcessedTrackData.
     - "clustering-done": void () -- Emitted when track clustering is complete
    */

    class KTIterativeTrackClustering : public Nymph::KTPrimaryProcessor
    {
        public:
            KTIterativeTrackClustering(const std::string& name = "iterative-track-clustering");
            virtual ~KTIterativeTrackClustering();

            bool Configure(const scarab::param_node* node);

            MEMBERVARIABLE(double, TimeGapTolerance);
            MEMBERVARIABLE(double, FrequencyAcceptance);
            MEMBERVARIABLE(double, MaxTrackWidth);
            MEMBERVARIABLE(bool, ApplyTotalPowerCut);
            MEMBERVARIABLE(bool, ApplyAveragePowerCut);
            MEMBERVARIABLE(bool, ApplyTotalSNRCut);
            MEMBERVARIABLE(bool, ApplyAverageSNRCut);
            MEMBERVARIABLE(bool, ApplyTotalUnitlessResidualCut);
            MEMBERVARIABLE(bool, ApplyAverageUnitlessResidualCut);
            MEMBERVARIABLE(double, TotalPowerThreshold);
            MEMBERVARIABLE(double, AveragePowerThreshold);
            MEMBERVARIABLE(double, TotalSNRThreshold);
            MEMBERVARIABLE(double, AverageSNRThreshold);
            MEMBERVARIABLE(double, TotalUnitlessResidualThreshold);
            MEMBERVARIABLE(double, AverageUnitlessResidualThreshold);
            MEMBERVARIABLE(unsigned, NTracks);


        public:
            // Store point information locally
            bool TakeTrack(KTProcessedTrackData& track);
            bool TakeSWFCandidate(KTSparseWaterfallCandidateData& swfCand);

            //void SetNComponents(unsigned nComps);
            bool DoTrackClustering();
            bool DoSWFClustering();
            bool Run();


        private:
            bool ExtrapolateTrackClustering();
            bool ExtrapolateSWFClustering();
            bool DoTheyMatch(KTProcessedTrackData& track1, KTProcessedTrackData& track2);
            bool DoTheyMatch(KTSparseWaterfallCandidateData& track1, KTSparseWaterfallCandidateData& track2);
            bool DoTheyOverlap(KTProcessedTrackData& track1, KTProcessedTrackData& track2);
            bool DoTheyOverlap(KTSparseWaterfallCandidateData& track1, KTSparseWaterfallCandidateData& track2);
            const void CombineTracks(const KTProcessedTrackData& track1, KTProcessedTrackData& track2);
            const void CombineSWFCandidates(const KTSparseWaterfallCandidateData& oldSWFCand, KTSparseWaterfallCandidateData& newSWFCand);
            bool FindMatchingTracks();
            bool FindMatchingSWFCands();
            void EmitTrackCandidates();
            void EmitSWFCandidates();
            const void ProcessNewTrack( KTProcessedTrackData& myNewTrack );


            std::vector<KTProcessedTrackData> fCompTracks;
            std::vector<KTProcessedTrackData> fNewTracks;

            std::vector<KTSparseWaterfallCandidateData> fCompSWFCands;
            std::vector<KTSparseWaterfallCandidateData> fNewSWFCands;


            //***************
            // Signals
            //***************

        private:
            Nymph::KTSignalData fTrackSignal;
            Nymph::KTSignalData fSWFCandSignal;
            Nymph::KTSignalOneArg< void > fDoneSignal;

            //***************
            // Slots
            //***************

        private:
            Nymph::KTSlotDataOneType< KTProcessedTrackData > fTakeTrackSlot;
            Nymph::KTSlotDataOneType< KTSparseWaterfallCandidateData > fTakeSWFCandSlot;

            void DoClusteringSlot();

    };

}
 /* namespace Katydid */
#endif /* KTITERATIVETRACKCLUSTERING_HH_ */
