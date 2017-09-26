/**
 @file KTIterativeTrackClustering.hh
 @brief Contains KTIterativeTrackClustering
 @details Groups collinear tracks into one
 @author: C. Claessens
 @date: August 7, 2017
 */

#ifndef KTITERATIVETRACKCLUSTERING_HH_
#define KTITERATIVETRACKCLUSTERING_HH_

#include "KTPrimaryProcessor.hh"

#include "KTSlot.hh"
#include "KTData.hh"
#include "KTMemberVariable.hh"
#include "KTProcessedTrackData.hh"

#include <vector>

namespace Katydid
{
    
    /*!
     @class KTIterativeLineClustering
     @author C. Claessens

     @brief merges track segments until number of lines stops decreasing

     @details
     Checks whether track start/ends matches another track's extrapolation

     Configuration name: "iterative-track-clustering"

     Available configuration values:
     - "time-gap-tolerance": maximum time gap between tracks
     - "frequency-acceptance": maximum allowed distance from the slope extrapolation
     - "max-track-width": radius around a track where there cannot be another track
     - "apply-power-cut": default is false
     - "apply-power-density-cut": default is false
     - "power-threshold": total track power must be above this threshold
     - "power-density-threshold": total power per second threshold

     Slots:
     - "track": void (shared_ptr<KTData>) -- If this is a new acquisition; Adds tracks to the internally-stored set of points; Requires KTProcessedTrackData.
     - "do-clustering": void () -- Triggers clustering algorithm

     Signals:
     - "track": void (shared_ptr<KTData>) -- Emitted for each group found; Guarantees KTProcessedTrackData.
     - "clustering-done": void () -- Emitted when track clustering is complete
    */

    class KTIterativeTrackClustering : public Nymph::KTPrimaryProcessor
    {
        public:
            KTIterativeTrackClustering(const std::string& name = "iterative-track-clustering");
            virtual ~KTIterativeTrackClustering();

            bool Configure(const scarab::param_node* node);

            MEMBERVARIABLE(double, TimeGapTolerance);
            MEMBERVARIABLE(double, FrequencyAcceptance);
            MEMBERVARIABLE(double, MaxTrackWidth);
            MEMBERVARIABLE(bool, ApplyPowerCut);
            MEMBERVARIABLE(bool, ApplyDensityCut);
            MEMBERVARIABLE(double, PowerThreshold);
            MEMBERVARIABLE(double, DensityThreshold);
            MEMBERVARIABLE(unsigned, NTracks);


        public:
            // Store point information locally
            bool TakeTrack(KTProcessedTrackData& track);

            //void SetNComponents(unsigned nComps);
            bool DoClustering();
            bool Run();


        private:
            bool ExtrapolateClustering();
            bool DoTheyMatch(KTProcessedTrackData& track1, KTProcessedTrackData& track2);
            bool DoTheyOverlap(KTProcessedTrackData& track1, KTProcessedTrackData& track2);
            const void CombineTracks(const KTProcessedTrackData& track1, KTProcessedTrackData& track2);
            bool FindMatchingTracks();
            void EmitTrackCandidates();
            const void ProcessNewTrack( KTProcessedTrackData& myNewTrack );


            std::vector<KTProcessedTrackData> fCompTracks;
            std::vector<KTProcessedTrackData> fNewTracks;


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
            Nymph::KTSlotDataOneType< KTProcessedTrackData > fTakeTrackSlot;

            void DoClusteringSlot();

    };

}
 /* namespace Katydid */
#endif /* KTITERATIVETRACKCLUSTERING_HH_ */
