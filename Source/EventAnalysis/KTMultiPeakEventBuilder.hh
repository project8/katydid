/**
 @file KTMultiPeakEventBuilder.hh
 @brief Contains KTMultiPeakEventBuilder
 @details Groups tracks into events
 @author: N.S. Oblath
 @date: Dec 7, 2015
 */

#ifndef KTMULTIPEAKEVENTBUILDER_HH_
#define KTMULTIPEAKEVENTBUILDER_HH_

#include "KTPrimaryProcessor.hh"

#include "KTDBScan.hh"
#include "KTDistanceMatrix.hh"
#include "KTSlot.hh"
#include "KTData.hh"
#include "KTMemberVariable.hh"
#include "KTProcessedTrackData.hh"

#include <algorithm>
#include <set>
#include <vector>
#include <list>

namespace Nymph
{
    class KTParamNode;
}
;

namespace Katydid
{
    using namespace Nymph;
/*
    // Track distance
    // Vector format for representing tracks: (tstart, fstart, tend, fend)
    // Dimension t: for tstart_1 < tstart_2, Dt = max(0, tstart_2 - tend_1)
    // Dimension f: Df = fstart_2 - fend_1
    // Dist = sqrt(Dt^2 + Df^2)
    template < typename VEC_T >
    class TrackDistance
    {
        protected:
            typedef VEC_T vector_type;

            double GetDistance(const VEC_T v1, const VEC_T v2)
            {
                double deltaT, deltaF;
                if (v1(0) < v2(0))
                {
                    deltaT = std::max(0., v2(0) - v1(2));
                    deltaF = v2(1) - v1(3);
                }
                else
                {
                    deltaT = std::max(0., v1(0) - v2(2));
                    deltaF = v1(1) - v2(3);
                }
                return sqrt(deltaT * deltaT + deltaF * deltaF);
            };

    };
*/

    /*!
     @class KTMultiPeakEventBuilder
     @author N.S. Oblath

     @brief Builds tracks into events; identifies multi-peak tracks (i.e. tracks with sidebands), and then groups into events.

     @details
     Normalization of the axes:
     The DBSCAN algorithm expects expects that all of the dimensions that describe a points will have the same scale,
     such that a single radius parameter can describe a sphere in the parameter space that's used to cluster points together.
     For track clustering, two radii are specified, one for the time dimension and one for the frequency dimension.
     For clustering, a scaling factor is calculated for each axis such that the ellipse formed by the two radii is
     scaled to a unit circle.  Those scaling factors are applied to every point before the data is passed to the
     DBSCAN algorithm.

     Configuration name: "multi-peak-event-builder"

     Available configuration values:
     - "radii": double[2] -- array used to describe the distances that will be used to cluster tracks together; [time, frequency]
     - "min-points": unsigned int -- minimum number of tracks required to have a cluster

     Slots:
     - "track": void (shared_ptr<KTData>) -- If this is a new acquisition; Adds tracks to the internally-stored set of points; Requires KTSliceHeader and KTDiscriminatedPoints1DData.
     - "do-clustering": void () -- Triggers clustering algorithm

     Signals:
     - "event": void (shared_ptr<KTData>) -- Emitted for each cluster found; Guarantees KT???Data.
     - "clustering-done": void () -- Emitted when track clustering is complete
    */

    class KTMultiPeakEventBuilder : public KTPrimaryProcessor
    {
        public:
            KTMultiPeakEventBuilder(const std::string& name = "multi-peak-event-builder");
            virtual ~KTMultiPeakEventBuilder();

            bool Configure(const KTParamNode* node);

            MEMBERVARIABLE(double, SidebandTimeTolerance);

            MEMBERVARIABLE(double, JumpTimeTolerance);
            //MEMBERVARIABLE(double, JumpFreqTolerance);

        public:
            // Store point information locally
            bool TakeTrack(KTProcessedTrackData& track);
            //bool TakeTrack(double startTime, double startFreq, double endTime, double endFreq, unsigned component=0);

            void SetNComponents(unsigned nComps);
            void SetTimeBinWidth(double bw);
            void SetFreqBinWidth(double bw);

            bool Run();

            bool DoClustering();

            const std::set< KTDataPtr >& GetCandidates() const;
            unsigned GetDataCount() const;

