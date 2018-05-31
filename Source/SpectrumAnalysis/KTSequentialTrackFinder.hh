/**
 @file KTSequentialTrackFinder.hh
 @brief Contains KTSeqTrackFinder
 @details Creates a track
 @author: C. Claessens
 @date: Aug 4, 2017
 */

#ifndef KTSEQUENTIALTRACKFINDER_HH_
#define KTSEQUENTIALTRACKFINDER_HH_

#include "KTProcessor.hh"
#include "KTData.hh"
#include "KTMemberVariable.hh"
#include "KTSlot.hh"

#include "KTSliceHeader.hh"
#include "KTPowerSpectrum.hh"
#include "KTPowerSpectrumData.hh"
#include "KTGainVariationData.hh"
#include "KTSequentialLineData.hh"
//#include "KTProcessedTrackData.hh"
#include "KTSparseWaterfallCandidateData.hh"
#include "KTDiscriminatedPoints1DData.hh"
#include "KTDiscriminatedPoint.hh"

#include <iostream>
#include <set>


namespace Katydid
{
    /*!
     @class KTSeqTrackFinder
     @author C. Claessens

     @brief Implementation of Dan Furse's algorithm with some modifications

     @details
     Collects points on a linear track

     Configuration name: "sequential-track-finder"

     Available configuration values:
     - "min-frequency": minimum allowed frequency (has to be set)
     - "max-frequency": max allowed frequency (has to be set)
     - "min-bin": can be set instead of min frequency
     - "max-bin": can be set instead of  max frequency
     - "trimming-threshold": before a line is converted to a sparse waterfall candidate its edges get trimmed. If the last or first line point snr is less than the trimming-threshold, they get removed
     - "line-power-radius": only valid for disc1d-ps slot. the power that is assigned to a line point is the sum of the power_spectrum[point_bin - line_width: point_bin + line_width]
     - "time-gap-tolerance": maximum gap between points in a line (in seconds)
     - "minimum-line-distance": requires some thought for disc1d-slot!!! For disc1d-ps slot: if a point is less than this distance (in bins) away from the last point it will be skipped
     - "search-radius": for disc1d-ps slot: before a point is added to a line, the weighted average of the points frequency neighborhood (+/- search-radius in bins) is taken and the point updated until the frequency converges
     - "converge-delta": for disc1d-ps slot: defines when convergence has been reached (in bins)
     - "frequency-acceptance": maximum allowed frequency distance of point to an extrapolated line (in Hz)
     - "slope-method": method to update the line slope after point collection
     - "initial-frequency-acceptance": if the line that a point is being compared to, only has a single point so far, this is the accepted frequency acceptance. Default isfrequency_acceptance
     - "initial-slope": if a line has only one point, this is the line's slope
     - "n-slope-points": maximum number of points to include in the slope calculation
     - "min-points": a line only gets converted to a track if it has collected more than this many number of points
     - "min-slope": a line only gets converted to a track if its slope is > than this slope (in Hz/s)
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
     - "disc1d": clusters discriminated points to sequential lines candidates
     - "disc1d-ps": clusters discriminated points to sequential line candidates; updates point properties using power spectrum slice
     - "done": connect with egg:done. Processes remaining active lines and emits clustering-done signal

     Signals:
     - "seq-lines": KTSparseWaterfallCandidateData
     - "clustering-done": void () -- Emitted when track clustering is complete
    */


    class KTSequentialTrackFinder : public Nymph::KTProcessor
    {

        private:
            enum class slopeMethod
            {
                weighted_first_point_ref,
                weighted_last_point_ref,
                unweighted
            };

        public:
            struct KTDiscriminatedPointComparePower
            {
                bool operator() (const KTDiscriminatedPoint& lhs, const KTDiscriminatedPoint& rhs) const
                {
                    return lhs.fAmplitude< rhs.fAmplitude || (lhs.fAmplitude == rhs.fAmplitude && lhs.fFrequency < rhs.fFrequency);
                }
            };

            typedef std::set< KTDiscriminatedPoint, KTDiscriminatedPointComparePower > KTDiscriminatedPowerSortedPoints;


        public:
            KTSequentialTrackFinder(const std::string& name = "seq-clustering");
            virtual ~KTSequentialTrackFinder();

