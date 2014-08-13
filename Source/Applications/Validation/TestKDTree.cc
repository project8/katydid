/*
 * TestKDTree.cc
 *
 *  Created on: Aug 12, 2014
 *      Author: nsoblath
 */

#include "KTKDTree.hh"

#include <iostream>
#include <time.h>

using namespace std;
using namespace Katydid;


struct Point : KT2DPoint< double >
{
    typedef KT2DPoint< double >::coord_t coord_t;
    Point() : fAmplitude(0.) {fCoords[0] = 0; fCoords[1] = 0;}
    Point(double x, double y, double amp) : fAmplitude(amp)
        {fCoords[0] = x; fCoords[1] = y; std::cout << "added point (" << fCoords[0] << "  " << fCoords[1] << ")";}
    double fAmplitude;
};

typedef KTPointCloud< Point > PointCloud;

typedef KTTreeIndex< double > TreeIndex;



void generateRandomPointCloud(PointCloud &pc, const size_t N, const double max_range = 10)
{
    std::cout << "Generating "<< N << " point cloud...";
    pc.fPoints.resize(N);
    srand(time(NULL));
    for (size_t i=0;i<N;i++)
    {
        pc.fPoints[i].fCoords[0] = max_range * (rand() % 1000) / double(1000);
        pc.fPoints[i].fCoords[1] = max_range * (rand() % 1000) / double(1000);
        if (i == 305445) std::cout << "point " << i << " " << pc.fPoints[i].fCoords[0] << "  " << pc.fPoints[i].fCoords[1] << std::endl;
    }

    std::cout << "done\n";
}


void kdtree_demo(const size_t N)
{
    PointCloud cloud;

    // Generate points:
    generateRandomPointCloud(cloud, N);

    double query_pt[3] = { 0.5, 0.5, 0.5};

    KTTreeIndex< double >* index = new KTTreeIndexEuclidean< double, PointCloud >(2, cloud, nanoflann::KDTreeSingleIndexAdaptorParams(10));

    index->BuildIndex();

    // do a knn search
    const size_t num_results = 1;
    size_t ret_index;
    double out_dist_sqr;
    nanoflann::KNNResultSet< double > resultSet(num_results);
    resultSet.init(&ret_index, &out_dist_sqr );
    index->FindNeighbors(resultSet, &query_pt[0], nanoflann::SearchParams(10));
    //index.knnSearch(query, indices, dists, num_results, mrpt_flann::SearchParams(10));

    std::cout << "knnSearch(nn="<<num_results<<"): \n";
    std::cout << "ret_index=" << ret_index << " out_dist_sqr=" << out_dist_sqr << endl;

    // radius search
    // Unsorted radius search:
    const double radius = 1;
    std::vector<std::pair<size_t,double> > indices_dists;

    cout << "Radius search using FindNeighbors" << endl;
    nanoflann::RadiusResultSet<double,size_t> resultSetRadius(radius,indices_dists);

    index->FindNeighbors(resultSetRadius, query_pt, nanoflann::SearchParams());

    // Get worst (furthest) point, without sorting:
    std::pair<size_t,double> worst_pair = resultSetRadius.worst_item();
    cout << "Worst pair: idx=" << worst_pair.first << " dist=" << worst_pair.second << endl;

}

int main(int argc, char** argv)
{
    kdtree_demo(1000000);

    return 0;
}

