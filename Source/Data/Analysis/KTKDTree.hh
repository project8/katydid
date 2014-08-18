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

//#include <iostream>

#include <utility>

namespace Katydid
{
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

                inline void push_back(PointId pid, TYPE dist=0.) {fIndicesAndDists.push_back(std::make_pair< PointId, TYPE >(pid, dist));}

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

        virtual void FindNeighbors(nanoflann::KNNResultSet< TYPE >& result, const TYPE* vec, const nanoflann::SearchParams& searchParams) const = 0;
        virtual void FindNeighbors(nanoflann::RadiusResultSet< TYPE >& result, const TYPE* vec, const nanoflann::SearchParams& searchParams) const = 0;
        virtual void knnSearch(const TYPE* query_point, const size_t num_closest, size_t* out_indices, TYPE* out_distances_sq, const int nChecks_IGNORED=10) const = 0;
        virtual size_t RadiusSearch(const TYPE* query_point, const TYPE radius, std::vector< std::pair< size_t, TYPE > >& IndicesDists, const nanoflann::SearchParams& searchParams) const = 0;
        virtual Neighbors FindNeighbors(PointId pid, TYPE radius) const = 0;
        virtual Neighbors knnSearch(PointId pid, size_t N) const = 0;

    };

    template< typename TYPE, typename DatasetAdaptor >
    struct KTTreeIndexManhattan : KTTreeIndex< TYPE >, nanoflann::KDTreeSingleIndexAdaptor< nanoflann::L1_Adaptor< TYPE, DatasetAdaptor >, DatasetAdaptor, 2 >
    {
        typedef typename KTTreeIndex< TYPE >::PointId PointId;
        typedef typename KTTreeIndex< TYPE >::Neighbors Neighbors;
        typedef nanoflann::KDTreeSingleIndexAdaptor< nanoflann::L1_Adaptor< TYPE, DatasetAdaptor >, DatasetAdaptor, 2 > NanoflannIndex;

        KTTreeIndexManhattan(const int dimensionality, const DatasetAdaptor& inputData, const nanoflann::KDTreeSingleIndexAdaptorParams& params = nanoflann::KDTreeSingleIndexAdaptorParams()) :
            NanoflannIndex(dimensionality, inputData, params)
        {}
        virtual ~KTTreeIndexManhattan() {}

        void FreeIndex() {NanoflannIndex::freeIndex();}
        void BuildIndex() {NanoflannIndex::buildIndex();}

        size_t size() const {return NanoflannIndex::size();}
        size_t Veclen() const {return NanoflannIndex::veclen();}
        size_t UsedMemory() const {return NanoflannIndex::usedMemory();}

        void SaveIndex(FILE* stream) {NanoflannIndex::saveIndex(stream);}
        void LoadIndex(FILE* stream) {NanoflannIndex::loadIndex(stream);}

        void FindNeighbors(nanoflann::KNNResultSet< TYPE >& result, const TYPE* vec, const nanoflann::SearchParams& searchParams) const
        {
            NanoflannIndex::findNeighbors(result, vec, searchParams);
        }
        virtual void FindNeighbors(nanoflann::RadiusResultSet< TYPE >& result, const TYPE* vec, const nanoflann::SearchParams& searchParams) const
        {
            NanoflannIndex::findNeighbors(result, vec, searchParams);
        }
        void knnSearch(const TYPE* query_point, const size_t num_closest, size_t* out_indices, TYPE* out_distances_sq, const int nChecks_IGNORED=10) const
        {
            NanoflannIndex::knnSearch(query_point, num_closest, out_indices, out_distances_sq, nChecks_IGNORED);
        }
        size_t RadiusSearch(const TYPE* query_point, const TYPE radius, std::vector< std::pair< size_t, TYPE > >& IndicesDists, const nanoflann::SearchParams& searchParams) const
        {
            return NanoflannIndex::radiusSearch(query_point, radius, IndicesDists, searchParams);
        }
        Neighbors FindNeighbors(PointId pid, TYPE radius) const
        {
            Neighbors neighbors;
            NanoflannIndex::radiusSearch(NanoflannIndex::dataset.fPoints[pid].fCoords, radius, neighbors.GetIndicesAndDists(), nanoflann::SearchParams(32, 0, true));
            return neighbors;
        }

        Neighbors knnSearch(PointId pid, size_t nPoints) const
        {
            size_t* out_indices = new size_t[nPoints];
            TYPE* out_distances_sq = new TYPE[nPoints];
            const int nChecks_IGNORED = 10;
            NanoflannIndex::knnSearch(NanoflannIndex::dataset.fPoints[pid].fCoords, nPoints, out_indices, out_distances_sq, nChecks_IGNORED);

            Neighbors neighbors;
            for (unsigned iPoint = 0; iPoint < nPoints; ++iPoint)
            {
                neighbors.GetIndicesAndDists().push_back(std::make_pair< size_t, TYPE > (out_indices[iPoint], out_distances_sq[iPoint]));
            }

            delete [] out_indices;
            delete [] out_distances_sq;
            return neighbors;
        }
    };

    template< typename TYPE, typename DatasetAdaptor >
    struct KTTreeIndexEuclidean : KTTreeIndex< TYPE >,
            nanoflann::KDTreeSingleIndexAdaptor< nanoflann::L2_Simple_Adaptor< TYPE, DatasetAdaptor >, DatasetAdaptor, 2 >
    {
        typedef typename KTTreeIndex< TYPE >::PointId PointId;
        typedef typename KTTreeIndex< TYPE >::Neighbors Neighbors;
        typedef nanoflann::KDTreeSingleIndexAdaptor< nanoflann::L2_Simple_Adaptor< TYPE, DatasetAdaptor >, DatasetAdaptor, 2 > NanoflannIndex;

        KTTreeIndexEuclidean(const int dimensionality, const DatasetAdaptor& inputData, const nanoflann::KDTreeSingleIndexAdaptorParams& params = nanoflann::KDTreeSingleIndexAdaptorParams()) :
            NanoflannIndex(dimensionality, inputData, params)
        {}
        virtual ~KTTreeIndexEuclidean() {}

        void FreeIndex() {NanoflannIndex::freeIndex();}
        void BuildIndex() {NanoflannIndex::buildIndex();}

        size_t size() const {return NanoflannIndex::size();}
        size_t Veclen() const {return NanoflannIndex::veclen();}
        size_t UsedMemory() const {return NanoflannIndex::usedMemory();}

        void SaveIndex(FILE* stream) {NanoflannIndex::saveIndex(stream);}
        void LoadIndex(FILE* stream) {NanoflannIndex::loadIndex(stream);}

        virtual void FindNeighbors(nanoflann::KNNResultSet< TYPE >& result, const TYPE* vec, const nanoflann::SearchParams& searchParams) const
        {
            NanoflannIndex::findNeighbors(result, vec, searchParams);
        }
        virtual void FindNeighbors(nanoflann::RadiusResultSet< TYPE >& result, const TYPE* vec, const nanoflann::SearchParams& searchParams) const
        {
            NanoflannIndex::findNeighbors(result, vec, searchParams);
        }
        virtual void knnSearch(const TYPE* query_point, const size_t num_closest, size_t* out_indices, TYPE* out_distances_sq, const int nChecks_IGNORED=10) const
        {
            NanoflannIndex::knnSearch(query_point, num_closest, out_indices, out_distances_sq, nChecks_IGNORED);
        }
        size_t RadiusSearch(const TYPE* query_point, const TYPE radius, std::vector< std::pair< size_t, TYPE > >& IndicesDists, const nanoflann::SearchParams& searchParams) const
        {
            return NanoflannIndex::radiusSearch(query_point, radius, IndicesDists, searchParams);
        }
        Neighbors FindNeighbors(PointId pid, TYPE radius) const
        {
            Neighbors neighbors;
            NanoflannIndex::radiusSearch(NanoflannIndex::dataset.fPoints[pid].fCoords, radius, neighbors.GetIndicesAndDists(), nanoflann::SearchParams(32, 0, true));
            return neighbors;
        }

        Neighbors knnSearch(PointId pid, size_t nPoints) const
        {
            size_t* out_indices = new size_t[nPoints];
            TYPE* out_distances_sq = new TYPE[nPoints];
            const int nChecks_IGNORED = 10;
            NanoflannIndex::knnSearch(NanoflannIndex::dataset.fPoints[pid].fCoords, nPoints, out_indices, out_distances_sq, nChecks_IGNORED);

            Neighbors neighbors;
            for (unsigned iPoint = 0; iPoint < nPoints; ++iPoint)
            {
                neighbors.GetIndicesAndDists().push_back(std::make_pair< size_t, TYPE > (out_indices[iPoint], out_distances_sq[iPoint]));
            }

            delete [] out_indices;
            delete [] out_distances_sq;
            return neighbors;
        }
    };


} /* namespace Katydid */

#endif /* KTKDTREE_HH_ */
