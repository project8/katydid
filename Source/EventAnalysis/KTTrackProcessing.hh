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

#include "KTSlot.hh"

#include <cmath>


namespace Katydid
{

    class KTHoughData;
    class KTSparseWaterfallCandidateData;

    /*!
     @class KTTrackProcessing
     @author N.S. Oblath & B. LaRoque

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

            double GetPointLineDistCut1() const;
            void SetPointLineDistCut1(double dist);

            double GetPointLineDistCut2() const;
            void SetPointLineDistCut2(double dist);

            double GetSlopeMinimum() const;
            void SetSlopeMinimum(double slope);

            unsigned GetProcTrackMinPoints() const;
            void SetProcTrackMinPoints(unsigned min);

            double GetProcTrackAssignedError() const;
            void SetProcTrackAssignedError(double err);

        private:
            double fPointLineDistCut1;
            double fPointLineDistCut2;

            double fSlopeMinimum;

            unsigned fProcTrackMinPoints;
            double fProcTrackAssError;

        public:
            bool ProcessTrack(KTSparseWaterfallCandidateData& swfData, KTHoughData& htData);
            bool ProcessTrackDoubleCuts(KTSparseWaterfallCandidateData& swfData, KTHoughData& htData);
            bool ProcessTrackWeightedSlope(KTSparseWaterfallCandidateData& swfData, KTHoughData& htData);


        private:
            /// Point-to-line distance: point coordinates (x, y); line equation a*x + b*y + c = 0
            double PointLineDistance(double pointX, double pointY, double lineA, double lineB, double lineC);
            typedef bool (KTTrackProcessing::*TrackProcPtr)(KTSparseWaterfallCandidateData& , KTHoughData& );
            TrackProcPtr fTrackProcPtr;

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

    };

    inline double KTTrackProcessing::GetPointLineDistCut1() const
    {
        return fPointLineDistCut1;
    }
    inline void KTTrackProcessing::SetPointLineDistCut1(double dist)
    {
        fPointLineDistCut1 = dist;
        return;
    }

    inline double KTTrackProcessing::GetPointLineDistCut2() const
    {
        return fPointLineDistCut2;
    }
    inline void KTTrackProcessing::SetPointLineDistCut2(double dist)
    {
        fPointLineDistCut2 = dist;
        return;
    }

    inline double KTTrackProcessing::GetSlopeMinimum() const
    {
        return fSlopeMinimum;
    }
    inline void KTTrackProcessing::SetSlopeMinimum(double slope)
    {
        fSlopeMinimum = slope;
        return;
    }

    inline unsigned KTTrackProcessing::GetProcTrackMinPoints() const
    {
        return fProcTrackMinPoints;
    }
    inline void KTTrackProcessing::SetProcTrackMinPoints(unsigned min)
    {
        fProcTrackMinPoints = min;
        return;
    }

    inline double KTTrackProcessing::GetProcTrackAssignedError() const
    {
        return fProcTrackAssError;
    }
    inline void KTTrackProcessing::SetProcTrackAssignedError(double err)
    {
        fProcTrackAssError = err;
        return;
    }

    double KTTrackProcessing::PointLineDistance(double pointX, double pointY, double lineA, double lineB, double lineC)
    {
        return fabs(lineA * pointX + lineB * pointY + lineC) / sqrt(lineA*lineA + lineB*lineB);
    }
}
 /* namespace Katydid */
#endif /* KTTRACKPROCESSING_HH_ */
