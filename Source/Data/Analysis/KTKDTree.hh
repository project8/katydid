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
            TYPE fCoords[2];
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
        typedef Derived dataset_type;

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
            const coord_t d0 = p1[0] - derived().fPoints[idx_p2].fCoords[0];
            const coord_t d1 = p1[1] - derived().fPoints[idx_p2].fCoords[1];
            return d0*d0 + d1*d1;
        }

        // Returns the dim'th component of the idx'th point in the class:
        // Since this is inlined and the "dim" argument is typically an immediate value, the
        //  "if/else's" are actually solved at compile time.
        inline coord_t kdtree_get_pt(const size_t idx, int dim) const
        {
            if (dim == 0) return derived().fPoints[idx].fCoords[1];
            else return derived().fPoints[idx].fCoords[0];
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
        typedef size_t PointId;

        struct Neighbors
        {
                typedef std::vector< std::pair< PointId, TYPE > > IndicesAndDists;
                typedef PointId value_type;
                typedef TYPE dist_type;

                Neighbors() : fIndicesAndDists() {}
                Neighbors(size_t n) : fIndicesAndDists(n) {}
                Neighbors(IndicesAndDists& indDists) : fIndicesAndDists(indDists) {}

                IndicesAndDists fIndicesAndDists;

                inline PointId operator[](size_t ind) const {return fIndicesAndDists[ind].first;}
                inline TYPE dist(size_t ind) const {return fIndicesAndDists[ind].second;}

                inline void reserve(size_t n) {fIndicesAndDists.reserve(n);}
                inline size_t size() {return fIndicesAndDists.size();}

                inline bool empty() {return fIndicesAndDists.size() == 0;}
                inline void clear() {fIndicesAndDists.clear();}

                inline IndicesAndDists& GetIndicesAndDists() {return fIndicesAndDists;}
                inline const IndicesAndDists& GetIndicesAndDists() const {return fIndicesAndDists;}
        };

        virtual ~KTTreeIndex() {}

        virtual void FreeIndex() = 0;
        virtual void BuildIndex() = 0;

        virtual size_t size() const = 0;
        virtual size_t Veclen() const = 0;
        virtual size_t UsedMemory() const = 0;

        virtual void SaveIndex(FILE* stream) = 0;
        virtual void LoadIndex(FILE* stream) = 0;

        virtual void FindNeighbors(nanoflann::KNNResultSet< TYPE >& result, const TYPE* vec, nanoflann::SearchParams& searchParams) const = 0;
        virtual void knnSearch(const TYPE* query_point, const size_t num_closest, size_t* out_indices, TYPE* out_distances_sq, const int nChecks_IGNORED=10) const = 0;
        //virtual size_t RadiusSearch(const TYPE* query_point, const TYPE radius, std::vector< std::pair< size_t, TYPE > >& IndicesDists, const nanoflann::SearchParams& searchParams) const = 0;
        virtual Neighbors FindNeighbors(PointId pid, TYPE radius) const = 0;
        virtual Neighbors knnSearch(PointId pid, size_t N) const = 0;

        //virtual TYPE GetPointCoordinates(PointId pid) const = 0;// const = 0;
    };

    template< typename TYPE, typename DatasetAdaptor >
    struct KTTreeIndexManhattan : KTTreeIndex< TYPE >
    {
        typedef typename KTTreeIndex< TYPE >::PointId PointId;
        typedef typename KTTreeIndex< TYPE >::Neighbors Neighbors;

        KTTreeIndexManhattan(const int dimensionality, const DatasetAdaptor& inputData, const nanoflann::KDTreeSingleIndexAdaptorParams& params = nanoflann::KDTreeSingleIndexAdaptorParams()) :
            fData(inputData.derived()),
            fIndex(dimensionality, inputData, params)
        {}
        virtual ~KTTreeIndexManhattan() {}

        void FreeIndex() {fIndex.freeIndex();}
        void BuildIndex() {fIndex.buildIndex();}

        size_t size() const {return fIndex.size();}
        size_t Veclen() const {return fIndex.veclen();}
        size_t UsedMemory() const {return fIndex.usedMemory();}

        void SaveIndex(FILE* stream) {fIndex.saveIndex(stream);}
        void LoadIndex(FILE* stream) {fIndex.loadIndex(stream);}

        void FindNeighbors(nanoflann::KNNResultSet< TYPE >& result, const TYPE* vec, nanoflann::SearchParams& searchParams) const
        {
            fIndex.findNeighbors(result, vec, searchParams);
        }
        void knnSearch(const TYPE* query_point, const size_t num_closest, size_t* out_indices, TYPE* out_distances_sq, const int nChecks_IGNORED=10) const
        {
            fIndex.knnSearch(query_point, num_closest, out_indices, out_distances_sq, nChecks_IGNORED);
        }
        //size_t RadiusSearch(const TYPE* query_point, const TYPE radius, std::vector< std::pair< size_t, TYPE > >& IndicesDists, const nanoflann::SearchParams& searchParams) const
        Neighbors FindNeighbors(PointId pid, TYPE radius) const
        {
            Neighbors neighbors;
            fIndex.radiusSearch(fData.fPoints[pid].fCoords, radius, neighbors.GetIndicesAndDists(), nanoflann::SearchParams(32, 0, true));
            return neighbors;
        }

        Neighbors knnSearch(PointId pid, size_t nPoints) const
        {
            size_t* out_indices;//[nPoints];
            TYPE* out_distances_sq;//[nPoints];
            const int nChecks_IGNORED=10;
            fIndex.knnSearch(fData.fPoints[pid].fCoords, nPoints, out_indices, out_distances_sq, nChecks_IGNORED);

            Neighbors neighbors;
            for (unsigned iPoint = 0; iPoint < nPoints; ++iPoint)
            {
                neighbors.GetIndicesAndDists().push_back(std::make_pair< size_t, TYPE > (out_indices[iPoint], out_distances_sq[iPoint]));
            }
            return neighbors;
        }

 /*       const TYPE GetPointCoordinates(PointId pid)
        {
            return fData.fPoints[pid].fCoords;
        }
*/

        const typename DatasetAdaptor::dataset_type& fData;
        nanoflann::KDTreeSingleIndexAdaptor< nanoflann::L1_Adaptor< TYPE, DatasetAdaptor >, DatasetAdaptor, 2 > fIndex;
    };

    template< typename TYPE, typename DatasetAdaptor >
    struct KTTreeIndexEuclidean : KTTreeIndex< TYPE >
    {
        typedef typename KTTreeIndex< TYPE >::PointId PointId;
        typedef typename KTTreeIndex< TYPE >::Neighbors Neighbors;

        KTTreeIndexEuclidean(const int dimensionality, const DatasetAdaptor& inputData, const nanoflann::KDTreeSingleIndexAdaptorParams& params = nanoflann::KDTreeSingleIndexAdaptorParams()) :
            fData(inputData.derived()),
            fIndex(dimensionality, inputData, params)
        {}
        virtual ~KTTreeIndexEuclidean() {}

        void FreeIndex() {fIndex.freeIndex();}
        void BuildIndex() {fIndex.buildIndex();}

        size_t size() const {return fIndex.size();}
        size_t Veclen() const {return fIndex.veclen();}
        size_t UsedMemory() const {return fIndex.usedMemory();}

        void SaveIndex(FILE* stream) {fIndex.saveIndex(stream);}
        void LoadIndex(FILE* stream) {fIndex.loadIndex(stream);}

        virtual void FindNeighbors(nanoflann::KNNResultSet< TYPE >& result, const TYPE* vec, nanoflann::SearchParams& searchParams) const
        {
            fIndex.findNeighbors(result, vec, searchParams);
        }
        virtual void knnSearch(const TYPE* query_point, const size_t num_closest, size_t* out_indices, TYPE* out_distances_sq, const int nChecks_IGNORED=10) const
        {
            fIndex.knnSearch(query_point, num_closest, out_indices, out_distances_sq, nChecks_IGNORED);
        }
        //virtual size_t RadiusSearch(const TYPE* query_point, const TYPE radius, std::vector< std::pair< size_t, TYPE > >& IndicesDists, const nanoflann::SearchParams& searchParams) const
        Neighbors FindNeighbors(PointId pid, TYPE radius) const
        {
            Neighbors neighbors;
            fIndex.radiusSearch(fData.fPoints[pid].fCoords, radius, neighbors.GetIndicesAndDists(), nanoflann::SearchParams(32, 0, true));
            return neighbors;
        }

        Neighbors knnSearch(PointId pid, size_t nPoints) const
        {
            size_t* out_indices;//[nPoints];
            TYPE* out_distances_sq;//[nPoints];
            const int nChecks_IGNORED=10;
            fIndex.knnSearch(fData.fPoints[pid].fCoords, nPoints, out_indices, out_distances_sq, nChecks_IGNORED);

            Neighbors neighbors;
            for (unsigned iPoint = 0; iPoint < nPoints; ++iPoint)
            {
                neighbors.GetIndicesAndDists().push_back(std::make_pair< size_t, TYPE > (out_indices[iPoint], out_distances_sq[iPoint]));
            }
            return neighbors;
        }

/*
        const TYPE GetPointCoordinates(PointId pid)
        {
            return fData.fPoints[pid].fCoords;
        }
        */

        const typename DatasetAdaptor::dataset_type& fData;
        nanoflann::KDTreeSingleIndexAdaptor< nanoflann::L2_Simple_Adaptor< TYPE, DatasetAdaptor >, DatasetAdaptor, 2 > fIndex;
    };


} /* namespace Katydid */

#endif /* KTKDTREE_HH_ */
