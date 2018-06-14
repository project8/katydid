/**
 @file KTTrackProcessingWeightedSlope.hh
 @brief Contains KTTrackProcessingWeightedSlope
 @details Extracts physics-relevant information about tracks
 @author: N.S. Oblath, B. LaRoque & M. Guigue
 @date: July 22, 2013
 */

#ifndef KTTrackProcessingWeightedSlope_HH_
#define KTTrackProcessingWeightedSlope_HH_

#include "KTProcessor.hh"

#include "KTMemberVariable.hh"
#include "KTDiscriminatedPoint.hh"
#include "KTProcessedTrackData.hh"

#include "KTSlot.hh"

#include <cmath>


namespace Katydid
{

    class KTSparseWaterfallCandidateData;
    class KTSequentialLineData;

    /*!
     @class KTTrackProcessingWeightedSlope
     @author N.S. Oblath, B. LaRoque & M. Guigue

     @brief Extracts physics-relevant information about tracks using a weighted-slope algorithm

     @details

     Configuration name: "track-proc-ws"

     Available configuration values:
     - "slope-min": double -- Minimum track slope to keep (Hz/s)
     - "min-points": unsigned -- Minimum number of points required to keep a processed track
     - "assigned-error": double -- Error assigned to the points in case of perfectly aligned points

     Slots:
     - "swfc": void (KTDataPr) -- [what it does]; Requires KTSparseWaterfallCandidateData; Adds KTProcessedTrackData; Emits signal "track"
     - "seqc": void (KTDataPr) -- [what it does]; Requires KTSequentialLineData; Adds KTProcessedTrackData; Emits signal "track"

     Signals:
     - "track": void (Nymph::KTDataPtr) -- Emitted when a track has been processed; Guarantees KTProcessedTrackData.
    */

    class KTTrackProcessingWeightedSlope : public Nymph::KTProcessor
    {

        public:
            typedef KTDiscriminatedPoints Points;
            struct TrackID
            {
                unsigned fComponent;
                unsigned fCandidateID;
                unsigned fAcquisitionID;
            };

        public:
            KTTrackProcessingWeightedSlope(const std::string& name = "track-proc");
            virtual ~KTTrackProcessingWeightedSlope();

            bool Configure(const scarab::param_node* node);

        private:

            MEMBERVARIABLE(std::string, TrackProcAlgorithm);
            MEMBERVARIABLE(double, SlopeMinimum);
            MEMBERVARIABLE(unsigned, ProcTrackMinPoints);
            MEMBERVARIABLE(double, ProcTrackAssignedError);

        public:
            template<typename TracklikeCandidate> 
            bool ProcessTrack(TracklikeCandidate& tlcData);
            // Core method
            bool DoWeightedSlopeAlgorithm(Points& points, TrackID trackID, KTProcessedTrackData* procTrack);

        private:
            template<typename TracklikeCandidate> 
            TrackID ExtractTrackID(TracklikeCandidate tlcData);

            //***************
            // Signals
            //***************

        private:
            Nymph::KTSignalData fTrackSignal;

            //***************
            // Slots
            //***************

        private:
            Nymph::KTSlotDataOneType< KTSparseWaterfallCandidateData > fSWFSlot;
            Nymph::KTSlotDataOneType< KTSequentialLineData > fSeqSlot;

    };

}
 /* namespace Katydid */
#endif /* KTTrackProcessingWeightedSlope_HH_ */
