/*
 * KTKDTreeData.hh
 *
 *  Created on: Aug 7, 2014
 *      Author: nsoblath
 */

#ifndef KTKDTREEDATA_HH_
#define KTKDTREEDATA_HH_

#include "KTData.hh"

#include "KTKDTree.hh"
#include "KTMemberVariable.hh"

#include <map>
#include <utility>
#include <vector>

namespace Katydid
{

    class KTKDTreeData : public KTExtensibleData< KTKDTreeData >
    {
        public:
            /// Extend KT2DPoint to include amplitude
            struct Point : KT2DPoint< double >
            {
                typedef KT2DPoint< double >::coord_t coord_t;
                Point() : fAmplitude(0.) {fCoords[0] = 0; fCoords[1] = 0;}
                Point(double x, double y, double amp) : fAmplitude(amp)
                    {fCoords[0] = x; fCoords[1] = y;}
                double fAmplitude;
            };

            typedef KTTreeIndex< double > TreeIndex;

            enum DistanceMethod
            {
                kManhattan,
                kEuclidean
            };

            static const unsigned fNDimensions;

        private:
            typedef KT2DPointCloudAdaptor< KTPointCloud< Point > > PointCloudAdaptor;

            struct PerComponentData
            {
                KTPointCloud< Point > fCloud;
                TreeIndex* fTreeIndex;
                PerComponentData() : fTreeIndex(NULL) {}
            };

        public:
            KTKDTreeData();
            virtual ~KTKDTreeData();

            MEMBERVARIABLE(double, XScaling);
            MEMBERVARIABLE(double, YScaling);

            const std::vector< Point >& GetSetOfPoints(unsigned component = 0) const;
            TreeIndex* GetTreeIndex(unsigned component = 0) const;

            unsigned GetNComponents() const;

            void AddPoint(const Point& point, unsigned component = 0);
            void CreateIndex(DistanceMethod, unsigned maxLeafSize = 10, unsigned component = 0);

            KTKDTreeData& SetNComponents(unsigned channels);

        private:
            std::vector< PerComponentData > fComponentData;

    };

    inline const std::vector< KTKDTreeData::Point >& KTKDTreeData::GetSetOfPoints(unsigned component) const
    {
        return fComponentData[component].fCloud.fPoints;
    }

    inline KTKDTreeData::TreeIndex* KTKDTreeData::GetTreeIndex(unsigned component) const
    {
        return fComponentData[component].fTreeIndex;
    }

    inline unsigned KTKDTreeData::GetNComponents() const
    {
        return unsigned(fComponentData.size());
    }

    inline void KTKDTreeData::AddPoint(const Point& point, unsigned component)
    {
        if (component >= fComponentData.size()) fComponentData.resize(component+1);
        fComponentData[component].fCloud.fPoints.push_back(point);
        unsigned pt = fComponentData[component].fCloud.fPoints.size()-1;
        std::cout << "added point " << pt << "; (" << fComponentData[component].fCloud.fPoints[pt].fCoords[0] << ", " << fComponentData[component].fCloud.fPoints[pt].fCoords[1] << std::endl;
    }

    inline KTKDTreeData& KTKDTreeData::SetNComponents(unsigned channels)
    {
        fComponentData.resize(channels);
        return *this;
    }

} /* namespace Katydid */

#endif /* KTKDTREEDATA_HH_ */
