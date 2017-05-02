/**
 @file KTCollinearTrackClustering.hh
 @brief Contains KTCollinearTrackClustering
 @details Groups collinear tracks into one
 @author: E. Zayas
 @date: May 1, 2017
 */

#ifndef KTCOLLINEARTRACKCLUSTERING_HH_
#define KTCOLLINEARTRACKCLUSTERING_HH_

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
     @class KTCollinearTrackClustering
     @author E. Zayas

     @brief Identifies and groups collinear tracks

     @details
     Clusters tracks together in the slope/intercept plane with an elliptical boundary

     Configuration name: "collinear-track-clustering"

     Available configuration values:
     - "slope-radius": clustering tolerance in the slope axis
     - "frequency-radius": clustering tolernace in the intercept axis

     Slots:
     - "track": void (shared_ptr<KTData>) -- If this is a new acquisition; Adds tracks to the internally-stored set of points; Requires KTProcessedTrackData.
     - "do-clustering": void () -- Triggers clustering algorithm

     Signals:
     - "track": void (shared_ptr<KTData>) -- Emitted for each group found; Guarantees KTProcessedTrackData.
     - "clustering-done": void () -- Emitted when track clustering is complete
    */

    class KTCollinearTrackClustering : public Nymph::KTPrimaryProcessor
    {
        public:
            KTCollinearTrackClustering(const std::string& name = "collinear-track-clustering");
            virtual ~KTCollinearTrackClustering();

            bool Configure(const scarab::param_node* node);

            MEMBERVARIABLE(double, SlopeRadius);
            MEMBERVARIABLE(double, FrequencyRadius);

        public:
            // Store point information locally
            bool TakeTrack(KTProcessedTrackData& track);

            void SetNComponents(unsigned nComps);
            bool DoClustering();
            bool Run();


        private:
            bool FindCollinearTracks();
            std::vector< int > FindCluster();
            void ProcessNewTrack( KTProcessedTrackData& myNewTrack );
            int GetNUngrouped();

            std::vector< TrackSet > fCompTracks; // input tracks

            std::vector< double > fSlopes;
            std::vector< double > fIntercepts;
            std::vector< double > fTimeLengths;
            std::vector< int > fGroupingStatuses;

            int fUNGROUPED = 0;
            int fGROUPED = 1;
            int fREMOVED = -1;

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
#endif /* KTCOLLINEARTRACKCLUSTERING_HH_ */
