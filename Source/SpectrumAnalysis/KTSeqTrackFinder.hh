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
#include "KTGainVariationData.hh"

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
            KTGainVariationData fGVData;

        public:
            KTSeqTrackFinder(const std::string& name = "seq-clustering");
            virtual ~KTSeqTrackFinder();

            bool Configure(const scarab::param_node* node);
            void SetSNRPowerThreshold(double thresh);

        public:
            MEMBERVARIABLE(ThresholdMode, Mode);
            //MEMBERVARIABLE(double, SNRPowerThreshold);
            MEMBERVARIABLE(double, TrimmingFactor);
            MEMBERVARIABLE(int, LinePowerWidth);
            MEMBERVARIABLE(double, PointAmplitudeAfterVisit);
            MEMBERVARIABLE(unsigned, MinFreqBinDistance);
            MEMBERVARIABLE(double, TimeGapTolerance);
            MEMBERVARIABLE(double, FrequencyAcceptance);
            MEMBERVARIABLE(int, SearchRadius);
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
            std::vector< LineRef> fActiveLines;


        public:
            bool SetPreCalcGainVar(KTGainVariationData& gvData);
            bool RunSequentialTrackFinding(KTSliceHeader& slHeader, KTPowerSpectrumData& spectrum);
            bool PointLineAssignment(KTSliceHeader& slHeader, KTPowerSpectrumData& spectrum, KTGainVariationData& gvData);
            bool LoopOverHighPowerPoints(std::vector<double>& slice, std::vector<Point>& Points, double& new_trimming_limits, unsigned component);
            void SearchTrueLinePoint(Point&, std::vector<double>& slice);
            void WeightedAverage(const std::vector<double>& slice, unsigned& FrequencyBin, double& Frequency);
            void ProcessNewTrack( KTProcessedTrackData& myNewTrack );
            bool EmitPreCandidate(LineRef Line, unsigned component);
            void AcquisitionIsOver();



            //***************
            // Signals
            //***************

        private:
            Nymph::KTSignalData fTrackSignal;
            Nymph::KTSignalOneArg< void > fClusterDoneSignal;

            //***************
            // Slots
            //***************

        private:
            //Nymph::KTSlotDataTwoTypes< KTSliceHeader, KTPowerSpectrumData > fSeqTrackSlot;
            Nymph::KTSlotDataOneType< KTGainVariationData > fGainVarSlot;
            Nymph::KTSlotDataTwoTypes< KTSliceHeader, KTPowerSpectrumData > fPSSlot;
            Nymph::KTSlotDone fDoneSlot;

    };
    inline void KTSeqTrackFinder::SetSNRPowerThreshold(double thresh)
    {
        fSNRPowerThreshold = thresh;
        fMode = eSNR_Power;
        return;
    }

} /* namespace Katydid */
#endif /* KTSEQTRACKFinder_HH_ */
