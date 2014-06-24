/**
 @file KTDBScanTrackClustering.hh
 @brief Contains KTDBScanTrackClustering
 @details [does something]
 @author: [name]
 @date: [date]
 */

#ifndef KTDBSCANTRACKCLUSTERING_HH_
#define KTDBSCANTRACKCLUSTERING_HH_

#include "KTPrimaryProcessor.hh"

#include "KTDBScan.hh"
#include "KTSlot.hh"

#include <vector>

namespace Katydid
{
    class KTSliceHeader;
    class KTDiscriminatedPoints1DData;
    class KTPStoreNode;

    /*!
     @class KTDBScanTrackClustering
     @author N.S. Oblath

     @brief Clustering for finding tracks using the DBSCAN algorithm

     @details

     Configuration name: "dbscan-track-clustering"

     Available configuration values:
     - "epsilon": double --
     - "min-points": unsigned int --

     Slots:
     - "header": void (const KTEggHeader* header) -- [what it does]
     - "points": void (shared_ptr<KTData>) -- [what it does]; Requires [input data type]; Adds [output data type]; Emits signal "[signal-name]"

     Signals:
     - "[signal-name]": void (shared_ptr<KTData>) -- Emitted upon [whatever was done]; Guarantees [output data type].
    */

    class KTDBScanTrackClustering : public KTPrimaryProcessor
    {
        public:
            KTDBScanTrackClustering(const std::string& name = "dbscan-track-clustering");
            virtual ~KTDBScanTrackClustering();

            bool Configure(const KTParamNode* node);

            double GetEpsilon() const;
            void SetEpsilon(double eps);

            unsigned GetMinPoints() const;
            void SetMinPoints(unsigned pts);

        private:
            void UpdateComponents();

            // eps radiuus
            // Two points are neighbors if the distance
            // between them does not exceed threshold value.
            double fEpsilon;

            //minimum number of points
            unsigned fMinPoints;

        public:
            // Store point information locally
            bool TakePoints(KTSliceHeader& slHeader, KTDiscriminatedPoints1DData& discPoints);

            bool Run();

            bool DoClustering();

        private:
            std::vector< KTDBScan > fComponents;

            std::vector< KTWaterfallCandidateData > fCandidates;

            //***************
            // Signals
            //***************

        private:
            KTSignalData fTrackSignal;

            //***************
            // Slots
            //***************

        private:
            KTSlotDataTwoTypes< KTSliceHeader, KTDiscriminatedPoints1DData > fTakePointSlot;

    };

    inline double KTDBScanTrackClustering::GetEpsilon() const
    {
        return fEpsilon;
    }
    inline void KTDBScanTrackClustering::SetEpsilon(double eps)
    {
        fEpsilon = eps;
        UpdateComponents();
        return;
    }

    inline unsigned KTDBScanTrackClustering::GetMinPoints() const
    {
        return fMinPoints;
    }
    inline void KTDBScanTrackClustering::SetMinPoints(unsigned pts)
    {
        fMinPoints = pts;
        UpdateComponents();
        return;
    }

}
 /* namespace Katydid */
#endif /* KTDBSCANTRACKCLUSTERING_HH_ */
