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
#include "KTMemberVariable.hh"
#include "KTSlot.hh"
#include "KTSeqLine.hh"
#include "KTLines.hh"

#include <iostream>

namespace Nymph
{
    class KTParamNode;
}
;

namespace Katydid
{

    class KTKDTreeData;


    class KTSeqTrackFinder : public Nymph::KTProcessor
    {

        //typedef std::map< unsigned, Point > SetOfPoints;

        public:
            KTSeqTrackFinder(const std::string& name = "seq-clustering");
            virtual ~KTSeqTrackFinder();

            bool KTSeqTrackFinder::Configure(const KTParamNode* node);
            void SetFrequencyRadius(double FreqRad);
            void SetTimeDistance(double TimeDistance);
            void SetBinDelta(int BinDelta);

            MEMBERVARIABLE(double, FDelta);
            MEMBERVARIABLE(int, TimeDistance);
            MEMBERVARIABLE(unsigned, Sigma);
            MEMBERVARIABLE(unsigned, BinDelta);
            MEMBERVARIABLE(unsigned, MinBin);
            MEMBERVARIABLE(bool, CalculateMinBin);
            MEMBERVARIABLE(unsigned, MaxBin);
            MEMBERVARIABLE(bool, CalculateMaxBin);
            double fMinFrequency;
            double fMaxFrequency;
            double fScoreThreshold;



        public:
            bool Configure(const scarab::param_node* node);
            unsigned GetMinBin() const;
            void SetMinBin(unsigned bin);
            unsigned GetMaxBin() const;
            void SetMaxBin(unsigned bin);
            double GetMinFrequency() const;
            void SetMinFrequency(double freq);
            double GetMaxFrequency() const;
            void SetMaxFrequency(double freq);
            void SetFrequencyRadius(double FreqRad);
            void SetTimeDistance(double TimeDistance);
            void SetBinDelta(int BinDelta);



            bool PointLineAssignment(KTSliceHeader& slHeader, KTScoredSpectrumData& spectrum, KTPowerSpectrumData& power_spectrum);
            bool LoopOverHighPowerPoints(KTScoredSpectrum& slice, std::vector<KTSeqLine::Point>& Points, KTLines& new_Lines, double& new_trimming_limits);
            void SearchAreaForBetterPoints(unsigned& PointBin, double& PointFreq, KTScoredSpectrum& slice);
            void WeightedAverage(KTScoredSpectrum& slice, double& PointFreq, unsigned& PointBin);

        private:
       //     bool LoopOverDiscriminatedPoints(const KTDiscriminatedPoints1DData::SetOfPoints&  incomingPts, KTPowerSpectrum& slice, double& TimeInAcq, double& TrimmingTimits);
       //     bool KTSeqTrackFinder::VetoPoint(KTDiscriminatedPoints1DData::SetOfPoints::const_iterator Point, KTPowerSpectrum& slice, double& freq);




            //***************
            // Signals
            //***************

        private:
            Nymph::KTSignalData fSeqTrackSignal;
            Nymph::KTSignalData fSeqTrackFindingDone;

            //***************
            // Slots
            //***************

        private:
            Nymph::KTSlotDataTwoTypes< KTSliceHeader, KTScoredSpectrumData > fSeqTrackSlot;

    };
    inline void KTSeqTrackFinder::SetFrequencyRadius(double FreqRad)
    {
       	fFDelta = FreqRad;
    }
    inline void KTSeqTrackFinder::SetTimeDistance(double TimeDistance)
    {
       	fTimeDistance = TimeDistance;
    }
    inline void KTSeqTrackFinder::SetBinDelta(int BinDelta)
    {
       	fBinDelta = BinDelta;
    }
    inline unsigned KTSeqTrackFinder::GetMinBin() const
    {
        return fMinBin;
    }

    inline void KTSeqTrackFinder::SetMinBin(unsigned bin)
    {
        fMinBin = bin;
        fCalculateMinBin = false;
        return;
    }

    inline unsigned KTSeqTrackFinder::GetMaxBin() const
    {
        return fMaxBin;
    }

    inline void KTSeqTrackFinder::SetMaxBin(unsigned bin)
    {
        fMaxBin = bin;
        fCalculateMaxBin = false;
        return;
    }

} /* namespace Katydid */
#endif /* KTSEQTRACKFinder_HH_ */
