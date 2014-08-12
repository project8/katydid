/**
 @file KTDistanceMatrix.hh
 @brief Contains DistanceMatrix classes
 @details Data structures for holding point-to-point distance and algorithms for calculating them
 @author: N.S. Oblath
 @date: Jun 24, 2014
 */

#ifndef KTDISTANCEMATRIX_HH_
#define KTDISTANCEMATRIX_HH_

#include <boost/foreach.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/numeric/ublas/symmetric.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/numeric/ublas/vector.hpp>

#include <cfloat>
#include <cmath>
#include <vector>

#include "KTLogger.hh"

namespace Katydid
{
    KTLOGGER(dbslog, "KTDBScan");

    // Euclidean distance
    template < typename VEC_T >
    class Euclidean
    {
        protected:
            typedef VEC_T vector_type;

            double GetDistance(const VEC_T v1, const VEC_T v2)
            {
                return norm_2(v1-v2);
            };

    };

    // Manhattan distance
    template < typename VEC_T >
    class Manhattan
    {
        protected:
            typedef VEC_T vector_type;

            double GetDistance(const VEC_T v1, const VEC_T v2)
            {
                return norm_1(v1-v2);
            };

    };

    template <typename Distance_Policy>   // this allows to provide a static mechanism for pseudo-like
    // inheritance, which is optimal from a performance point of view.
    class Distance : Distance_Policy
    {
        public:

        // distance function with equal weighting
        double GetDistance(typename Distance_Policy::vector_type x,
                           typename Distance_Policy::vector_type y)
        {
            return Distance_Policy::GetDistance(x, y);
        };

    };



    template< typename TYPE >
    struct KTSymmetricDistanceMatrix
    {
        typedef boost::numeric::ublas::symmetric_matrix< TYPE, boost::numeric::ublas::upper > matrix_type;
        typedef TYPE value_type;

        typedef boost::numeric::ublas::vector< TYPE > Point;
        typedef std::vector< Point > Points;

        typedef size_t PointId;
        typedef std::vector< PointId > Neighbors;

        matrix_type fDist;

        inline size_t size() const {return fDist.size1();}

        template < typename DistanceType >
        void ComputeDistances(const Points& points)
        {
            unsigned nPoints = points.size();
            fDist.resize(nPoints, nPoints, false);

            Distance< DistanceType > dist;
            for (unsigned i=0; i < nPoints; ++i)
            {
#ifndef NDEBUG
                if (i % 100 == 0)
                {
                    KTDEBUG(dbslog, "doing distance " << i << " of " << nPoints);
                }
#endif
                unsigned j = i + 1;
                // points are time-ordered, so point j should be equal to or after point i in time
                for (; j < nPoints /* && points[j](0) - points[i](0) < fRadius*/; ++j)
                {
                    fDist(i, j) = dist.GetDistance(points[i], points[j]);
                    //std::cout << "dist(" << i << ", " << j << ") = dist( " << points[i] << ", " << points[j] << " ) = " << fDist(i, j) << std::endl;
                }
                //for (; j < nPoints; ++j)
                //{
                //    fDist(i, j) = DBL_MAX;
                //}
            }
        }

        Neighbors FindNeighbors(PointId pid, TYPE threshold) const
        {
            Neighbors neighbors;

            for (unsigned j = 0; j < fDist.size1(); ++j)
            {
                if ((pid != j) && (fDist(pid, j)) < threshold)
                {
                    neighbors.push_back(j);
                    //std::cout << "sim(" << pid  << "," << j << ") = " << fSim(pid, j) << ">" << threshold << std::endl;
                }
            }

            return neighbors;
        }
    };

    template< typename TYPE >
    struct KTSparseDistanceMatrix
    {
        typedef boost::numeric::ublas::compressed_matrix< TYPE > matrix_type;
        typedef TYPE value_type;

        typedef boost::numeric::ublas::vector< TYPE > Point;
        typedef std::vector< Point > Points;

        typedef size_t PointId;
        typedef std::vector< PointId > Neighbors;

        matrix_type fDist;

        inline size_t size() const {return fDist.size1();}

        template < typename DistanceType >
        void ComputeDistances(const Points& points)
        {
            unsigned nPoints = points.size();
            fDist.resize(nPoints, nPoints, false);

            Distance< DistanceType > dist;
            for (unsigned i=0; i < nPoints; ++i)
            {
                //KTDEBUG(dbslog, "doing distance " << i << " of " << nPoints);
#ifndef NDEBUG
                if (i % 100 == 0)
                {
                    KTDEBUG(dbslog, "doing distance " << i << " of " << nPoints);
                }
#endif
                unsigned j = i + 1;
                // points are time-ordered, so point j should be equal to or after point i in time
                for (; j < nPoints /**/ && points[j](0) - points[i](0) < 1./*fRadius*/ /**/; ++j)
                {
                    fDist.insert_element(i, j, dist.GetDistance(points[i], points[j]));
                    fDist.insert_element(j, i, fDist(i, j));
                    /*if (i < 100 && j < 100)*/ //KTDEBUG(dbslog, "i = " << i << "  j = " << j << "  dist = " << fDist(i, j));
                    //std::cout << "dist(" << i << ", " << j << ") = dist( " << points[i] << ", " << points[j] << " ) = " << fDist(i, j) << std::endl;
                }
            }
        }

        Neighbors FindNeighbors(PointId pid, TYPE threshold) const
        {
            typedef boost::numeric::ublas::matrix_row< const matrix_type > dist_row;
            dist_row pidRow(fDist, pid);

            Neighbors neighbors;

            //unsigned nonzerocols = 0;
            for (typename dist_row::const_iterator colIt = pidRow.begin(); colIt != pidRow.end(); ++colIt)
            {
                //++nonzerocols;
                if (colIt.index() != pid && *colIt < threshold)
                {
                     neighbors.push_back(colIt.index());
                }
            }
            //KTDEBUG(dbslog, "pid: " << pid << ";  columns actually checked: " << nonzerocols);

            return neighbors;
        }
    };

} /* namespace Katydid */
#endif /* KTDISTANCEMATRIX_HH_ */
