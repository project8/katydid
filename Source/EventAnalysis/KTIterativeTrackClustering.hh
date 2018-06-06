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
#include "KTSequentialLineData.hh"
#include "KTDiscriminatedPoint.hh"

#include <vector>

namespace Katydid
{
    
    /*!
     @class KTIterativeLineClustering
     @author C. Claessens

     @brief merges track segments until number of tracks stops decreasing

     @details
     Checks whether track start/ends match another track's extrapolation.
     Can work with KTProcessedTracksData or KTSequentialLineData

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
     - "sql-cand": Collects incoming KTSequentialLineData objects. Clustering will produced new data pointer with KTSequentialLineData
     - "do-clustering": Triggers clustering algorithm

     Signals:
     - "track": void (shared_ptr<KTData>) -- Created and emitted for each group found; Guarantees KTProcessedTrackData.
     - "sql-cand: void (shared_ptr<KTData) -- Created and emitted for each group found; Guarantees KTSequentialLineData.
     - "clustering-done": void () -- Emitted when clustering is complete
    */

    KTLOGGER(itchlog, "KTIterativeTrackClustering");

    class KTIterativeTrackClustering : public Nymph::KTPrimaryProcessor
    {
        public:
            KTIterativeTrackClustering(const std::string& name = "iterative-track-clustering");
            virtual ~KTIterativeTrackClustering();

            bool Configure(const scarab::param_node* node);
            bool TakeTrack(KTProcessedTrackData& track);
            bool TakeSeqLineCandidate(KTSequentialLineData& SeqLineCand);
            bool Run();
            const std::set< Nymph::KTDataPtr >& GetCandidates() const;


        private:
            MEMBERVARIABLE(double, TimeGapTolerance);
            MEMBERVARIABLE(double, FrequencyAcceptance);
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
            bool DoCandidateClustering(std::vector<TracklikeCandidate> compCands, std::vector<TracklikeCandidate> newCands)
            {
                if (! FindMatchingCandidates(compCands, newCands))
                {
                    KTERROR(itchlog, "An error occurred while identifying extrapolated tracks");
                    return false;
                }

                KTDEBUG(itchlog, "Candidate building complete");
                fDoneSignal();

                return true;
            }

            template<typename TracklikeCandidate>
            bool FindMatchingCandidates(std::vector<TracklikeCandidate> compCands, std::vector<TracklikeCandidate> newCands)
            {
                KTINFO(itchlog, "Finding extrapolated candidates");
                KTDEBUG(itchlog, "TimeGapTolerance FrequencyAcceptance and MaxTrackWidth are: "<<fTimeGapTolerance<< " "<<fFrequencyAcceptance<< " "<<fMaxTrackWidth);
                newCands.clear();

                unsigned numberOfCandidates = compCands.size();
                unsigned numberOfNewCandidates = newCands.size();


                if (numberOfCandidates > 1)
                {
                    while (numberOfCandidates!=numberOfNewCandidates)
                    {
                        numberOfCandidates = compCands.size();
                        KTDEBUG(itchlog, "Number of candidates to cluster: "<< numberOfCandidates);
                        this->ExtrapolateClustering(compCands, newCands);

                        // Update number of tracks
                        numberOfNewCandidates = newCands.size();

                        KTDEBUG(itchlog, "Number of candidates after clustering: "<< numberOfNewCandidates);

                        compCands.clear();
                        compCands = newCands;
                        newCands.clear();
                    }
                }

                this->EmitCandidates(compCands);

                return true;
            }

            template<typename TracklikeCandidate>
            bool ExtrapolateClustering(std::vector<TracklikeCandidate>& compCands, std::vector<TracklikeCandidate>& newCands)
            {
                bool match = false;
                for (typename std::vector<TracklikeCandidate>::iterator compIt = compCands.begin(); compIt != compCands.end(); ++compIt)
                {
                    match = false;
                    for (typename std::vector<TracklikeCandidate>::iterator newIt = newCands.begin(); newIt != newCands.end(); ++newIt)
                    {
                        if (this->DoTheyMatch(*compIt, *newIt))
                        {
                            match = true;
                            KTDEBUG(itchlog, "Found matching candidates");
                            this->CombineCandidates(*compIt, *newIt);
                            break;
                        }
                        // it is possible that the segments that get combined first are not direct neighbors in time
                        // in that case there can be a track segment very close to an already combined track
                        if (this->DoTheyOverlap(*compIt, *newIt))
                        {
                            match = true;
                            KTDEBUG(itchlog, "Found overlapping candidates");
                            this->CombineCandidates(*compIt, *newIt);
                            break;
                        }
                    }

                    if (match == false)
                    {
                        newCands.push_back(*compIt);
                    }
                }
                return true;
            }