            bool Configure(const scarab::param_node* node);
            bool CollectDiscrimPointsFromSlice(KTSliceHeader& slHeader, KTPowerSpectrumData& spectrum, KTDiscriminatedPoints1DData& discrimPoints);
            bool CollectDiscrimPointsFromSlice(KTSliceHeader& slHeader, KTDiscriminatedPoints1DData& discrimPoints);
            bool CollectDiscrimPoints(const KTSliceHeader& slHeader, const KTPowerSpectrumData& spectrum, const KTDiscriminatedPoints1DData& discrimPoints);
            bool CollectDiscrimPoints(const KTSliceHeader& slHeader, const KTDiscriminatedPoints1DData& discrimPoints);
            bool LoopOverHighPowerPoints(KTPowerSpectrum& powerSpectrum, KTDiscriminatedPowerSortedPoints& points, uint64_t acqID, unsigned component);
            bool LoopOverHighPowerPoints(KTDiscriminatedPowerSortedPoints& points, uint64_t acqID, unsigned component);

            void UpdateLinePoint(KTDiscriminatedPoint& point, KTPowerSpectrum& slice);
            void WeightedAverage(const KTPowerSpectrum& slice, unsigned& frequencyBin, double& frequency);
            void (KTSequentialTrackFinder::*fCalcSlope)(KTSequentialLineData& Line);
            void CalculateSlopeFirstRef(KTSequentialLineData& Line);
            void CalculateSlopeLastRef(KTSequentialLineData& Line);
            //void CalculateWeightedSlope(LineRef& Line);
            void CalculateUnweightedSlope(KTSequentialLineData& Line);
            bool EmitPreCandidate(KTSequentialLineData line);
            void AcquisitionIsOver();


            const std::set< Nymph::KTDataPtr >& GetCandidates() const;

        public:
            // Parameters for point update before adding point to line
            MEMBERVARIABLE(int, SearchRadius);
            MEMBERVARIABLE(double, ConvergeDelta);
            MEMBERVARIABLE(int, LinePowerRadius);
            MEMBERVARIABLE(double, PointAmplitudeAfterVisit);
            MEMBERVARIABLE(int, MinFreqBinDistance);

            // Parameters for point collection
            MEMBERVARIABLE(double, InitialSlope);
            MEMBERVARIABLE(slopeMethod, SlopeMethod);
            MEMBERVARIABLE(signed, NSlopePoints);
            MEMBERVARIABLE(double, FrequencyAcceptance);
            MEMBERVARIABLE(double, InitialFrequencyAcceptance);
            MEMBERVARIABLE(double, TimeGapTolerance);

            // Parameters for line post-processing
            //MEMBERVARIABLE(double, TrimmingFactor);
            MEMBERVARIABLE(double, TrimmingThreshold);
            MEMBERVARIABLE(unsigned, MinPoints);
            MEMBERVARIABLE(double, MinSlope);
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

            // Others
            MEMBERVARIABLE(unsigned, NLines);
            MEMBERVARIABLE(unsigned, MinBin);
            MEMBERVARIABLE(bool, CalculateMinBin);
            MEMBERVARIABLE(unsigned, MaxBin);
            MEMBERVARIABLE(double, BinWidth);
            MEMBERVARIABLE(bool, CalculateMaxBin);
            MEMBERVARIABLE(double, MinFrequency);
            MEMBERVARIABLE(double, MaxFrequency);


        private:
            std::vector< KTSequentialLineData > fActiveLines;
            std::set< Nymph::KTDataPtr > fCandidates;






            //***************
            // Signals
            //***************

        private:
            Nymph::KTSignalData fLineSignal;
            Nymph::KTSignalOneArg< void > fClusterDoneSignal;

            //***************
            // Slots
            //***************

        private:
            Nymph::KTSlotDataThreeTypes < KTSliceHeader, KTPowerSpectrumData, KTDiscriminatedPoints1DData > fDiscrimPowerSlot;
            Nymph::KTSlotDataTwoTypes < KTSliceHeader, KTDiscriminatedPoints1DData > fDiscrimSlot;
            Nymph::KTSlotDone fDoneSlot;

    };
    inline const std::set< Nymph::KTDataPtr >& KTSequentialTrackFinder::GetCandidates() const
    {
        return fCandidates;
    }

} /* namespace Katydid */
#endif /* KTSEQUENTIALTRACKFinder_HH_ */
