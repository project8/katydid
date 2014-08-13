/***********************************************************************
 * Software License Agreement (BSD License)
 *
 * Copyright 2011 Jose Luis Blanco (joseluisblancoc@gmail.com).
 *   All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *************************************************************************/

#include <nanoflann.hpp>

#include <cstdlib>
#include <iostream>

using namespace std;
using namespace nanoflann;

    void dump_mem_usage()
    {
        FILE* f=fopen("/proc/self/statm","rt");
        if (!f) return;
        char str[300];
        size_t n=fread(str,1,200,f);
        str[n]=0;
        printf("MEM: %s\n",str);
        fclose(f);
    }

// This is an exampleof a custom data set class
template <typename T>
struct PointCloud
{
    typedef T coord_t; //!< The type of each coordinate

    struct Point
    {
        T  x,y,z;
    };

    std::vector<Point>  pts;
}; // end of PointCloud

// And this is the "dataset to kd-tree" adaptor class:
template <typename Derived>
struct PointCloudAdaptor
{
    typedef typename Derived::coord_t coord_t;

    const Derived &obj; //!< A const ref to the data set origin

    /// The constructor that sets the data set source
    PointCloudAdaptor(const Derived &obj_) : obj(obj_) { }

    /// CRTP helper method
    inline const Derived& derived() const { return obj; }

    // Must return the number of data points
    inline size_t kdtree_get_point_count() const { return derived().pts.size(); }

    // Returns the distance between the vector "p1[0:size-1]" and the data point with index "idx_p2" stored in the class:
    inline coord_t kdtree_distance(const coord_t *p1, const size_t idx_p2,size_t size) const
    {
        const coord_t d0=p1[0]-derived().pts[idx_p2].x;
        const coord_t d1=p1[1]-derived().pts[idx_p2].y;
        const coord_t d2=p1[2]-derived().pts[idx_p2].z;
        std::cout << "dist fcn!" << std::endl;
        return d0*d0+d1*d1+d2*d2;
    }

    // Returns the dim'th component of the idx'th point in the class:
    // Since this is inlined and the "dim" argument is typically an immediate value, the
    //  "if/else's" are actually solved at compile time.
    inline coord_t kdtree_get_pt(const size_t idx, int dim) const
    {
        if (dim==0) return derived().pts[idx].x;
        else if (dim==1) return derived().pts[idx].y;
        else return derived().pts[idx].z;
    }

    // Optional bounding-box computation: return false to default to a standard bbox computation loop.
    //   Return true if the BBOX was already computed by the class and returned in "bb" so it can be avoided to redo it again.
    //   Look at bb.size() to find out the expected dimensionality (e.g. 2 or 3 for point clouds)
    template <class BBOX>
    bool kdtree_get_bbox(BBOX &bb) const { return false; }

}; // end of PointCloudAdaptor


template <typename T>
void generateRandomPointCloud(PointCloud<T> &point, const size_t N, const T max_range = 10)
{
    std::cout << "Generating "<< N << " point cloud...";
    point.pts.resize(N);
    for (size_t i=0;i<N;i++)
    {
        point.pts[i].x = max_range * (rand() % 1000) / T(1000);
        point.pts[i].y = max_range * (rand() % 1000) / T(1000);
        point.pts[i].z = max_range * (rand() % 1000) / T(1000);
        std::cout << "point " << i << " " <<  point.pts[i].x << "  " <<  point.pts[i].y << "  " <<  point.pts[i].z << std::endl;
    }

    std::cout << "done\n";
}

template <typename num_t>
void kdtree_demo(const size_t N)
{
    PointCloud<num_t> cloud;

    // Generate points:
    generateRandomPointCloud(cloud, N);

    num_t query_pt[3] = { 0.5, 0.5, 0.5};

    typedef PointCloudAdaptor<PointCloud<num_t> > PC2KD;
    const PC2KD  pc2kd(cloud); // The adaptor

    // construct a kd-tree index:
    typedef KDTreeSingleIndexAdaptor<
        L2_Simple_Adaptor<num_t, PC2KD > ,
        PC2KD,
        3 /* dim */
        > my_kd_tree_t;

    dump_mem_usage();

    my_kd_tree_t   index(3 /*dim*/, pc2kd, KDTreeSingleIndexAdaptorParams(10 /* max leaf */) );
    index.buildIndex();
    dump_mem_usage();

    // do a knn search
    const size_t num_results = 1;
    size_t ret_index;
    num_t out_dist_sqr;
    nanoflann::KNNResultSet<num_t> resultSet(num_results);
    resultSet.init(&ret_index, &out_dist_sqr );
    index.findNeighbors(resultSet, &query_pt[0], nanoflann::SearchParams(10));
    //index.knnSearch(query, indices, dists, num_results, mrpt_flann::SearchParams(10));

    std::cout << "knnSearch(nn="<<num_results<<"): \n";
    std::cout << "ret_index=" << ret_index << " out_dist_sqr=" << out_dist_sqr << endl;


    // Unsorted radius search:
    const num_t radius = 1;
    std::vector<std::pair<size_t,num_t> > indices_dists;
    //RadiusResultSet<num_t,size_t> resultSet(radius,indices_dists);

    //index.findNeighbors(resultSet, query_pt, nanoflann::SearchParams());
    index.radiusSearch(query_pt, radius, indices_dists, nanoflann::SearchParams());

    // Get worst (furthest) point, without sorting:
    //std::pair<size_t,num_t> worst_pair = resultSet.worst_item();
    //cout << "Worst pair: idx=" << worst_pair.first << " dist=" << worst_pair.second << endl;
    cout << "Radius search; n neighbors: " << indices_dists.size() << endl;

}

