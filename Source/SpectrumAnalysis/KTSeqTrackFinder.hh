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
#include "KTLines.hh"

#include <iostream>
#include <set>

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
            const std::set< Nymph::KTDataPtr >& GetLines() const;
            //void SetFrequencyRadius(double FreqRad);
            //void SetTimeDistance(double TimeDistance);
            //void SetBinDelta(int BinDelta);

        public:
            MEMBERVARIABLE(char, Mode);
            MEMBERVARIABLE(double, FDelta);
            MEMBERVARIABLE(int, TimeDistance);
            MEMBERVARIABLE(unsigned, Sigma);
            MEMBERVARIABLE(unsigned, BinDelta);
            MEMBERVARIABLE(unsigned, MinBin);
            MEMBERVARIABLE(bool, CalculateMinBin);
            MEMBERVARIABLE(unsigned, MaxBin);
            MEMBERVARIABLE(bool, CalculateMaxBin);
            MEMBERVARIABLE(double, MinFrequency);
            MEMBERVARIABLE(double, MaxFrequency);
            MEMBERVARIABLE(double, ScoreThreshold);

        private:
            std::vector< Nymph::KTDataPtr > fnew_Lines;



        public:
            bool Configure(const scarab::param_node* node);
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



            bool PointLineAssignment(KTSliceHeader& slHeader, KTScoredSpectrumData& spectrum);
            bool LoopOverHighPowerPoints(KTScoredSpectrum& slice, std::vector<KTSeqLine::Point>& Points, double& new_trimming_limits);
            void SearchAreaForBetterPoints(unsigned& PointBin, double& PointFreq, KTScoredSpectrum& slice);
            void WeightedAverage(KTScoredSpectrum& slice, double& PointFreq, unsigned& PointBin);

        private:
       //     bool LoopOverDiscriminatedPoints(const KTDiscriminatedPoints1DData::SetOfPoints&  incomingPts, KTPowerSpectrum& slice, double& TimeInAcq, double& TrimmingTimits);
       //     bool KTSeqTrackFinder::VetoPoint(KTDiscriminatedPoints1DData::SetOfPoints::const_iterator Point, KTPowerSpectrum& slice, double& freq);

        public:
            inline const std::set< Nymph::KTDataPtr >& KTSeqTrackFinder::GetLines() const
                {
                    return fnew_Lines;
                }


            //***************
            // Signals
            //***************

        private:
            Nymph::KTSignalData fLineSignal;
            Nymph::KTSignalOneArg< void > fSeqTrackFindingDoneSignal;

            //***************
            // Slots
            //***************

        private:
            Nymph::KTSlotDataTwoTypes< KTSliceHeader, KTScoredSpectrumData > fSeqTrackSlot;

    };
   /* inline void KTSeqTrackFinder::SetFrequencyRadius(double FreqRad)
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
    */

} /* namespace Katydid */
#endif /* KTSEQTRACKFinder_HH_ */
