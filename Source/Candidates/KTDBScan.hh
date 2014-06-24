/**
 @file KTDBScan.hh
 @brief Contains KTDBScan
 @details DBScan Clustering Algorithm
 @author: N.S. Oblath
 @date: Jun 24, 2014
 */

#ifndef KTDBSCAN_HH_
#define KTDBSCAN_HH_

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>

#include <vector>

namespace Katydid
{
    class KTSliceHeader;
    class KTDiscirminatedPoints1DData;
    class KTPStoreNode;

    /*!
     @class KTDBScan
     @author N.S. Oblath

     @brief DBSCAN Clustering Algorithm

     @details
     This implementation was adapted from:
     http://codingplayground.blogspot.com/2009/11/dbscan-clustering-algorithm.html
     Accessed on 6/5/2014.
     Code was provided without a license.
    */

    class KTDBScan
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
            KTDBScan(double epsilon = 1., unsigned minPoints = 1);
            virtual ~KTDBScan();

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
            bool TakePoints(const Points& points);
            bool TakePoint(const Point& point);

            void ResetPoints();

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

            const std::vector< Cluster >& GetClusters() const;

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
                std::ostream& operator << (std::ostream& o, const KTDBScan& c);


    };

    inline double KTDBScan::GetEpsilon() const
    {
        return fEpsilon;
    }
    inline void KTDBScan::SetEpsilon(double eps)
    {
        fEpsilon = eps;
        return;
    }

    inline unsigned KTDBScan::GetMinPoints() const
    {
        return fMinPoints;
    }
    inline void KTDBScan::SetMinPoints(unsigned pts)
    {
        fMinPoints = pts;
        return;
    }

    template < typename DistanceType >
    void KTDBScan::ComputeSimilarity(DistanceType& dist)
    {
        unsigned size = fPoints.size();
        fSim.resize(size, size, false);
        for (unsigned i=0; i < size; ++i)
        {
            for (unsigned j=i+1; j < size; ++j)
            {
                fSim(j, i) = fSim(i, j) = dist.GetSimilarity(fPoints[i], fPoints[j]);
                //std::cout << "(" << i << ", " << j << ")=" << _sim(i, j) << " ";
            }
            //std::cout << std::endl;
        }
    };

    const std::vector< KTDBScan::Cluster >& KTDBScan::GetClusters() const
    {
        return fClusters;
    }


    //
    // Euclidean distance
    template < typename VEC_T >
    class Euclidean
    {
        protected:
            typedef VEC_T vector_type;

            // this must be not directly accessible
            // since we want to provide a rich set of distances

            double GetDistance(const VEC_T v1, const VEC_T v2)
            {
                return norm_2(v1-v2);
            };
    };

    template <typename VEC_T>
    class Cosine
    {
        protected:
            typedef VEC_T vector_type;

            // this must be not directly accessible
            // since we want to provide a rich set of distances

            double GetDistance(const VEC_T v1, const VEC_T v2)
            {
                return 1.0 - (inner_prod(v1, v2) / (norm_2(v1) * norm_2(v2)));
            };

            double GetSimilarity(const VEC_T v1, const VEC_T v2)
            {
                //std::cout << "dot=" << prec_inner_prod(v1, v2) << " norm_2=" << norm_2(v1) << "norm2=" << norm_2(v2) << std::endl;
                return prec_inner_prod(v1, v2) / (norm_2(v1) * norm_2(v2));
            };
    };


    template <typename Distance_Policy>   // this allows to provide a static mechanism for pseudo-like
                                          // inheritance, which is optimal from a performance point of view.
    class Distance : Distance_Policy
    {
        public:
            double GetDistance(typename Distance_Policy::vector_type x, typename Distance_Policy::vector_type y)
            {
                return Distance_Policy::GetDistance(x, y);
            };

            double GetSimilarity(typename Distance_Policy::vector_type x, typename Distance_Policy::vector_type y)
            {
                return Distance_Policy::GetSimilarity(x, y);
            };

    };
}
 /* namespace Katydid */
#endif /* KTDBSCAN_HH_ */