int main(int argc, char** argv)
{
    kdtree_demo<float>(1000000);
    kdtree_demo<double>(1000000);

    return 0;
}


#if 0



/***********************************************************************
 * Software License Agreement (BSD License)
 *
 * Copyright 2011 Jose Luis Blanco (joseluisblancoc@gmail.com).
 *   All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *************************************************************************/

#include <nanoflann.hpp>

#include <cstdlib>
#include <iostream>

using namespace std;
using namespace nanoflann;

// This is an exampleof a custom data set class
template <typename T>
struct PointCloud
{
    struct Point
    {
        T  x,y,z;
    };

    std::vector<Point>  pts;

    // Must return the number of data points
    inline size_t kdtree_get_point_count() const { return pts.size(); }

    // Returns the distance between the vector "p1[0:size-1]" and the data point with index "idx_p2" stored in the class:
    inline T kdtree_distance(const T *p1, const size_t idx_p2,size_t size) const
    {
        const T d0=p1[0]-pts[idx_p2].x;
        const T d1=p1[1]-pts[idx_p2].y;
        const T d2=p1[2]-pts[idx_p2].z;
        std::cout << "dist fcn!" << std::endl;
        return d0*d0+d1*d1+d2*d2;
    }

    // Returns the dim'th component of the idx'th point in the class:
    // Since this is inlined and the "dim" argument is typically an immediate value, the
    //  "if/else's" are actually solved at compile time.
    inline T kdtree_get_pt(const size_t idx, int dim) const
    {
        if (dim==0) return pts[idx].x;
        else if (dim==1) return pts[idx].y;
        else return pts[idx].z;
    }

    // Optional bounding-box computation: return false to default to a standard bbox computation loop.
    //   Return true if the BBOX was already computed by the class and returned in "bb" so it can be avoided to redo it again.
    //   Look at bb.size() to find out the expected dimensionality (e.g. 2 or 3 for point clouds)
    template <class BBOX>
    bool kdtree_get_bbox(BBOX &bb) const { return false; }

};

template <typename T>
void generateRandomPointCloud(PointCloud<T> &point, const size_t N, const T max_range = 10)
{
    std::cout << "Generating "<< N << " point cloud...";
    point.pts.resize(N);
    for (size_t i=0;i<N;i++)
    {
        point.pts[i].x = max_range * (rand() % 1000) / T(1000);
        point.pts[i].y = max_range * (rand() % 1000) / T(1000);
        point.pts[i].z = max_range * (rand() % 1000) / T(1000);
    }

    std::cout << "done\n";
}

    void dump_mem_usage()
    {
        FILE* f=fopen("/proc/self/statm","rt");
        if (!f) return;
        char str[300];
        size_t n=fread(str,1,200,f);
        str[n]=0;
        printf("MEM: %s\n",str);
        fclose(f);
    }

template <typename num_t>
void kdtree_demo(const size_t N)
{
    PointCloud<num_t> cloud;

    // Generate points:
    generateRandomPointCloud(cloud, N);

    num_t query_pt[3] = { 0.5, 0.5, 0.5};


    // construct a kd-tree index:
    typedef KDTreeSingleIndexAdaptor<
        L2_Simple_Adaptor<num_t, PointCloud<num_t> > ,
        PointCloud<num_t>,
        3 /* dim */
        > my_kd_tree_t;

    dump_mem_usage();

    my_kd_tree_t   index(3 /*dim*/, cloud, KDTreeSingleIndexAdaptorParams(10 /* max leaf */) );
    index.buildIndex();

    dump_mem_usage();

    // do a knn search
    const size_t num_results = 1;
    size_t ret_index;
    num_t out_dist_sqr;
    nanoflann::KNNResultSet<num_t> resultSet(num_results);
    resultSet.init(&ret_index, &out_dist_sqr );
    index.findNeighbors(resultSet, &query_pt[0], nanoflann::SearchParams(10));

    std::cout << "knnSearch(nn="<<num_results<<"): \n";
    std::cout << "ret_index=" << ret_index << " out_dist_sqr=" << out_dist_sqr << endl;


    {
        // Unsorted radius search:
        const num_t radius = 1;
        std::vector<std::pair<size_t,num_t> > indices_dists;
        RadiusResultSet<num_t,size_t> resultSet(radius,indices_dists);

        index.findNeighbors(resultSet, query_pt, nanoflann::SearchParams());

        // Get worst (furthest) point, without sorting:
        std::pair<size_t,num_t> worst_pair = resultSet.worst_item();
        cout << "Worst pair: idx=" << worst_pair.first << " dist=" << worst_pair.second << endl;
    }

}

int main(int argc, char** argv)
{
    kdtree_demo<float>(1000000);
    kdtree_demo<double>(1000000);

    return 0;
}

#endif
