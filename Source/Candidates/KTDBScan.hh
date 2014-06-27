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

    /*
    template <typename VEC_T>
    class Cosine
    {
        protected:
            typedef VEC_T vector_type;

            // this must be not directly accessible
            // since we want to provide a rich set of distances

            double GetDistance(const VEC_T v1, const VEC_T v2)
            {
                //std::cout << "dot=" << prec_inner_prod(v1, v2) << " norm_2=" << norm_2(v1) << "norm2=" << norm_2(v2) << std::endl;
                return prec_inner_prod(v1, v2) / (norm_2(v1) * norm_2(v2));
            };
    };
    */

    template <typename Distance_Policy>   // this allows to provide a static mechanism for pseudo-like
                                          // inheritance, which is optimal from a performance point of view.
    class Distance : Distance_Policy
    {
        public:

            double GetDistance(typename Distance_Policy::vector_type x, typename Distance_Policy::vector_type y)
            {
                return Distance_Policy::GetDistance(x, y);
            };

    };


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
            template < typename DistanceType >
            bool RunDBScan(const Points& points);

            void InitializeArrays(size_t nPoints);

            // assign each point to a new cluster
            void UniformPartition();

            // compute similarity
            template < typename DistanceType >
            void ComputeDistance(const Points& points);

            bool DoClustering();

            const std::vector< Cluster >& GetClusters() const;

        private:
            //
            // findNeighbors(PointId pid, double threshold)
            //
            // this can be implemented with reduced complexity by using R+trees
            //
            Neighbors FindNeighbors(PointId pid, double threshold);

            unsigned fNPoints;

            // noise-point vector
            std::vector< bool > fNoise;

            // visited-point vector
            std::vector< bool > fVisited;

            // mapping point_id -> clusterId
            std::vector< ClusterId > fPointIdToClusterId;

            // the collection of clusters
            std::vector< Cluster > fClusters;

            // distance matrix
            boost::numeric::ublas::matrix< double > fDist;

            friend std::ostream& operator<<(std::ostream& stream, const KTDBScan& cs);
            friend std::ostream& operator<<(std::ostream& stream, const KTDBScan::Cluster& cluster);
            friend std::ostream& operator<<(std::ostream& stream, const KTDBScan::Point& point);
    };

    std::ostream& operator<<(std::ostream& stream, const KTDBScan& cs);
    std::ostream& operator<<(std::ostream& stream, const KTDBScan::Cluster& cluster);
    std::ostream& operator<<(std::ostream& stream, const KTDBScan::Point& point);


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
    bool KTDBScan::RunDBScan(const Points& points)
    {
        InitializeArrays(points.size());
        //UniformPartition();
        ComputeDistance< DistanceType >(points);
        return DoClustering();
    }


    template < typename DistanceType >
    void KTDBScan::ComputeDistance(const Points& points)
    {
        Distance< DistanceType > dist;
        for (unsigned i=0; i < fNPoints; ++i)
        {
            for (unsigned j=i+1; j < fNPoints; ++j)
            {
                fDist(j, i) = fDist(i, j) = dist.GetDistance(points[i], points[j]);
                std::cout << "dist(" << i << ", " << j << ") = dist( " << points[i] << ", " << points[j] << " ) = " << fDist(i, j) << std::endl;
            }
        }
    };

    inline const std::vector< KTDBScan::Cluster >& KTDBScan::GetClusters() const
    {
        return fClusters;
    }









    /*
    class DBSCAN
    {
    public:
        typedef boost::numeric::ublas::vector<double> FeaturesWeights;
        typedef boost::numeric::ublas::matrix<double> ClusterData;
        typedef boost::numeric::ublas::matrix<double> DistanceMatrix;
        typedef std::vector<uint32_t> Neighbors;
        typedef std::vector<int32_t> Labels;

        static ClusterData gen_cluster_data( size_t features_num, size_t elements_num );
        static FeaturesWeights std_weights( size_t s );

        DBSCAN(double eps, size_t min_elems, int num_threads=1);
        DBSCAN();
        ~DBSCAN();

        void init(double eps, size_t min_elems, int num_threads=1);
        void fit( const ClusterData & C );
        void fit_precomputed( const DistanceMatrix & D );
        void wfit( const ClusterData & C, const FeaturesWeights & W );
        void reset();

        const Labels & get_labels() const;

    private:

        void prepare_labels( size_t s );
        const DistanceMatrix calc_dist_matrix( const ClusterData & C, const FeaturesWeights & W );
        Neighbors find_neighbors(const DistanceMatrix & D, uint32_t pid);
        void dbscan( const DistanceMatrix & dm );

        double m_eps;
        size_t m_min_elems;
        int m_num_threads;
        double m_dmin;
        double m_dmax;

        Labels m_labels;
    };

    std::ostream& operator<<(std::ostream& o, DBSCAN & d);

    */









} /* namespace Katydid */
#endif /* KTDBSCAN_HH_ */
