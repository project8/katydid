/**
 @file KTOverlappingTrackClustering.hh
 @brief Contains KTOverlappingTrackClustering
 @details Groups overlapping or crossing tracks into one
 @author: C. Claessens
 @date: August 7, 2017
 */

#ifndef KTOVERLAPPINGTRACKCLUSTERING_HH_
#define KTOVERLAPPINGTRACKCLUSTERING_HH_

#include "KTPrimaryProcessor.hh"

#include "KTSlot.hh"
#include "KTData.hh"
#include "KTMemberVariable.hh"
#include "KTProcessedTrackData.hh"

#include <vector>

namespace Katydid
{

    /*!
     @class KTOverlappingLineClustering
     @author C. Claessens

     @brief Clusters tracks together until number of lines stops decreasing

     @details
     Checks whether tracks start/ends are very close to another track or whether tracks cross.
     This step is necessary for dans algorithm, because for a wide of slightly curved track it often finds several parallel track segments

     Configuration name: "overlapping-track-clustering"

     Available configuration values:
     - "max-track-width": tracks that are not further apart than this value in frequency will be grouped together to a combined track
     - "apply-power-cut": default is false
     - "apply-power-density-cut": default is false
     - "power-threshold": total track power must be above this threshold
     - "power-density-threshold": total power/second threshold

     Slots:
     - "track": void (shared_ptr<KTData>) -- If this is a new acquisition; Adds tracks to the internally-stored set of points; Requires KTProcessedTrackData.
     - "do-clustering": void () -- Triggers clustering algorithm

     Signals:
     - "track": void (shared_ptr<KTData>) -- Emitted for each group found; Guarantees KTProcessedTrackData.
     - "clustering-done": void () -- Emitted when track clustering is complete
    */

    class KTOverlappingTrackClustering : public Nymph::KTPrimaryProcessor
    {
        public:
            KTOverlappingTrackClustering(const std::string& name = "overlapping-track-clustering");
            virtual ~KTOverlappingTrackClustering();

            bool Configure(const scarab::param_node* node);

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
            bool OverlapClustering();
            bool DoTheyOverlap(KTProcessedTrackData& track1, KTProcessedTrackData& track2);
            bool DoTheyCross(KTProcessedTrackData& track1, KTProcessedTrackData& track2);
            const void CombineTracks(const KTProcessedTrackData& track1, KTProcessedTrackData& track2);
            bool FindMatchingTracks();
            void EmitTrackCandidates();
            const void ProcessNewTrack( KTProcessedTrackData& myNewTrack );

            //std::vector< TrackSet > fCompTracks; // input tracks
            //std::vector< TrackSet> fNewTracks;

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
#endif /* KTOVERLAPPINGTRACKCLUSTERING_HH_ */
