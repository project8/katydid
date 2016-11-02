/*
 * KTPowerFitData.hh
 *
 *  Created on: Oct 27, 2016
 *      Author: ezayas
 */

#ifndef KTPOWERFITDATA_HH
#define KTPOWERFITDATA_HH

#include "KTData.hh"

#include <inttypes.h>
#include <utility>
#include <vector>
#include <map>

namespace Katydid
{
    
    class KTPowerFitData : public Nymph::KTExtensibleData< KTPowerFitData >
    {
        public:
            struct Point
            {
                double fAbscissa;
                double fOrdinate;
                double fThreshold;
                Point(double abscissa, double ordinate, double threshold) : fAbscissa(abscissa), fOrdinate(ordinate), fThreshold(threshold) {}
            };
            typedef std::map< unsigned, Point > SetOfPoints;

        protected:
            struct PerComponentData
            {
                SetOfPoints fPoints;
            };

        public:
            KTPowerFitData();
            KTPowerFitData(const KTPowerFitData& orig);
            virtual ~KTPowerFitData();

            KTPowerFitData& operator=(const KTPowerFitData& rhs);

        public:

            unsigned GetNComponents() const;
            KTPowerFitData& SetNComponents(unsigned channels);

            double GetCurvature() const;
            void SetCurvature(double k);

            double GetWidth() const;
            void SetWidth(double sigma);

            const SetOfPoints& GetSetOfPoints(unsigned component = 0) const;

            void AddPoint(unsigned bin, const Point& point, unsigned component = 0);
        
        private:
            double fCurvature;
            double fWidth;

        private:
            std::vector< PerComponentData > fComponentData;


        public:
            static const std::string sName;
    };

    std::ostream& operator<<(std::ostream& out, const KTPowerFitData& hdr);

    inline unsigned KTPowerFitData::GetNComponents() const
    {
        return unsigned(fComponentData.size());
    }

    inline KTPowerFitData& KTPowerFitData::SetNComponents(unsigned channels)
    {
        fComponentData.resize(channels);
        return *this;
    }

    inline double KTPowerFitData::GetCurvature() const
    {
        return fCurvature;
    }

    inline void KTPowerFitData::SetCurvature(double k)
    {
        fCurvature = k;
        return;
    }

    inline double KTPowerFitData::GetWidth() const
    {
        return fWidth;
    }

    inline void KTPowerFitData::SetWidth(double sigma)
    {
        fWidth = sigma;
        return;
    }

    inline const KTPowerFitData::SetOfPoints& KTPowerFitData::GetSetOfPoints(unsigned component) const
    {
         return fComponentData[component].fPoints;
    }

    inline void KTPowerFitData::AddPoint(unsigned bin, const Point& point, unsigned component)
    {
        if (component >= fComponentData.size()) fComponentData.resize(component+1);
        fComponentData[component].fPoints.insert(std::make_pair(bin, point));
    }

} /* namespace Katydid */
#endif /* KTPOWERFITDATA_HH */
