/*
 * KTDiscriminatedPoints2DData.hh
 *
 *  Created on: Dec 12, 2012
 *      Author: nsoblath
 */

#ifndef KTDISCRIMINATEDPOINTS2DDATA_HH_
#define KTDISCRIMINATEDPOINTS2DDATA_HH_

#include "KTData.hh"

#include <map>
#include <utility>
#include <vector>

namespace Katydid
{
    
    struct KTPairCompare
    {
        bool operator() (const std::pair< unsigned, unsigned >& lhs, const std::pair< unsigned, unsigned >& rhs) const
        {
            return lhs.first < rhs.first || (lhs.first == rhs.first && lhs.second < rhs.second);
        }
    };

    class KTDiscriminatedPoints2DData : public Nymph::KTExtensibleData< KTDiscriminatedPoints2DData >
    {
        public:
            struct Point
            {
                double fAbscissa;
                double fOrdinate;
                double fApplicate;
                double fThreshold;
                double fMean;
                double fVariance;
                double fNeighborhoodAmplitude;
                Point(double abscissa, double ordinate, double applicate, double threshold, double mean, double variance, double neighborhoodAmplitude) : fAbscissa(abscissa), fOrdinate(ordinate), fApplicate(applicate), fThreshold(threshold), fMean(mean), fVariance(variance), fNeighborhoodAmplitude(neighborhoodAmplitude) {}
            };
            typedef std::map< std::pair< unsigned, unsigned >, Point > SetOfPoints;

        protected:
            struct PerComponentData
            {
                SetOfPoints fPoints;
            };

        public:
            KTDiscriminatedPoints2DData();
            virtual ~KTDiscriminatedPoints2DData();

            const SetOfPoints& GetSetOfPoints(unsigned component = 0) const;
            double GetThreshold(unsigned component = 0) const;

            unsigned GetNComponents() const;

            void AddPoint(unsigned binX, unsigned binY, const Point& point, unsigned component = 0);
        
            KTDiscriminatedPoints2DData& SetNComponents(unsigned channels);

        private:
            std::vector< PerComponentData > fComponentData;

            MEMBERVARIABLE(unsigned, NBinsX);
            MEMBERVARIABLE(unsigned, NBinsY);
            MEMBERVARIABLE(double, BinWidthX);
            MEMBERVARIABLE(double, BinWidthY);

        public:
            static const std::string sName;
    };

    inline const KTDiscriminatedPoints2DData::SetOfPoints& KTDiscriminatedPoints2DData::GetSetOfPoints(unsigned component) const
    {
        return fComponentData[component].fPoints;
    }
/*
    inline double KTDiscriminatedPoints2DData::GetThreshold(unsigned component) const
    {
        return fComponentData[component].fThreshold;
    }
*/
    inline unsigned KTDiscriminatedPoints2DData::GetNComponents() const
    {
        return unsigned(fComponentData.size());
    }

    inline void KTDiscriminatedPoints2DData::AddPoint(unsigned binX, unsigned binY, const Point& point, unsigned component)
    {
        if (component >= fComponentData.size()) fComponentData.resize(component+1);
        fComponentData[component].fPoints.insert(std::make_pair(std::make_pair(binX, binY), point));
    }

    inline KTDiscriminatedPoints2DData& KTDiscriminatedPoints2DData::SetNComponents(unsigned channels)
    {
        fComponentData.resize(channels);
        return *this;
    }

} /* namespace Katydid */

#endif /* KTDISCRIMINATEDPOINTS2DDATA_HH_ */
