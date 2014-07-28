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
    class KTParamNode;
    class KTSparseWaterfallCandidateData;

    /*!
     @class KTTrackProcessing
     @author N.S. Oblath & B. LaRoque

     @brief Extracts physics-relevant information about tracks

     @details

     Configuration name: "track-proc"

     Available configuration values:
     - "pl-dist-cut1": double -- Point-line distance cut 1; rough cut
     - "pl-dist-cut2": double -- Point-line distance cut 2: fine cut

     Slots:
     - "swfc-and-hough": void (KTDataPr) -- [what it does]; Requires KTSparseWaterfallCandidateData and KTHoughData; Adds KTProcessedTrackData; Emits signal "track"

     Signals:
     - "track": void (KTDataPtr) -- Emitted when a track has been processed; Guarantees KTProcessedTrackData.
    */

    class KTTrackProcessing : public KTProcessor
    {
        public:
            KTTrackProcessing(const std::string& name = "track-proc");
            virtual ~KTTrackProcessing();

            bool Configure(const KTParamNode* node);

            double GetPointLineDistCut1() const;
            void SetPointLineDistCut1(double dist);

            double GetPointLineDistCut2() const;
            void SetPointLineDistCut2(double dist);

        private:
            double fPointLineDistCut1;
            double fPointLineDistCut2;

        public:
            bool ProcessTrack(KTSparseWaterfallCandidateData& swfData, KTHoughData& htData);

        private:
            /// Point-to-line distance: point coordinates (x, y); line equation a*x + b*y + c = 0
            double PointLineDistance(double pointX, double pointY, double lineA, double lineB, double lineC);

            //***************
            // Signals
            //***************

        private:
            KTSignalData fTrackSignal;

            //***************
            // Slots
            //***************

        private:
            KTSlotDataTwoTypes< KTSparseWaterfallCandidateData, KTHoughData > fSWFAndHoughSlot;

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

    double KTTrackProcessing::PointLineDistance(double pointX, double pointY, double lineA, double lineB, double lineC)
    {
        return fabs(lineA * pointX + lineB * pointY + lineC) / sqrt(lineA*lineA + lineB*lineB);
    }
}
 /* namespace Katydid */
#endif /* KTTRACKPROCESSING_HH_ */
