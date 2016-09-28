/**
 @file KTSeqTrackCreator.hh
 @brief Contains KTSeqTrackCreator
 @details Creates a track
 @author: Christine
 @date: Sep 15, 2016
 */

#ifndef KTSEQTRACKCREATOR_HH_
#define KTSEQTRACKCREATOR_HH_

#include "KTProcessor.hh"
#include "KTMemberVariable.hh"
#include "KTSlot.hh"

namespace Nymph
{
    class KTParamNode;
}
;

namespace Katydid
{

    class KTKDTreeData;


    class KTSeqTrackCreator : public Nymph::KTProcessor
    {
        public:
            KTSeqTrackCreator(const std::string& name = "seq-clustering");
            virtual ~KTSeqTrackCreator();

            bool KTSeqTrackCreator::Configure(const KTParamNode* node);
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



            bool PointLineAssignment(KTSliceHeader& slHeader, KTPowerSpectrumData& slice,  KTDiscriminatedPoints1DData& discPoints);
            bool PointLineAssignment(KTSliceHeader& slHeader, KTScoredSpectrumData& spectrum);
            bool LoopOverHighPowerPoints(KTScoredSpectrum& slice, std::vector<unsigned>& Points, double& TimeInAcq, double& TrimmingLimits);
            void SearchAreaForBetterPoints(unsigned& PointBin, KTScoredSpectrum& slice, double& PointFreq);
            void WeightedAverage(KTScoredSpectrum& slice, double& PointFreq, unsigned& PointBin)

        private:
       //     bool LoopOverDiscriminatedPoints(const KTDiscriminatedPoints1DData::SetOfPoints&  incomingPts, KTPowerSpectrum& slice, double& TimeInAcq, double& TrimmingTimits);
       //     bool KTSeqTrackCreator::VetoPoint(KTDiscriminatedPoints1DData::SetOfPoints::const_iterator Point, KTPowerSpectrum& slice, double& freq);




            //***************
            // Signals
            //***************

        private:
            Nymph::KTSignalData fSeqTrackSignal;

            //***************
            // Slots
            //***************

        private:
            Nymph::KTSlotDataTwoTypes< KTSliceHeader, KTScoredSpectrumData > fSeqTrackSlot;

    };
    inline void KTSeqTrackCreator::SetFrequencyRadius(double FreqRad)
    {
       	fFDelta = FreqRad;
    }
    inline void KTSeqTrackCreator::SetTimeDistance(double TimeDistance)
    {
       	fTimeDistance = TimeDistance;
    }
    inline void KTSeqTrackCreator::SetBinDelta(int BinDelta)
    {
       	fBinDelta = BinDelta;
    }
    inline unsigned KTSeqTrackCreator::GetMinBin() const
    {
        return fMinBin;
    }

    inline void KTSeqTrackCreator::SetMinBin(unsigned bin)
    {
        fMinBin = bin;
        fCalculateMinBin = false;
        return;
    }

    inline unsigned KTSeqTrackCreator::GetMaxBin() const
    {
        return fMaxBin;
    }

    inline void KTSeqTrackCreator::SetMaxBin(unsigned bin)
    {
        fMaxBin = bin;
        fCalculateMaxBin = false;
        return;
    }

} /* namespace Katydid */
#endif /* KTSEQTRACKCREATOR_HH_ */
