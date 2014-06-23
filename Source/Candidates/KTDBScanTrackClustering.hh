/**
 @file KTDBScanTrackClustering.hh
 @brief Contains KTDBScanTrackClustering
 @details [does something]
 @author: [name]
 @date: [date]
 */

#ifndef KTPROCESSORTEMPLATE_HH_
#define KTPROCESSORTEMPLATE_HH_

#include "KTPrimaryProcessor.hh"

#include "KTSlot.hh"

#include <vector>

namespace Katydid
{
    class KTSliceHeader;
    class KTDiscirminatedPoints1DData;
    class KTPStoreNode;

    /*!
     @class KTDBScanTrackClustering
     @author N.S. Oblath

     @brief Clustering for finding tracks using the DBSCAN algorithm

     @details

     Configuration name: "dbscan-track-clustering"

     Available configuration values:
     - "some-name": [type] -- [what it does]

     Slots:
     - "header": void (const KTEggHeader* header) -- [what it does]
     - "[slot-name]": void (shared_ptr<KTData>) -- [what it does]; Requires [input data type]; Adds [output data type]; Emits signal "[signal-name]"

     Signals:
     - "[signal-name]": void (shared_ptr<KTData>) -- Emitted upon [whatever was done]; Guarantees [output data type].
    */

    class KTDBScanTrackClustering : public KTPrimaryProcessor
    {
        public:
            // a single point is made up of vector of doubles
            typedef boost::numeric::ublas::vector< double > Point;
            typedef std::vector< Point > Points;

            typedef unsigned ClusterId;
            typedef unsigned PointId;

            // a cluster is a vector of pointid
            typedef std::vector< PointId > Cluster;
            // a set of Neighbors is a vector of pointid
            typedef std::vector< PointId > Neighbors;

        public:
            KTDBScanTrackClustering(const std::string& name = "dbscan-track-clustering");
            virtual ~KTDBScanTrackClustering();

            bool Configure(const KTPStoreNode* node);

            double GetEpsilon() const;
            void SetEpsilon(double eps);

            unsigned GetMinPoints() const;
            void SetMinPoints(unsigned pts);

        private:
            // eps radiuus
            // Two points are neighbors if the distance
            // between them does not exceed threshold value.
            double fEpsilon;

            //minimum number of points
            unsigned fMinPoints;

        public:
            // Store point information locally
            bool TakePoint(KTSliceHeader& slHeader, KTDiscriminatedPoints1DData& discPoints);

            bool TakePoints(const Points& points);
            bool TakePoint(const Point& point);

            void ResetPoints();

            bool Run();

            // assign each point to a new cluster
            void UniformPartition();

            // compute similarity
            template < typename DistanceType >
            void ComputeSimilarity(DistanceType& dist);

            //
            // findNeighbors(PointId pid, double threshold)
            //
            // this can be implemented with reduced complexity by using R+trees
            //
            Neighbors FindNeighbors(PointId pid, double threshold);

            bool DoClustering();

        private:
            // noise-point vector
            std::vector< bool > fNoise;

            // visited-point vector
            std::vector< bool > fVisited;

            // the collection of points we are working on
            Points fPoints;

            // mapping point_id -> clusterId
            std::vector< ClusterId > fPointIdToClusterId;

            // the collection of clusters
            std::vector< Cluster > fClusters;

            // simarity_matrix
            boost::numeric::ublas::matrix< double > fSim;

            friend
                std::ostream& operator << (std::ostream& o, const KTDBScanTrackClustering& c);


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
        return;
    }

    inline unsigned KTDBScanTrackClustering::GetMinPoints() const
    {
        return fMinPoints;
    }
    inline void KTDBScanTrackClustering::SetMinPoints(unsigned pts)
    {
        fMinPoints = pts;
        return;
    }

    template < typename DistanceType >
    void KTDBScanTrackClustering::ComputeSimilarity(DistanceType& dist)
    {
        unsigned size = fPoints.size();
        fSim.resize(size, size, false);
        for (unsigned i=0; i < size; ++i)
        {
            for (unsigned j=i+1; j < size; ++j)
            {
                fSim(j, i) = fSim(i, j) = dist.similarity(fPoints[i], fPoints[j]);
                //std::cout << "(" << i << ", " << j << ")=" << _sim(i, j) << " ";
            }
            //std::cout << std::endl;
        }
    };


}
 /* namespace Katydid */
#endif /* KTCOMPLEXFFTW_HH_ */
