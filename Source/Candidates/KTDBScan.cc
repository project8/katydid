/*
 * KTDBScan.cc
 *
 *  Created on: June 24, 2014
 *      Author: N.S. Oblath
 */

#include "KTDBScan.hh"

#include "KTLogger.hh"

//#include <boost/numeric/ublas/matrix_proxy.hpp>
//#include <boost/numeric/ublas/io.hpp>
//#include <boost/algorithm/minmax_element.hpp>
#include <boost/foreach.hpp>

using namespace boost::numeric;

namespace Katydid
{
    KTLOGGER(dbslog, "katydid.fft");




    KTDBScan::KTDBScan(double epsilon, unsigned minPoints) :
            fEpsilon(epsilon),
            fMinPoints(minPoints),
            fNPoints(0),
            fNoise(),
            fVisited(),
            fPointIdToClusterId(),
            fClusters(),
            fDist()
    {
    }

    KTDBScan::~KTDBScan()
    {
    }


    void KTDBScan::InitializeArrays(size_t nPoints)
    {
        fNPoints = nPoints;

        fDist.resize(fNPoints, fNPoints, false);

        fClusters.clear();
        fClusters.reserve(fNPoints);

        fVisited.clear();
        fVisited.resize(fNPoints, false);

        fNoise.clear();
        fNoise.resize(fNPoints, false);

        fPointIdToClusterId.clear();
        fPointIdToClusterId.resize(fNPoints, 0);

        return;
    }


    void KTDBScan::UniformPartition()
    {
        ClusterId cid = 0;
        for (PointId pid = 0; pid < fNPoints; ++pid)
        {
            // create a new cluster for this current point
            Cluster cluster;
            cluster.push_back(pid);
            fClusters.push_back(cluster);
            fPointIdToClusterId[pid] = cid++;
        }
    }

    KTDBScan::Neighbors KTDBScan::FindNeighbors(PointId pid, double threshold)
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

    bool KTDBScan::DoClustering()
    {

        ClusterId cid = 1;
        // foreach pid
        for (PointId pid = 0; pid < fNPoints; ++pid)
        {
            // not already visited
            if (! fVisited[pid])
            {

                fVisited[pid] = true;

                // get the neighbors
                Neighbors ne = FindNeighbors(pid, fEpsilon);

                // not enough support -> mark as noise
                if (ne.size() < fMinPoints)
                {
                    fNoise[pid] = true;
                }
                else
                {
                    //std::cout << "Point i=" << pid << " can be expanded " << std::endl;// = true;

                    // Add p to current cluster

                    Cluster cluster;              // a new cluster
                    cluster.push_back(pid);       // assign pid to cluster
                    fPointIdToClusterId[pid] = cid;

                    // go to neighbors
                    for (unsigned int i = 0; i < ne.size(); ++i)
                    {
                        PointId nPid = ne[i];

                        // not already visited
                        if (! fVisited[nPid])
                        {
                            fVisited[nPid] = true;

                            // go to neighbors
                            Neighbors ne1 = FindNeighbors(nPid, fEpsilon);

                            // enough support
                            if (ne1.size() >= fMinPoints)
                            {
                                //std::cout << "\t Expanding to pid=" << nPid << std::endl;
                                // join
                                BOOST_FOREACH(Neighbors::value_type n1, ne1)
                                {
                                    // join neighbors
                                    ne.push_back(n1);
                                    //std::cerr << "\tPushback pid=" << n1 << std::endl;
                                }
                                //std::cout << std::endl;
                            }
                        }

                        // not already assigned to a cluster
                        if (!fPointIdToClusterId[nPid])
                        {
                            //std::cout << "\tadding pid=" << nPid << std::endl;
                            cluster.push_back(nPid);
                            fPointIdToClusterId[nPid]=cid;
                        }
                    }

                    fClusters.push_back(cluster);
                    cid++;
                }
            } // if (!visited)
        } // for

        return true;
    }

    // single point output
    std::ostream& operator<<(std::ostream& stream, const KTDBScan::Point& point)
    {
        stream << "{ ";
        BOOST_FOREACH(KTDBScan::Point::value_type x, point)
        {
            stream << " " << x;
        }
        stream << " }, ";

        return stream;
    }

