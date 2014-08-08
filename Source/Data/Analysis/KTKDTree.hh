/*
 * KTKDTree.hh
 *
 *  Created on: Aug 8, 2014
 *      Author: nsoblath
 *
 *  nanoflann adapters
 */

#ifndef KTKDTREE_HH_
#define KTKDTREE_HH_

#include "nanoflann.hpp"

namespace Katydid
{

    // 2-dimensional point, templated for the type (e.g. double, float . . .)
    // To associate other information with a 2-D point, derive a class from this one.
    // Your derived class MUST typedef coord_t.
    template< typename TYPE >
    struct KT2DPoint
    {
            typedef TYPE coord_t;
            TYPE fX, fY;
    };

    // Generic point cloud, templated for the type of point
    // e.g. KTPointCloud< KT2DPoint >
    // If you've derived a class from KT2DPoint, use that as the template parameter
    template< typename POINT >
    struct KTPointCloud
    {
            typedef typename POINT::coord_t coord_t;
            std::vector< POINT > fPoints;
    };


    // And this is the "dataset to kd-tree" adaptor class
    // Derived MUST contain fPoints and type coord_t
    template <typename Derived>
    struct KT2DPointCloudAdaptor
    {
        typedef typename Derived::coord_t coord_t;

        const Derived &obj; //!< A const ref to the data set origin

        /// The constructor that sets the data set source
        KT2DPointCloudAdaptor(const Derived &obj_) : obj(obj_) { }

        /// CRTP helper method
        inline const Derived& derived() const { return obj; }

        // Must return the number of data points
        inline size_t kdtree_get_point_count() const { return derived().fPoints.size(); }

        // Returns the distance between the vector "p1[0:size-1]" and the data point with index "idx_p2" stored in the class:
        inline coord_t kdtree_distance(const coord_t *p1, const size_t idx_p2, size_t size) const
        {
            const coord_t d0 = p1[0] - derived().fPoints[idx_p2].fX;
            const coord_t d1 = p1[1] - derived().fPoints[idx_p2].fY;
            return d0*d0 + d1*d1;
        }

        // Returns the dim'th component of the idx'th point in the class:
        // Since this is inlined and the "dim" argument is typically an immediate value, the
        //  "if/else's" are actually solved at compile time.
        inline coord_t kdtree_get_pt(const size_t idx, int dim) const
        {
            if (dim == 0) return derived().fPoints[idx].fY;
            else return derived().fPoints[idx].fX;
        }

        // Optional bounding-box computation: return false to default to a standard bbox computation loop.
        //   Return true if the BBOX was already computed by the class and returned in "bb" so it can be avoided to redo it again.
        //   Look at bb.size() to find out the expected dimensionality (e.g. 2 or 3 for point clouds)
        template <class BBOX>
        bool kdtree_get_bbox(BBOX &bb) const { return false; }

    };


    //***************************************
    // TreeIndex classes, for type erasure
    //***************************************

    // There is some efficiency penalty for using virtual calls, but they should be done infrequently enough
    // or be for relatively expensive functions, such that it won't be noticeable.

    // Assumes the distance and element types are double.

    template< typename TYPE >
    struct KTTreeIndex
    {
        virtual ~KTTreeIndex() {}
        virtual void freeIndex() = 0;
        virtual void buildIndex() = 0;
        virtual size_t size() const = 0;
        virtual size_t veclen() const = 0;
        virtual size_t usedMemory() const = 0;
        virtual void saveIndex(FILE* stream) = 0;
        virtual void loadIndex(FILE* stream) = 0;

        virtual void findNeighbors(nanoflann::KNNResultSet< TYPE >& result, const TYPE* vec, nanoflann::SearchParams& searchParams) const = 0;
        virtual void knnSearch(const TYPE* query_point, const size_t num_closest, size_t* out_indices, TYPE* out_distances_sq, const int nChecks_IGNORED=10) const = 0;
        virtual size_t radiusSearch(const TYPE* query_point, const TYPE radius, std::vector< std::pair< size_t, TYPE > >& IndicesDists, const nanoflann::SearchParams& searchParams) const = 0;
    };

