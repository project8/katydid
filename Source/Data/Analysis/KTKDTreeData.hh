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
            struct PerComponentData
            {
                KTPointCloud< Point > fCloud;
                TreeIndex* fTreeIndex;
                unsigned fMaxLeafSize;
                DistanceMethod fDistanceMethod;
                PerComponentData() : fTreeIndex(NULL)
                {
                    fDistanceMethod = kEuclidean;
                    fMaxLeafSize = 10;
                }
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
            void RemovePoint(unsigned pid, unsigned component = 0);
            void RemovePoint(std::vector< size_t > points, unsigned component = 0);
            void CreateIndex(unsigned component = 0);
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
        return;
    }

    inline void KTKDTreeData::RemovePoint(unsigned pid, unsigned component)
    {
        std::vector< size_t > points;
        points.push_back(pid);
        this->RemovePoint(points, component);
        return;
    }

    inline void KTKDTreeData::RemovePoint(std::vector< size_t > points, unsigned component)
    {
        size_t index;
        for (size_t iPoint=0; iPoint <= points.size(); ++iPoint)
        {
            index = points.back();
            fComponentData[component].fCloud.fPoints.erase(fComponentData[component].fCloud.fPoints.begin() + index);
            points.pop_back();
        }
        this->CreateIndex();
        return;
    }

    inline KTKDTreeData& KTKDTreeData::SetNComponents(unsigned channels)
    {
        fComponentData.resize(channels);
        return *this;
    }

} /* namespace Katydid */

#endif /* KTKDTREEDATA_HH_ */
