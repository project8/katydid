/**
 @file KTTrackProcessing.hh
 @brief Contains KTTrackProcessing
 @details Extracts physics-relevant information about tracks
 @author: N.S. Oblath & B. LaRoque
 @date: July 22, 2013
 */

#ifndef KTTRACKPROCESSING_HH_
#define KTTRACKPROCESSING_HH_

#include "KTProcessor.hh"

#include "KTMemberVariable.hh"

#include "KTSlot.hh"

#include <cmath>


namespace Katydid
{

    class KTHoughData;
    class KTSparseWaterfallCandidateData;

    /*!
     @class KTTrackProcessing
     @author N.S. Oblath, B. LaRoque & M. Guigue

     @brief Extracts physics-relevant information about tracks

     @details

     Configuration name: "track-proc"

     Available configuration values:
     - "algorithm": string -- Select the track processing algorithm: "double-cuts" (default) or "weighted-slope"
     - "pl-dist-cut1": double -- Point-line distance cut 1; rough cut
     - "pl-dist-cut2": double -- Point-line distance cut 2: fine cut
     - "slope-min": double -- Minimum track slope to keep (Hz/s)
     - "min-points": unsigned -- Minimum number of points required to keep a processed track
     - "assigned-error": double -- Error assigned to the points in case of perfectly aligned points

     Slots:
     - "swfc": void (KTDataPr) -- [what it does]; Requires KTSparseWaterfallCandidateData; Adds KTProcessedTrackData; Emits signal "track"
     - "swfc-and-hough": void (KTDataPr) -- [what it does]; Requires KTSparseWaterfallCandidateData and KTHoughData; Adds KTProcessedTrackData; Emits signal "track"

     Signals:
     - "track": void (Nymph::KTDataPtr) -- Emitted when a track has been processed; Guarantees KTProcessedTrackData.
    */

    class KTTrackProcessing : public Nymph::KTProcessor
    {
        public:
            KTTrackProcessing(const std::string& name = "track-proc");
            virtual ~KTTrackProcessing();

            bool Configure(const scarab::param_node* node);

        private:

            MEMBERVARIABLE(std::string, TrackProcAlgorithm);
            MEMBERVARIABLE(double, PointLineDistCut1);
            MEMBERVARIABLE(double, PointLineDistCut2);
            MEMBERVARIABLE(double, SlopeMinimum);
            MEMBERVARIABLE(unsigned, ProcTrackMinPoints);
            MEMBERVARIABLE(double, ProcTrackAssignedError);

        public:
            bool ProcessTrackSWF(KTSparseWaterfallCandidateData& swfData);
            bool ProcessTrackSWFAndHough(KTSparseWaterfallCandidateData& swfData, KTHoughData& htData);
            bool ProcessTrackDoubleCuts(KTSparseWaterfallCandidateData& swfData, KTHoughData& htData);
            bool ProcessTrackWeightedSlope(KTSparseWaterfallCandidateData& swfData);

        private:
            double PointLineDistance(double pointX, double pointY, double lineA, double lineB, double lineC);

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
            Nymph::KTSlotDataTwoTypes< KTSparseWaterfallCandidateData, KTHoughData > fSWFAndHoughSlot;

    };

    double KTTrackProcessing::PointLineDistance(double pointX, double pointY, double lineA, double lineB, double lineC)
    {
        return fabs(lineA * pointX + lineB * pointY + lineC) / sqrt(lineA*lineA + lineB*lineB);
    }
}
 /* namespace Katydid */
#endif /* KTTRACKPROCESSING_HH_ */
