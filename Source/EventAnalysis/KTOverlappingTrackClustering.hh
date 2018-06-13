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
#include "KTSequentialLineData.hh"
#include "KTDiscriminatedPoint.hh"

#include <vector>
#include <cmath>

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
    KTLOGGER(otchlog, "KTOverlappingTrackClusteringHeader");

    class KTOverlappingTrackClustering : public Nymph::KTPrimaryProcessor
    {
        public:
            KTOverlappingTrackClustering(const std::string& name = "overlapping-track-clustering");
            virtual ~KTOverlappingTrackClustering();

            bool Configure(const scarab::param_node* node);
            bool TakeTrack(KTProcessedTrackData& track);
            bool TakeSeqLineCandidate(KTSequentialLineData& swfCand);
            bool Run();
            const std::set< Nymph::KTDataPtr >& GetCandidates() const;


        private:
            MEMBERVARIABLE(double, MaxTrackWidth);
            MEMBERVARIABLE(double, LargeMaxTrackWidth);
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


        private:
            template<typename TracklikeCandidate>
            bool DoCandidateClustering(std::vector<TracklikeCandidate>& compCands, std::vector<TracklikeCandidate>& newCands);

            template<typename TracklikeCandidate>
            bool FindMatchingCandidates(std::vector<TracklikeCandidate>& compCands, std::vector<TracklikeCandidate>& newCands);

            template<typename TracklikeCandidate>
            bool OverlapClustering(std::vector<TracklikeCandidate>& compCands, std::vector<TracklikeCandidate>& newCands);

            template<typename TracklikeCandidate>
            bool DoTheyOverlap(TracklikeCandidate& track1, TracklikeCandidate& track2);

            const void CombineCandidates(const KTProcessedTrackData& track1, KTProcessedTrackData& track2);
            const void CombineCandidates(const KTSequentialLineData& track1, KTSequentialLineData& track2);
            void EmitCandidates(std::vector<KTProcessedTrackData>& compTracks);
            void EmitCandidates(std::vector<KTSequentialLineData>& compCands);
            const void ProcessNewTrack( KTProcessedTrackData& myNewTrack );


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

    template<typename TracklikeCandidate>
    bool KTOverlappingTrackClustering::DoCandidateClustering(std::vector<TracklikeCandidate>& compCands, std::vector<TracklikeCandidate>& newCands)
    {
        if (! FindMatchingCandidates(compCands, newCands))
        {
            KTERROR(otchlog, "An error occurred while identifying overlapping tracks");
            return false;
        }

        KTDEBUG(otchlog, "Track clustering complete");
        fDoneSignal();

        return true;
    }

    template<typename TracklikeCandidate>
    bool KTOverlappingTrackClustering::FindMatchingCandidates(std::vector<TracklikeCandidate>& compCands, std::vector<TracklikeCandidate>& newCands)
    {
        KTINFO( otchlog, "Finding overlapping candidates" );
        KTDEBUG( otchlog, "FrequencyAcceptance is: "<<fMaxTrackWidth );
        newCands.clear();

        unsigned numberOfCands =compCands.size();
        unsigned numberOfNewCands = newCands.size();



        if (numberOfCands > 1)
        {
            while (numberOfCands!=numberOfNewCands)
            {
                numberOfCands = compCands.size();
                KTDEBUG(otchlog, "Number of candidates to cluster: "<< numberOfCands);
                this->OverlapClustering(compCands, newCands);

                // Update number of tracks
                numberOfNewCands = newCands.size();

                KTDEBUG(otchlog, "Number of candidates after clustering: "<< numberOfNewCands);

                compCands.clear();
                compCands = newCands;
                newCands.clear();
            }
        }

        this->EmitCandidates(compCands);

        return true;
    }


    template<typename TracklikeCandidate>
    bool KTOverlappingTrackClustering::OverlapClustering(std::vector<TracklikeCandidate>& compCands, std::vector<TracklikeCandidate>& newCands)
    {
        bool match = false;
        for (typename std::vector<TracklikeCandidate>::iterator compIt = compCands.begin(); compIt != compCands.end(); ++compIt)
        {
            match = false;
            for (typename std::vector<TracklikeCandidate>::iterator newIt = newCands.begin(); newIt != newCands.end(); ++newIt)
            {
                if (this->DoTheyOverlap(*compIt, *newIt))
                {
                    match = true;
                    KTDEBUG(otchlog, "Found overlapping candidates")
                    this->CombineCandidates(*compIt, *newIt);
                    break;
                }

                // since there appear to be real tracks crossing each other this check is removed here
                /*if (this->DoTheyCross(*compIt, *newIt))
                {
                    match = true;
                    KTDEBUG(otchlog, "Found crossing tracks")
                    this->CombineTracks(*compIt, *newIt);
                    break;
                }*/
            }

            if (match == false)
            {
                //T newTrack(*compIt);
                newCands.push_back(*compIt);
            }
        }
        return true;
    }

    template<typename TracklikeCandidate>
    bool KTOverlappingTrackClustering::DoTheyOverlap(TracklikeCandidate& track1, TracklikeCandidate& track2)
    {
        // if there are two tracks that should be just one, any point of the track will be close to the other track (extrapolated)
        // therefore it is enough to check start and endpoint of a track. one should overlap in time, both should be close in frequency

        // if the start time of track 2 is between start and end time of track 1
        bool condition1 = track2.GetStartTimeInRunC() < track1.GetEndTimeInRunC() and track2.GetStartTimeInRunC() >= track1.GetStartTimeInRunC();

        // and the start frequency of track 2 is too close to track 1
        bool condition2 = std::abs(track2.GetStartFrequency() - (track1.GetStartFrequency() + track1.GetSlope() * (track2.GetStartTimeInRunC() - track1.GetStartTimeInRunC()))) < fMaxTrackWidth;

        // same for endpoint
        bool condition3 = std::abs(track2.GetEndFrequency() - (track1.GetStartFrequency() + track1.GetSlope() * (track2.GetEndTimeInRunC() - track1.GetStartTimeInRunC()))) < fLargeMaxTrackWidth;

        if (condition1 and condition2 and condition3)
        {
            return true;
        }
        // the other way around
        bool condition4 = track1.GetStartTimeInRunC() < track2.GetEndTimeInRunC() and track1.GetStartTimeInRunC() >= track2.GetStartTimeInRunC();
        bool condition5 = std::abs(track1.GetStartFrequency() - (track2.GetStartFrequency() + track2.GetSlope() * (track1.GetStartTimeInRunC() - track2.GetStartTimeInRunC()))) < fMaxTrackWidth;
        bool condition6 = std::abs(track1.GetEndFrequency() - (track2.GetStartFrequency() + track2.GetSlope() * (track1.GetEndTimeInRunC() - track2.GetStartTimeInRunC()))) < fLargeMaxTrackWidth;

        if (condition4 and condition5 and condition6)
        {
            return true;
        }
        // same for end point of track2
        condition1 = track2.GetEndTimeInRunC() <= track1.GetEndTimeInRunC() and track2.GetEndTimeInRunC() > track1.GetStartTimeInRunC();
        condition2 = std::abs(track2.GetEndFrequency() - (track1.GetStartFrequency() + track1.GetSlope() * (track2.GetEndTimeInRunC() - track1.GetStartTimeInRunC()))) < fMaxTrackWidth;
        condition3 = std::abs(track2.GetStartFrequency() - (track1.GetStartFrequency() + track1.GetSlope() * (track2.GetStartTimeInRunC() - track1.GetStartTimeInRunC()))) < fLargeMaxTrackWidth;

        if (condition1 and condition2 and condition3)
        {
            return true;
        }
        // the other way around
        condition4 = track1.GetEndTimeInRunC() <= track2.GetEndTimeInRunC() and track1.GetEndTimeInRunC() > track2.GetStartTimeInRunC();
        condition5 = std::abs(track1.GetEndFrequency() - (track2.GetStartFrequency() + track2.GetSlope() * (track1.GetEndTimeInRunC() - track2.GetStartTimeInRunC()))) < fMaxTrackWidth;
        condition6 = std::abs(track1.GetStartFrequency() - (track2.GetStartFrequency() + track2.GetSlope() * (track1.GetStartTimeInRunC() - track2.GetStartTimeInRunC()))) < fLargeMaxTrackWidth;

        if (condition4 and condition5 and condition6)
        {
            return true;
        }

        return false;
    }

}
 /* namespace Katydid */
#endif /* KTOVERLAPPINGTRACKCLUSTERING_HH_ */
