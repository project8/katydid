/**
 @file KTOverlappingTrackClustering.hh
 @brief Contains KTOverlappingTrackClustering
 @details Groups overlapping or crossing tracks into one
 @author: C. Claessens
 @date: August 7, 2017
 */

#ifndef KTOVERLAPPINGTRACKCLUSTERING_HH_
#define KTOVERLAPPINGTRACKCLUSTERING_HH_

#include "KTPrimaryProcessor.hh"

#include "KTSlot.hh"
#include "KTData.hh"
#include "KTMemberVariable.hh"
#include "KTProcessedTrackData.hh"
#include "KTSeqLine.hh"

#include <vector>

namespace Katydid
{

    /*!
     @class KTOverlappingLineClustering
     @author C. Claessens

     @brief Clusters tracks together until number of tracks stops decreasing.

     @details
     Checks whether tracks start/ends are very close to another track or whether tracks cross.
     Can work with KTProcessedTrackData or KTSequentialLineData

     Configuration name: "overlapping-track-clustering"

     Available configuration values:
     - "max-track-width": tracks that are not further apart than this value in frequency will be grouped together to a combined track
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
     - "swf-cand": Collects incoming KTSequentialLineData objects. Clustering will produced new data pointer with KTSequentialLineData
     - "do-clustering": Triggers clustering algorithm

     Signals:
     - "track": void (shared_ptr<KTData>) -- Created and emitted for each group found; Guarantees KTProcessedTrackData.
     - "swf-cand: void (shared_ptr<KTData) -- Created and emitted for each group found; Guarantees KTSequentialLineData.
     - "clustering-done": void () -- Emitted when clustering is complete
    */

    class KTOverlappingTrackClustering : public Nymph::KTPrimaryProcessor
    {
        public:
            KTOverlappingTrackClustering(const std::string& name = "overlapping-track-clustering");
            virtual ~KTOverlappingTrackClustering();

            bool Configure(const scarab::param_node* node);

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
            bool TakeSeqLineCandidate(KTSequentialLineData& swfCand);
            //void SetNComponents(unsigned nComps);
            bool DoTrackClustering();
            bool DoSeqLineClustering();
            bool Run();
            const std::set< Nymph::KTDataPtr >& GetCandidates() const;


        private:
            bool OverlapClustering();
            bool OverlapSeqLineClustering();
            bool DoTheyOverlap(KTProcessedTrackData& track1, KTProcessedTrackData& track2);
            bool DoTheyOverlap(KTSequentialLineData& track1, KTSequentialLineData& track2);
            //bool DoTheyCross(KTProcessedTrackData& track1, KTProcessedTrackData& track2);
            const void CombineTracks(const KTProcessedTrackData& track1, KTProcessedTrackData& track2);
            const void CombineSeqLineCandidates(const KTSequentialLineData& track1, KTSequentialLineData& track2);
            bool FindMatchingTracks();
            bool FindMatchingSeqLineCands();
            void EmitTrackCandidates();
            void EmitSeqLineCandidates();
            const void ProcessNewTrack( KTProcessedTrackData& myNewTrack );

            //std::vector< TrackSet > fCompTracks; // input tracks
            //std::vector< TrackSet> fNewTracks;

            std::vector<KTProcessedTrackData> fCompTracks;
            std::vector<KTProcessedTrackData> fNewTracks;

            std::vector<KTSequentialLineData> fCompSeqLineCands;
            std::vector<KTSequentialLineData> fNewSeqLineCands;

            std::set< Nymph::KTDataPtr > fCandidates;

            //***************
            // Signals
            //***************

        private:
            Nymph::KTSignalData fTrackSignal;
            Nymph::KTSignalData fSeqLineCandSignal;
            Nymph::KTSignalOneArg< void > fDoneSignal;

            //***************
            // Slots
            //***************

        private:
            Nymph::KTSlotDataOneType< KTProcessedTrackData > fTakeTrackSlot;
            Nymph::KTSlotDataOneType< KTSequentialLineData > fTakeSeqLineCandSlot;

            void DoClusteringSlot();

    };
    inline const std::set< Nymph::KTDataPtr >& KTOverlappingTrackClustering::GetCandidates() const
    {
        return fCandidates;
    }

}
 /* namespace Katydid */
#endif /* KTOVERLAPPINGTRACKCLUSTERING_HH_ */
