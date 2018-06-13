/**
 @file KTDBScanTrackClustering.hh
 @brief Contains KTDBScanTrackClustering
 @details [does something]
 @author: [name]
 @date: [date]
 */

#ifndef KTDBSCANTRACKCLUSTERING_HH_
#define KTDBSCANTRACKCLUSTERING_HH_

#include "KTProcessor.hh"

#include "KTDBScan.hh"
#include "KTMemberVariable.hh"
#include "KTSlot.hh"
#include "KTData.hh"

#include <set>
#include <vector>


namespace Katydid
{

    class KTKDTreeData;

    /*!
     @class KTDBScanTrackClustering
     @author N.S. Oblath

     @brief Clustering for finding tracks using the DBSCAN algorithm

     @details
     Normalization of the axes:
     The DBSCAN algorithm expects expects that all of the dimensions that describe a points will have the same scale,
     such that a single radius parameter can describe a sphere in the parameter space that's used to cluster points together.
     For track clustering, one radius is given to define the circle around points to be clustered together.
     For clustering, a scaling factor is calculated for each axis such that the ellipse formed by the two radii is
     scaled to a unit circle.  Those scaling factors are applied to every point before the data is passed to the
     DBSCAN algorithm.

     Configuration name: "dbscan-track-clustering"

     Available configuration values:
     - "radius": double -- double used to define the circle around points to be clustered together
     - "min-points": unsigned int -- minimum number of points required to have a cluster

     Slots:
     - "points": void (shared_ptr<KTData>) -- If this is a new acquisition, triggers the clustering algorithm; Adds points to the internally-stored set of points; Requires KTSliceHeader and KTDiscriminatedPoints1DData.
     - "do-clustering": void () -- Triggers clustering algorithm

     Signals:
     - "track": void (shared_ptr<KTData>) -- Emitted for each cluster found; Guarantees KTSparseWaterfallCandidateData.
     - "clustering-done": void () -- Emitted when track clustering is complete
    */

    class KTDBScanTrackClustering : public Nymph::KTProcessor
    {
        public:
            //typedef KTSparseDistanceMatrix< double > DistanceMatrix;
            //typedef DistanceMatrix::Point Point;
            //typedef DistanceMatrix::Points Points;

            const static unsigned fNDimensions;

        public:
            KTDBScanTrackClustering(const std::string& name = "dbscan-track-clustering");
            virtual ~KTDBScanTrackClustering();

            bool Configure(const scarab::param_node* node);

            MEMBERVARIABLE(unsigned, MinPoints);
            MEMBERVARIABLE(double, Radius);
            MEMBERVARIABLE_NOSET(unsigned, DataCount);            
            //MEMBERVARIABLEREF(Point, Radii);

        public:
            bool DoClustering(KTKDTreeData& data);

            // Store point information locally
            //bool TakePoints(KTSliceHeader& slHeader, KTDiscriminatedPoints1DData& discPoints);
            //bool TakePoint(double time, double frequency /*, double amplitude*/, unsigned component=0);



            //void SetNComponents(unsigned nComps);
            //void SetTimeBinWidth(double bw);
            //void SetFreqBinWidth(double bw);

            //bool Run();

            //bool DoClustering();

            const std::set< Nymph::KTDataPtr >& GetCandidates() const;

        private:

            //double fTimeBinWidth;
            //double fFreqBinWidth;

            //std::vector< Points > fCompPoints; // points vectors for each component

            std::set< Nymph::KTDataPtr > fCandidates;

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
            Nymph::KTSlotDataOneType< KTKDTreeData > fClusterKDTreeSlot;
            //Nymph::KTSlotDataTwoTypes< KTSliceHeader, KTDiscriminatedPoints1DData > fTakePointSlot;
            //Nymph::KTSlotDataOneType< KTInternalSignalWrapper > fDoClusterSlot;

            //void DoClusteringSlot();

    };
/*
    inline void KTDBScanTrackClustering::SetTimeBinWidth(double bw)
    {
        fTimeBinWidth = bw;
        return;
    }
    inline void KTDBScanTrackClustering::SetFreqBinWidth(double bw)
    {
        fFreqBinWidth = bw;
        return;
    }
*/
    inline const std::set< Nymph::KTDataPtr >& KTDBScanTrackClustering::GetCandidates() const
    {
        return fCandidates;
    }


}
 /* namespace Katydid */
#endif /* KTDBSCANTRACKCLUSTERING_HH_ */
