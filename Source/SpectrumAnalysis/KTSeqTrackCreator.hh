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

#include "KTKDTreeData.hh"
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

            bool Configure(const scarab::param_node* node);
            void SetFrequencyRadius(double FreqRad);
            void SetTimeDistance(double TimeDistance);
            void SetBinDelta(int BinDelta);

            MEMBERVARIABLE(double, FDelta);
            MEMBERVARIABLE(int, TimeDistance);
            MEMBERVARIABLE(unsigned, Sigma);
            MEMBERVARIABLE(unsigned, BinDelta);


        public:
            bool Configure(const KTParamNode* node);
            bool PointLineAssignment(KTSliceHeader& slHeader, KTPowerSpectrumData& slice,  KTDiscriminatedPoints1DData& discPoints);

        private:
            bool LoopOverDiscriminatedPoints(const KTDiscriminatedPoints1DData::SetOfPoints&  incomingPts, KTPowerSpectrum& slice, double& TimeInAcq, double* new_trimming_limits);
            bool KTSeqTrackCreator::VetoPoint(KTDiscriminatedPoints1DData::SetOfPoints::const_iterator Point, KTPowerSpectrum& slice, double& freq);




            //***************
            // Signals
            //***************

        private:
            Nymph::KTSignalData fSeqTrackSignal;

            //***************
            // Slots
            //***************

        private:
            Nymph::KTSlotDataTwoTypes< KTSliceHeader, KTDiscriminatedPoints1DData > fSeqTrackSlot;

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

} /* namespace Katydid */
#endif /* KTSEQTRACKCREATOR_HH_ */