    // single cluster output
    std::ostream& operator<<(std::ostream& stream, const KTDBScan::Cluster& cluster)
    {
        stream << "[ ";
        BOOST_FOREACH(KTDBScan::PointId pid, cluster)
        {
            stream << " " << pid;
        }
        stream << " ]";

        return stream;
    }

    // clusters output
    std::ostream& operator<<(std::ostream& stream, const KTDBScan& cs)
    {
        KTDBScan::ClusterId cid = 1;
        BOOST_FOREACH(KTDBScan::Cluster c, cs.fClusters)
        {
            stream << "c(" << cid++ << ") = " << c << std::endl;
            /*
            BOOST_FOREACH(KTDBScan::PointId pid, c)
            {
                stream << cs.fPoints[pid];
            }
            stream << std::endl;
            */
        }
        return stream;
    }





/*
    DBSCAN::ClusterData DBSCAN::gen_cluster_data( size_t features_num, size_t elements_num )
    {
        DBSCAN::ClusterData cl_d( elements_num, features_num );

        for (size_t i = 0; i < elements_num; ++i)
        {
            for (size_t j = 0; j < features_num; ++j)
            {
                cl_d(i, j) = (-1.0 + rand() * (2.0) / RAND_MAX);
            }
        }

        return cl_d;
    }

    DBSCAN::FeaturesWeights DBSCAN::std_weights( size_t s )
    {
        // num cols
        DBSCAN::FeaturesWeights ws( s );

        for (size_t i = 0; i < s; ++i)
        {
            ws(i) = 1.0;
        }

        return ws;
    }

    DBSCAN::DBSCAN()
    : m_eps( 0 )
    , m_min_elems( 0 )
    , m_num_threads( 1 )
    , m_dmin(0.0)
    , m_dmax(0.0)
    {

    }

    void DBSCAN::init(double eps, size_t min_elems, int num_threads)
    {
        m_eps = eps;
        m_min_elems = min_elems;
        m_num_threads = num_threads;
    }

    DBSCAN::DBSCAN(double eps, size_t min_elems, int num_threads)
    : m_eps( eps )
    , m_min_elems( min_elems )
    , m_num_threads( num_threads )
    , m_dmin(0.0)
    , m_dmax(0.0)
    {
        reset();
    }

    DBSCAN::~DBSCAN()
    {

    }

    void DBSCAN::reset()
    {
        m_labels.clear();
    }

    void DBSCAN::prepare_labels( size_t s )
    {
        m_labels.resize(s);

        for (unsigned i = 0; i < m_labels.size(); ++i)
        {
            m_labels[i] = -1;
        }
    }

    const DBSCAN::DistanceMatrix DBSCAN::calc_dist_matrix( const DBSCAN::ClusterData & C, const DBSCAN::FeaturesWeights & W )
    {
        typedef std::pair< ublas::matrix_column< DBSCAN::ClusterData >::iterator, ublas::matrix_column< DBSCAN::ClusterData >::iterator > mm_col_iterators;
        typedef std::pair< ublas::matrix_row<DBSCAN::DistanceMatrix>::iterator, ublas::matrix_row<DBSCAN::DistanceMatrix>::iterator > mm_row_iterators;

        DBSCAN::ClusterData cl_d = C;

        KTDEBUG(dbslog, "Calculating scaled data");
        for (size_t i = 0; i < cl_d.size2(); ++i)
        {
            ublas::matrix_column< DBSCAN::ClusterData > col(cl_d, i);

            const mm_col_iterators r = boost::minmax_element( col.begin(), col.end() );

            double data_min = *r.first;
            double data_range = *r.second - *r.first;

            if (data_range == 0.0)
            {
                data_range = 1.0;
            }

            double scale = 1/data_range;
            double min = -1.0*data_min*scale;

            col *= scale;
            col.plus_assign( ublas::scalar_vector< ublas::matrix_column<DBSCAN::ClusterData>::value_type >(col.size(), min) );
        }

        // rows x rows
        DBSCAN::DistanceMatrix d_m( cl_d.size1(), cl_d.size1() );
        ublas::vector<double> d_max( cl_d.size1() );
        ublas::vector<double> d_min( cl_d.size1() );
        //ublas::vector< double > diff(cl_d.size2());

        KTDEBUG(dbslog, "Calculating distance matrix");
        for (size_t i = 0; i < cl_d.size1(); ++i)
        {
            ublas::matrix_row<DBSCAN::ClusterData> U (cl_d, i);
            for (size_t j = i; j < cl_d.size1(); ++j)
            {
                d_m(i, j) = 0.0;

                if (i != j)
                {
                    ublas::matrix_row<DBSCAN::ClusterData> V (cl_d, j);

                    d_m(i, j) = inner_prod(U-V, W);
                    //diff =  U - V;
                    //for (size_t k = 0; k < cl_d.size2(); ++k)
                    //{
                        //std::cout << k << std::endl;
                    //    d_m(i, j) += diff(k) * W[k];
                    //}

                    d_m(j, i) = d_m(i, j);
                }
            }

            ublas::matrix_row<DBSCAN::DistanceMatrix> cur_row = ublas::matrix_row<DBSCAN::DistanceMatrix>(d_m, i);
            mm_row_iterators mm = boost::minmax_element( cur_row.begin(), cur_row.end() );

            d_max(i) = *mm.second;
            d_min(i) = *mm.first;
        }

        m_dmin = *(min_element( d_min.begin(), d_min.end() ));
        m_dmax = *(max_element( d_max.begin(), d_max.end() ));

        m_eps = (m_dmax - m_dmin) * m_eps + m_dmin;

        return d_m;
    }

    DBSCAN::Neighbors DBSCAN::find_neighbors(const DBSCAN::DistanceMatrix & D, uint32_t pid)
    {
        Neighbors ne;

        for (uint32_t j = 0; j < D.size1(); ++j)
        {
            if  ( D(pid, j) <= m_eps )
            {
                ne.push_back(j);
            }
        }
        return ne;
    }

    void DBSCAN::dbscan( const DBSCAN::DistanceMatrix & dm )
    {
        std::vector<uint8_t> visited( dm.size1() );

        uint32_t cluster_id = 0;

        KTDEBUG(dbslog, "Clustering");
        for (uint32_t pid = 0; pid < dm.size1(); ++pid)
        {
            if ( !visited[pid] )
            {
                visited[pid] = 1;

                Neighbors ne = find_neighbors(dm, pid );

                if (ne.size() >= m_min_elems)
                {
                    m_labels[pid] = cluster_id;

                    for (uint32_t i = 0; i < ne.size(); ++i)
                    {
                        uint32_t nPid = ne[i];

                        if ( !visited[nPid] )
                        {
                            visited[nPid] = 1;

                            Neighbors ne1 = find_neighbors(dm, nPid);

                            if ( ne1.size() >= m_min_elems )
                            {
                                BOOST_FOREACH(Neighbors::value_type n1, ne1)
                                {
                                    ne.push_back(n1);
                                }
                            }
                        }

                        if ( m_labels[nPid] == -1 )
                        {
                            m_labels[nPid] = cluster_id;
                        }
                    }

                    ++cluster_id;
                }
            }
        }
    }

    void DBSCAN::fit( const DBSCAN::ClusterData & C )
    {
        const DBSCAN::FeaturesWeights W = DBSCAN::std_weights( C.size2() );
        wfit( C, W );
    }
    void DBSCAN::fit_precomputed( const DBSCAN::DistanceMatrix & D )
    {
        prepare_labels( D.size1() );
        dbscan( D );
    }

    void DBSCAN::wfit( const DBSCAN::ClusterData & C, const DBSCAN::FeaturesWeights & W )
    {
        prepare_labels( C.size1() );
        const DBSCAN::DistanceMatrix D = calc_dist_matrix( C, W );
        dbscan( D );
    }

    const DBSCAN::Labels & DBSCAN::get_labels() const
    {
        return m_labels;
    }

    std::ostream& operator<<(std::ostream& o, DBSCAN & d)
    {
        o << "[ ";
        for (unsigned i = 0; i < d.get_labels().size(); ++i)
        {
            o << " " << d.get_labels()[i];
        }
        o << " ] " << std::endl;

        return o;
    }

*/







} /* namespace Katydid */