    template< typename TYPE, typename DatasetAdaptor >
    struct KTTreeIndexManhattan : KTTreeIndex< TYPE >
    {
        KTTreeIndexManhattan(const int dimensionality, const DatasetAdaptor& inputData, const nanoflann::KDTreeSingleIndexAdaptorParams& params = nanoflann::KDTreeSingleIndexAdaptorParams()) :
            fIndex(dimensionality, inputData, params)
        {}
        virtual ~KTTreeIndexManhattan() {}
        void freeIndex() {fIndex.freeIndex();}
        void buildIndex() {fIndex.buildIndex();}
        size_t size() const {return fIndex.size();}
        size_t veclen() const {return fIndex.veclen();}
        size_t usedMemory() const {return fIndex.usedMemory();}
        void saveIndex(FILE* stream) {fIndex.saveIndex(stream);}
        void loadIndex(FILE* stream) {fIndex.loadIndex(stream);}

        void findNeighbors(nanoflann::KNNResultSet< TYPE >& result, const TYPE* vec, nanoflann::SearchParams& searchParams) const
        {
            fIndex.findNeighbors(result, vec, searchParams);
        }
        void knnSearch(const TYPE* query_point, const size_t num_closest, size_t* out_indices, TYPE* out_distances_sq, const int nChecks_IGNORED=10) const
        {
            fIndex.knnSearch(query_point, num_closest, out_indices, out_distances_sq, nChecks_IGNORED);
        }
        size_t radiusSearch(const TYPE* query_point, const TYPE radius, std::vector< std::pair< size_t, TYPE > >& IndicesDists, const nanoflann::SearchParams& searchParams) const
        {
            return fIndex.radiusSearch(query_point, radius, IndicesDists, searchParams);
        }

        nanoflann::KDTreeSingleIndexAdaptor< nanoflann::L1_Adaptor< TYPE, DatasetAdaptor >, DatasetAdaptor, 2 > fIndex;
    };

    template< typename TYPE, typename DatasetAdaptor >
    struct KTTreeIndexEuclidean : KTTreeIndex< TYPE >
    {
        KTTreeIndexEuclidean(const int dimensionality, const DatasetAdaptor& inputData, const nanoflann::KDTreeSingleIndexAdaptorParams& params = nanoflann::KDTreeSingleIndexAdaptorParams()) :
            fIndex(dimensionality, inputData, params)
        {}
        virtual ~KTTreeIndexEuclidean() {}
        void freeIndex() {fIndex.freeIndex();}
        void buildIndex() {fIndex.buildIndex();}
        size_t size() const {return fIndex.size();}
        size_t veclen() const {return fIndex.veclen();}
        size_t usedMemory() const {return fIndex.usedMemory();}
        void saveIndex(FILE* stream) {fIndex.saveIndex(stream);}
        void loadIndex(FILE* stream) {fIndex.loadIndex(stream);}

        virtual void findNeighbors(nanoflann::KNNResultSet< TYPE >& result, const TYPE* vec, nanoflann::SearchParams& searchParams) const
        {
            fIndex.findNeighbors(result, vec, searchParams);
        }
        virtual void knnSearch(const TYPE* query_point, const size_t num_closest, size_t* out_indices, TYPE* out_distances_sq, const int nChecks_IGNORED=10) const
        {
            fIndex.knnSearch(query_point, num_closest, out_indices, out_distances_sq, nChecks_IGNORED);
        }
        virtual size_t radiusSearch(const TYPE* query_point, const TYPE radius, std::vector< std::pair< size_t, TYPE > >& IndicesDists, const nanoflann::SearchParams& searchParams) const
        {
            return fIndex.radiusSearch(query_point, radius, IndicesDists, searchParams);
        }

        nanoflann::KDTreeSingleIndexAdaptor< nanoflann::L2_Simple_Adaptor< TYPE, DatasetAdaptor >, DatasetAdaptor, 2 > fIndex;
    };


} /* namespace Katydid */

#endif /* KTKDTREE_HH_ */
