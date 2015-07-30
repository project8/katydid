/*
 * KTKDTreePointCloud.hh
 *
 *  Created on: Aug 8, 2014
 *      Author: nsoblath
 *
 *  Point cloud class for the nanoflann-based k-d tree
 */

#ifndef KTKDTREEPOINTCLOUD_HH_
#define KTKDTREEPOINTCLOUD_HH_

//#include <iostream>

#include <utility>
#include <vector>

namespace Katydid
{
    using namespace Nymph;

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
        typedef std::vector< POINT > SetOfPoints;
        SetOfPoints fPoints;

        // Must return the number of data points
        inline size_t kdtree_get_point_count() const
        {
            return fPoints.size();
        }

        // Returns the distance between the vector "p1[0:size-1]" and the data point with index "idx_p2" stored in the class:
        inline coord_t kdtree_distance(const coord_t *p1, const size_t idx_p2, size_t size) const
        {
            const coord_t d0 = p1[0] - fPoints[idx_p2].fCoords[0];
            const coord_t d1 = p1[1] - fPoints[idx_p2].fCoords[1];
            //std::cout << "distance between (" << p1[0] << ", " << p1[1] << ") and pid " << idx_p2 << "(" << fPoints[idx_p2].fCoords[0] << ", " << fPoints[idx_p2].fCoords[1] << ") = " << d0 << "^2 + " << d1 << "^2 = " << d0*d0+d1*d1 << std::endl;
            return d0*d0 + d1*d1;
        }

        // Returns the dim'th component of the idx'th point in the class:
        // Since this is inlined and the "dim" argument is typically an immediate value, the
        //  "if/else's" are actually solved at compile time.
        inline coord_t kdtree_get_pt(const size_t idx, int dim) const
        {
            return fPoints[idx].fCoords[dim];
            //if (dim == 0) return derived().fPoints[idx].fCoords[0];
            //else return derived().fPoints[idx].fCoords[1];
        }

        // Optional bounding-box computation: return false to default to a standard bbox computation loop.
        //   Return true if the BBOX was already computed by the class and returned in "bb" so it can be avoided to redo it again.
        //   Look at bb.size() to find out the expected dimensionality (e.g. 2 or 3 for point clouds)
        template <class BBOX>
        bool kdtree_get_bbox(BBOX &bb) const { return false; }

    };

} /* namespace Katydid */

#endif /* KTKDTREEPOINTCLOUD_HH_ */