        private:
            bool FindMultiPeakTracks();
            bool FindEvents();

            double fTimeBinWidth;
            double fFreqBinWidth;

            struct TrackComp
            {
                bool operator() (const KTProcessedTrackData& lhs, const KTProcessedTrackData& rhs) const
                {
                    if (lhs.GetStartTimeInRunC() != rhs.GetStartTimeInRunC()) return lhs.GetStartTimeInRunC() < rhs.GetStartTimeInRunC();
                    if (lhs.GetEndTimeInRunC() != rhs.GetEndTimeInRunC()) return lhs.GetEndTimeInRunC() < rhs.GetEndTimeInRunC();
                    if (lhs.GetStartFrequency() != rhs.GetStartFrequency()) return lhs.GetStartFrequency() < rhs.GetStartFrequency();
                    return lhs.GetEndFrequency() < rhs.GetEndFrequency();
                }
            };

            typedef std::set< KTProcessedTrackData, TrackComp > TrackSet;
            typedef TrackSet::iterator TrackSetIt;
            typedef TrackSet::const_iterator TrackSetCIt;

            std::vector< TrackSet > fCompTracks; // input tracks

            struct TrackSetCItComp
            {
                bool operator() (const TrackSetCIt& lhs, const TrackSetCIt& rhs) const
                {
                    if (lhs->GetStartTimeInRunC() != rhs->GetStartTimeInRunC()) return lhs->GetStartTimeInRunC() < rhs->GetStartTimeInRunC();
                    if (lhs->GetEndTimeInRunC() != rhs->GetEndTimeInRunC()) return lhs->GetEndTimeInRunC() < rhs->GetEndTimeInRunC();
                    if (lhs->GetStartFrequency() != rhs->GetStartFrequency()) return lhs->GetStartFrequency() < rhs->GetStartFrequency();
                    return lhs->GetEndFrequency() < rhs->GetEndFrequency();
                }
            };

            //typedef std::vector< std::list< KTProcessedTrackData > > MultiPeakTrackRef;
            struct MultiPeakTrackRef
            {
                std::set< TrackSetCIt, TrackSetCItComp > fTrackRefs;
                // Keep track of both the sum and the mean so that the mean can be updated regularly without an extra multiplication
                double fMeanStartTimeInRunC;
                double fSumStartTimeInRunC;
                double fMeanEndTimeInRunC;
                double fSumEndTimeInRunC;
                uint64_t fAcquisitionID;

                MultiPeakTrackRef();
                bool InsertTrack(const TrackSetCIt& trackRef);
                void Clear();
            };

            struct MTRComp
            {
                bool operator() (const MultiPeakTrackRef& lhs, const MultiPeakTrackRef& rhs)
                {
                    if (lhs.fMeanStartTimeInRunC != rhs.fMeanStartTimeInRunC) return lhs.fMeanStartTimeInRunC < rhs.fMeanStartTimeInRunC;
                    return lhs.fMeanEndTimeInRunC < rhs.fMeanEndTimeInRunC;
                }

            };

            std::vector< std::set< MultiPeakTrackRef, MTRComp > > fMPTracks;

            std::set< KTDataPtr > fCandidates;
            unsigned fDataCount;

            //***************
            // Signals
            //***************

        private:
            KTSignalData fEventSignal;
            KTSignalOneArg< void > fEventsDoneSignal;

            //***************
            // Slots
            //***************

        private:
            KTSlotDataOneType< KTProcessedTrackData > fTakeTrackSlot;
            //KTSlotDataOneType< KTInternalSignalWrapper > fDoClusterSlot;

            void DoClusteringSlot();

    };

    inline void KTMultiPeakEventBuilder::SetTimeBinWidth(double bw)
    {
        fTimeBinWidth = bw;
        return;
    }
    inline void KTMultiPeakEventBuilder::SetFreqBinWidth(double bw)
    {
        fFreqBinWidth = bw;
        return;
    }

    inline const std::set< KTDataPtr >& KTMultiPeakEventBuilder::GetCandidates() const
    {
        return fCandidates;
    }
    inline unsigned KTMultiPeakEventBuilder::GetDataCount() const
    {
        return fDataCount;
    }


}
 /* namespace Katydid */
#endif /* KTMULTIPEAKEVENTBUILDER_HH_ */
