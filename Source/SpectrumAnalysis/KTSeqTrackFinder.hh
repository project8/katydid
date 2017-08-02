/**
 @file KTSeqTrackFinder.hh
 @brief Contains KTSeqTrackFinder
 @details Creates a track
 @author: Christine
 @date: Sep 15, 2016
 */

#ifndef KTSEQTRACKFinder_HH_
#define KTSEQTRACKFinder_HH_

#include "KTProcessor.hh"
#include "KTData.hh"
#include "KTMemberVariable.hh"
#include "KTSlot.hh"
#include "KTSeqLine.hh"
#include "KTProcessedTrackData.hh"

#include "KTSliceHeader.hh"
#include "KTPowerSpectrum.hh"
#include "KTPowerSpectrumData.hh"

#include <iostream>
#include <set>


namespace Katydid
{


    class KTSeqTrackFinder : public Nymph::KTProcessor
    {

        private:
            enum ThresholdMode
            {
                eSNR_Amplitude,
                eSNR_Power,
                eSigma
            };
            double fSNRPowerThreshold;

        public:
            KTSeqTrackFinder(const std::string& name = "seq-clustering");
            virtual ~KTSeqTrackFinder();

            bool Configure(const scarab::param_node* node);
            //const std::set< Nymph::KTDataPtr >& GetLines() const;
            //void SetFrequencyRadius(double FreqRad);
            //void SetTimeDistance(double TimeDistance);
            //void SetBinDelta(int BinDelta);

        public:
            MEMBERVARIABLE(ThresholdMode, Mode);
            //MEMBERVARIABLE(double, SNRPowerThreshold);
            MEMBERVARIABLE(double, TrimmingFactor);
            MEMBERVARIABLE(unsigned, LinePowerWidth);
            MEMBERVARIABLE(double, PointAmplitudeAfterVisit);
            MEMBERVARIABLE(unsigned, MinFreqBinDistance);
            MEMBERVARIABLE(double, TimeGapTolerance);
            MEMBERVARIABLE(double, FrequencyAcceptance);
            MEMBERVARIABLE(unsigned, SearchRadius);
            MEMBERVARIABLE(double, ConvergeDelta);
            //MEMBERVARIABLE(double, SNRThreshold);
            MEMBERVARIABLE(unsigned, MinPoints);
            MEMBERVARIABLE(double, MinSlope);
            MEMBERVARIABLE(unsigned, MinBin);
            MEMBERVARIABLE(bool, CalculateMinBin);
            MEMBERVARIABLE(unsigned, MaxBin);
            MEMBERVARIABLE(double, BinWidth);
            MEMBERVARIABLE(bool, CalculateMaxBin);
            MEMBERVARIABLE(double, MinFrequency);
            MEMBERVARIABLE(double, MaxFrequency);
            MEMBERVARIABLE(unsigned, NLines);



        private:
            //list< LineRef > fLines;
            //list< LineRef > fActiveLines;
            //std::vector< Nymph::KTDataPtr > fnew_Lines;
            //std::vector< LineRef> fLines;
            std::vector< LineRef> fActiveLines;


        public:
           /* unsigned GetMinBin() const;
            void SetMinBin(unsigned bin);
            unsigned GetMaxBin() const;
            void SetMaxBin(unsigned bin);
            double GetMinFrequency() const;
            void SetMinFrequency(double freq);
            double GetMaxFrequency() const;
            void SetMaxFrequency(double freq);
            void SetFrequencyRadius(double FreqRad);
            void SetTimeDistance(double TimeDistance);
            void SetBinDelta(int BinDelta);*/



            void PointLineAssignment(KTSliceHeader& slHeader, const KTPowerSpectrumData& spectrum);
            bool LoopOverHighPowerPoints(std::vector<double>& slice, std::vector<Point>& Points, double& new_trimming_limits, unsigned component);
            void SearchTrueLinePoint(Point&, std::vector<double>& slice);
            void WeightedAverage(const std::vector<double>& slice, unsigned& FrequencyBin, double& Frequency);
            void ProcessNewTrack( KTProcessedTrackData& myNewTrack );
            bool EmitPreCandidate(LineRef Line, unsigned component);
            void SetSNRPowerThreshold(double thresh);



            //***************
            // Signals
            //***************

        private:
            Nymph::KTSignalData fTrackSignal;
            Nymph::KTSignalOneArg< void > fDoneSignal;

            //***************
            // Slots
            //***************

        private:
            Nymph::KTSlotDataTwoTypes< KTSliceHeader, KTPowerSpectrumData > fSeqTrackSlot;

    };
    inline void KTSeqTrackFinder::SetSNRPowerThreshold(double thresh)
    {
        fSNRPowerThreshold = thresh;
        fMode = eSNR_Power;
        return;
    }

} /* namespace Katydid */
#endif /* KTSEQTRACKFinder_HH_ */
