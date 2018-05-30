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

     @brief merges track segments until number of tracks stops decreasing

     @details
     Checks whether track start/ends match another track's extrapolation.
     Can work with KTProcessedTracksData or KTSparseWaterfallCandidateData

     Configuration name: "iterative-track-clustering"

     Available configuration values:
     - "time-gap-tolerance": maximum time gap between tracks
     - "frequency-acceptance": maximum allowed distance from the slope extrapolation
     - "max-track-width": radius around a track where there cannot be another track
     - "apply-power-cut": default false; if true, the summed-power has to be > total-power-threshold; uses fNeighborhoodAmplitude
     - "apply-point-density-cut": default false; if true, the summed-power/time-length has to be > average-power-threshold; uses fNeighborhoodAmplitude
     - "apply-total-snr-cut": default false; if true, the summed-snr has to be > total-snr-threshold; uses fNeighborhoodAmplitude
     - "apply-average-snr-cut": default false; if true, the summed-snr/time-length has to be > average-snr-threshold; uses fNeighborhoodAmplitude
     - "apply-total-residual-cut: default false; if true, the summed-unitless-residual has to be > total-residual-threshold; uses fNeighborhoodAmplitude
     - "apply-average-residual-cut: default false; if true, the summed-unitless-residual/time-length has to be > average-residual-threshold; uses fNeighborhoodAmplitude
     - "total-power-threshold": threshold for apply-total-power-cut
     - "average-power-threshold": threshold for apply-average-power-cut
     - "total-snr-threshold": threshold for apply-total-snr-cut
     - "average-snr-threshold": threshold for apply-average-snr-cut
     - "total-residual-threshold": threshold for apply-total-residual-cut
     - "average-residual-threshold": threshold for apply-average-residual

     Slots:
     - "track": Collects incoming KTProcessedTrackData objects. Clustering will produces new data pointer with KTProcessedTrackData
     - "swf-cand": Collects incoming KTSparseWaterfallCandidateData objects. Clustering will produced new data pointer with KTSparseWaterfallCandidateData
     - "do-clustering": Triggers clustering algorithm

     Signals:
     - "track": void (shared_ptr<KTData>) -- Created and emitted for each group found; Guarantees KTProcessedTrackData.
     - "swf-cand: void (shared_ptr<KTData) -- Created and emitted for each group found; Guarantees KTSparseWaterfallCandidateData.
     - "clustering-done": void () -- Emitted when clustering is complete
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