            template<typename TracklikeCandidate>
            bool DoTheyMatch(TracklikeCandidate& track1, TracklikeCandidate& track2)
            {
                bool slopeCondition1 = std::abs(track1.GetEndFrequency()+track1.GetSlope()*(track2.GetStartTimeInRunC()-track1.GetEndTimeInRunC()) - track2.GetStartFrequency())<fFrequencyAcceptance;
                bool slopeCondition2 = std::abs(track2.GetStartFrequency()-track2.GetSlope()*(track2.GetStartTimeInRunC()-track1.GetEndTimeInRunC()) - track1.GetEndFrequency())<fFrequencyAcceptance;
                bool timeGapInLine = track1.GetEndTimeInRunC() <= track2.GetStartTimeInRunC();
                bool gapSmallerThanLimit = std::abs(track2.GetStartTimeInRunC() - track1.GetEndTimeInRunC())<fTimeGapTolerance;

                if (timeGapInLine and gapSmallerThanLimit and (slopeCondition1 or slopeCondition2))
                {
                    return true;
                }

                slopeCondition1 = std::abs(track2.GetEndFrequency()+track2.GetSlope()*(track1.GetStartTimeInRunC()-track2.GetEndTimeInRunC()) - track1.GetStartFrequency())<fFrequencyAcceptance;
                slopeCondition2 = std::abs(track1.GetStartFrequency()-track1.GetSlope()*(track1.GetStartTimeInRunC()-track2.GetEndTimeInRunC()) - track2.GetEndFrequency())<fFrequencyAcceptance;
                timeGapInLine = track2.GetEndTimeInRunC() <= track1.GetStartTimeInRunC();
                gapSmallerThanLimit = std::abs(track1.GetStartTimeInRunC() - track2.GetEndTimeInRunC())<fTimeGapTolerance;

                if (timeGapInLine and gapSmallerThanLimit and (slopeCondition1 or slopeCondition2))
                {
                    return true;
                }
            return false;
            }

            template<typename TracklikeCandidate>
            bool DoTheyOverlap(TracklikeCandidate& track1, TracklikeCandidate& track2)
            {
                // if the start time of track 2 is between start and end time of track 1
                bool condition1 = track2.GetStartTimeInRunC() < track1.GetEndTimeInRunC() and track2.GetStartTimeInRunC() >= track1.GetStartTimeInRunC();

                // and the start and end frequency of track 2 are close to track 1 (or an extrapolated track 1)
                bool condition2 = std::abs(track2.GetStartFrequency() - (track1.GetStartFrequency() + track1.GetSlope() * (track2.GetStartTimeInRunC() - track1.GetStartTimeInRunC()))) < fMaxTrackWidth;

                // and the other end is nearby too
                bool condition3 = std::abs(track2.GetEndFrequency() - (track1.GetStartFrequency() + track1.GetSlope() * (track2.GetEndTimeInRunC() - track1.GetStartTimeInRunC()))) < fLargeMaxTrackWidth;
                // This condition doesn't need to be as strict and just makes sure this isn't a new track after all (instead one could compare slopes)

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

                // same for endpoints overlapping in time
                condition1 = track2.GetEndTimeInRunC() <= track1.GetEndTimeInRunC() and track2.GetEndTimeInRunC() > track1.GetStartTimeInRunC();
                condition2 = std::abs(track2.GetEndFrequency() - (track1.GetStartFrequency() + track1.GetSlope() * (track2.GetEndTimeInRunC() - track1.GetStartTimeInRunC()))) < fMaxTrackWidth;
                condition3 = std::abs(track2.GetStartFrequency() - (track1.GetStartFrequency() + track1.GetSlope() * (track2.GetStartTimeInRunC() - track1.GetStartTimeInRunC()))) < fLargeMaxTrackWidth;

                if (condition1 and condition2 and condition2)
                {
                    return true;
                }

                // again the other way around
                condition4 = track1.GetEndTimeInRunC() <= track2.GetEndTimeInRunC() and track1.GetEndTimeInRunC() > track2.GetStartTimeInRunC();
                condition5 = std::abs(track1.GetEndFrequency() - (track2.GetStartFrequency() + track2.GetSlope() * (track1.GetEndTimeInRunC() - track2.GetStartTimeInRunC()))) < fMaxTrackWidth;
                condition6 = std::abs(track1.GetStartFrequency() - (track2.GetStartFrequency() + track2.GetSlope() * (track1.GetStartTimeInRunC() - track2.GetStartTimeInRunC()))) < fLargeMaxTrackWidth;

                if (condition4 and condition5 and condition6)
                {
                    return true;
                }
                return false;
            }


        private:
            const void CombineCandidates(const KTProcessedTrackData& track1, KTProcessedTrackData& track2);
            const void CombineCandidates(const KTSequentialLineData& oldSeqLineCand, KTSequentialLineData& newSeqLineCand);
            void EmitCandidates(std::vector<KTProcessedTrackData>& compCands);
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
    inline const std::set< Nymph::KTDataPtr >& KTIterativeTrackClustering::GetCandidates() const
    {
        return fCandidates;
    }
}
 /* namespace Katydid */
#endif /* KTITERATIVETRACKCLUSTERING_HH_ */
