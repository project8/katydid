/*
 * KTKDTreeData.hh
 *
 *  Created on: Aug 7, 2014
 *      Author: nsoblath
 */

#ifndef KTKDTREEDATA_HH_
#define KTKDTREEDATA_HH_

#include "KTData.hh"

#include "KTPointCloud.hh"
#include "KTMemberVariable.hh"
#include "KTKDTree.hh"

#include <map>
#include <stdint.h>
#include <utility>
#include <vector>

namespace Katydid
{
    

    class KTKDTreeData : public Nymph::KTExtensibleData< KTKDTreeData >
    {
        public:
            /// Extend KT2DPoint to include amplitude
            struct Point : KT2DPoint< double >
            {
                typedef KT2DPoint< double >::coord_t coord_t;
                Point() : fAmplitude(0.), fTimeInAcq(0.), fNoiseFlag(false), fBinInSlice(0), fSliceNumber(0),
                        fMean(0.), fVariance(0.), fNeighborhoodAmplitude(0.)
                {
                    fCoords[0] = 0;
                    fCoords[1] = 0;
                }
                double fAmplitude;
                double fTimeInAcq;
                bool fNoiseFlag;
                unsigned fBinInSlice;
                uint64_t fSliceNumber;
                double fMean;
                double fVariance;
                double fNeighborhoodAmplitude;
                //uint64_t fAcquisitionID;
            };

            typedef KTPointCloud< Point >::SetOfPoints SetOfPoints;
            typedef KTTreeIndex< double > TreeIndex;

            enum DistanceMethod
            {
                kManhattan,
                kEuclidean
            };

            static const unsigned fNDimensions;
            MEMBERVARIABLE(uint64_t, AcquisitionID);

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

            MEMBERVARIABLE(bool, DataWillContinue);
            MEMBERVARIABLE(uint64_t, LastSlice);
            MEMBERVARIABLE(double, XScaling);
            MEMBERVARIABLE(double, YScaling);

            const SetOfPoints& GetSetOfPoints(unsigned component = 0) const;
            SetOfPoints& GetSetOfPoints(unsigned component = 0);
            const TreeIndex* GetTreeIndex(unsigned component = 0) const;
            TreeIndex* GetTreeIndex(unsigned component = 0);

            unsigned GetMaxLeafSize(unsigned component = 0) const;
            void SetMaxLeafSize(unsigned size, unsigned component = 0);

            DistanceMethod GetDistanceMethod(unsigned component = 0) const;
            void SetDistanceMethod(DistanceMethod dist, unsigned component = 0);

            unsigned GetNComponents() const;

            void AddPoint(const Point& point, unsigned component = 0);

            void RemovePoint(unsigned pid, unsigned component = 0);
            void RemovePoints(const std::vector< size_t >& points, unsigned component = 0);

            void ClearPoints(unsigned component = 0);
            void ClearIndex(unsigned component = 0);

            void FlagPoint(unsigned, unsigned component = 0, bool flag = true);
            void FlagPoints(const std::vector< size_t >& points, unsigned compoennt = 0, bool flag = true);

            void BuildIndex(unsigned component = 0);
            void BuildIndex(DistanceMethod, unsigned maxLeafSize = 10, unsigned component = 0);

            KTKDTreeData& SetNComponents(unsigned channels);

        private:
            std::vector< PerComponentData > fComponentData;

        public:
            static const std::string sName;
    };

    inline const std::vector< KTKDTreeData::Point >& KTKDTreeData::GetSetOfPoints(unsigned component) const
    {
        return fComponentData[component].fCloud.fPoints;
    }

    inline std::vector< KTKDTreeData::Point >& KTKDTreeData::GetSetOfPoints(unsigned component)
    {
        return fComponentData[component].fCloud.fPoints;
    }

    inline const KTKDTreeData::TreeIndex* KTKDTreeData::GetTreeIndex(unsigned component) const
    {
        return fComponentData[component].fTreeIndex;
    }

    inline KTKDTreeData::TreeIndex* KTKDTreeData::GetTreeIndex(unsigned component)
    {
        return fComponentData[component].fTreeIndex;
    }

    inline unsigned KTKDTreeData::GetMaxLeafSize(unsigned component) const
    {
        return fComponentData[component].fMaxLeafSize;
    }

    inline void KTKDTreeData::SetMaxLeafSize(unsigned size, unsigned component)
    {
        if (component >= fComponentData.size()) fComponentData.resize(component+1);
        fComponentData[component].fMaxLeafSize = size;
        return;
    }

    inline KTKDTreeData::DistanceMethod KTKDTreeData::GetDistanceMethod(unsigned component) const
    {
        return fComponentData[component].fDistanceMethod;
    }

    inline void KTKDTreeData::SetDistanceMethod(DistanceMethod dist, unsigned component)
    {
        if (component >= fComponentData.size()) fComponentData.resize(component+1);
        fComponentData[component].fDistanceMethod = dist;
        return;
    }

    inline unsigned KTKDTreeData::GetNComponents() const
    {
        return unsigned(fComponentData.size());
    }

    inline void KTKDTreeData::AddPoint(const Point& point, unsigned component)
    {
        if (component >= fComponentData.size()) fComponentData.resize(component+1);
        fComponentData[component].fCloud.fPoints.push_back(point);
        return;
    }

    inline void KTKDTreeData::RemovePoint(unsigned pid, unsigned component)
    {
        this->RemovePoints(std::vector< size_t >(1, pid), component);
        return;
    }

    inline void KTKDTreeData::FlagPoint(unsigned pid, unsigned component, bool flag)
    {
        fComponentData[component].fCloud.fPoints[pid].fNoiseFlag = flag;
        return;
    }

    inline KTKDTreeData& KTKDTreeData::SetNComponents(unsigned channels)
    {
        fComponentData.resize(channels);
        return *this;
    }

} /* namespace Katydid */

#endif /* KTKDTREEDATA_HH_ */
