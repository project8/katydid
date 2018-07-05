/**
 @file KTTrackProcessingDoubleCuts.hh
 @brief Contains KTTrackProcessingDoubleCuts
 @details Extracts physics-relevant information about tracks using a double-cuts algorithm
 @author: N.S. Oblath, B. LaRoque & M. Guigue
 @date: July 22, 2013
 */

#ifndef KTTrackProcessingDoubleCuts_HH_
#define KTTrackProcessingDoubleCuts_HH_

#include "KTProcessor.hh"

#include "KTMemberVariable.hh"
#include "KTDiscriminatedPoint.hh"
#include "KTProcessedTrackData.hh"

#include "KTSlot.hh"

#include <cmath>


namespace Katydid
{

    class KTHoughData;
    class KTSparseWaterfallCandidateData;
    // class KTSequentialLineData;

    /*!
     @class KTTrackProcessingDoubleCuts
     @author N.S. Oblath, B. LaRoque & M. Guigue

     @brief Extracts physics-relevant information about tracks using a double-cuts algorithm

     @details

     Configuration name: "track-proc"

     Available configuration values:
     - "pl-dist-cut1": double -- Point-line distance cut 1; rough cut
     - "pl-dist-cut2": double -- Point-line distance cut 2: fine cut
     - "slope-min": double -- Minimum track slope to keep (Hz/s)
     - "min-points": unsigned -- Minimum number of points required to keep a processed track
     - "assigned-error": double -- Error assigned to the points in case of perfectly aligned points

     Slots:
     - "swfc-and-hough": void (KTDataPr) -- [what it does]; Requires KTSparseWaterfallCandidateData and KTHoughData; Adds KTProcessedTrackData; Emits signal "track"
     - "seqc-and-hough": void (KTDataPr) -- [what it does]; Requires KTSequentialLineData and KTHoughData; Adds KTProcessedTrackData; Emits signal "track"

     Signals:
     - "track": void (Nymph::KTDataPtr) -- Emitted when a track has been processed; Guarantees KTProcessedTrackData.
    */

    class KTTrackProcessingDoubleCuts : public Nymph::KTProcessor
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
            KTTrackProcessingDoubleCuts(const std::string& name = "track-proc-dc");
            virtual ~KTTrackProcessingDoubleCuts();

            bool Configure(const scarab::param_node* node);

        private:

            MEMBERVARIABLE(std::string, TrackProcAlgorithm);
            MEMBERVARIABLE(double, PointLineDistCut1);
            MEMBERVARIABLE(double, PointLineDistCut2);
            MEMBERVARIABLE(double, SlopeMinimum);
            MEMBERVARIABLE(unsigned, ProcTrackMinPoints);
            MEMBERVARIABLE(double, ProcTrackAssignedError);

        public:
            template<typename TracklikeCandidate> 
            bool ProcessTrack(TracklikeCandidate& tlcData, KTHoughData& htData);
            // Core methods for both algorithm
            bool DoDoubleCutsAlgorithm(Points& points, KTHoughData& htData, TrackID trackID, KTProcessedTrackData* procTrack);

        private:
            double PointLineDistance(double pointX, double pointY, double lineA, double lineB, double lineC);
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
            Nymph::KTSlotDataTwoTypes< KTSparseWaterfallCandidateData, KTHoughData > fSWFAndHoughSlot;
            // Nymph::KTSlotDataTwoTypes< KTSequentialLineData, KTHoughData > fSeqAndHoughSlot;

    };

    double KTTrackProcessingDoubleCuts::PointLineDistance(double pointX, double pointY, double lineA, double lineB, double lineC)
    {
        return fabs(lineA * pointX + lineB * pointY + lineC) / sqrt(lineA*lineA + lineB*lineB);
    }
}
 /* namespace Katydid */
#endif /* KTTrackProcessingDoubleCuts_HH_ */
