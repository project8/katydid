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

#include <algorithm>
#include <set>
#include <vector>
#include <list>


namespace Katydid
{
    
    /*!
     @class KTIterativeLineClustering
     @author C. Claessens

     @brief Clusters tracks together until number of lines stops decreasing

     @details
     Checks whether line start/ends matches another track's extrapolation

     Configuration name: "iterative-track-clustering"

     Available configuration values:
     - "time-gap-tolerance": maximum time gap between tracks
     - "frequency-acceptance": maximum allowed distance from the slope extrapolation
     - "apply-power-cut": default is true
     - "apply-power-density-cut": default is false
     - "power-threshold": total track power must be above this threshold
     - "power-density-threshold": totel power per second threshold

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
            bool LineClustering();
            bool DoTheyMatch(KTProcessedTrackData& Track1, KTProcessedTrackData& Track2);
            bool DoTheyOverlap(KTProcessedTrackData& Track1, KTProcessedTrackData& Track2);
            void CombineTracks(KTProcessedTrackData& Track1, KTProcessedTrackData& Track2);
            bool FindMatchingTracks();
            void EmitTrackCandidates();
            //void EmitPreCandidatesAsCandidates();
            void ProcessNewTrack( KTProcessedTrackData& myNewTrack );

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
#endif /* KTITERATIVETRACKCLUSTERING_HH_ */
