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
                double fScale;
                double fBackground;
                double fCenter;
                double fCurvature;
                double fWidth;

                double fScaleErr;
                double fBackgroundErr;
                double fCenterErr;
                double fCurvatureErr;
                double fWidthErr;

                int fIsValid;
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

            double GetScale(unsigned component=0) const;
            void SetScale(double a, unsigned component=0);

            double GetBackground(unsigned component=0) const;
            void SetBackground(double b, unsigned component=0);

            double GetCenter(unsigned component=0) const;
            void SetCenter(double z0, unsigned component=0);

            double GetCurvature(unsigned component=0) const;
            void SetCurvature(double k, unsigned component=0);

            double GetWidth(unsigned component=0) const;
            void SetWidth(double sigma, unsigned component=0);

            double GetScaleErr(unsigned component=0) const;
            void SetScaleErr(double a, unsigned component=0);

            double GetBackgroundErr(unsigned component=0) const;
            void SetBackgroundErr(double b, unsigned component=0);

            double GetCenterErr(unsigned component=0) const;
            void SetCenterErr(double z0, unsigned component=0);

            double GetCurvatureErr(unsigned component=0) const;
            void SetCurvatureErr(double k, unsigned component=0);

            double GetWidthErr(unsigned component=0) const;
            void SetWidthErr(double sigma, unsigned component=0);

            bool GetIsValid(unsigned component=0) const;
            void SetIsValid(int valid, unsigned component=0);

            const SetOfPoints& GetSetOfPoints(unsigned component = 0) const;

            void AddPoint(unsigned bin, const Point& point, unsigned component = 0);

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

    inline double KTPowerFitData::GetScale(unsigned component) const
    {
        return fComponentData[component].fScale;
    }

    inline void KTPowerFitData::SetScale(double a, unsigned component)
    {
        fComponentData[component].fScale = a;
        return;
    }

    inline double KTPowerFitData::GetBackground(unsigned component) const
    {
        return fComponentData[component].fBackground;
    }

    inline void KTPowerFitData::SetBackground(double b, unsigned component)
    {
        fComponentData[component].fBackground = b;
        return;
    }

    inline double KTPowerFitData::GetCenter(unsigned component) const
    {
        return fComponentData[component].fCenter;
    }

    inline void KTPowerFitData::SetCenter(double z0, unsigned component)
    {
        fComponentData[component].fCenter = z0;
        return;
    }

    inline double KTPowerFitData::GetCurvature(unsigned component) const
    {
        return fComponentData[component].fCurvature;
    }

    inline void KTPowerFitData::SetCurvature(double k, unsigned component)
    {
        fComponentData[component].fCurvature = k;
        return;
    }

    inline double KTPowerFitData::GetWidth(unsigned component) const
    {
        return fComponentData[component].fWidth;
    }

    inline void KTPowerFitData::SetWidth(double sigma, unsigned component)
    {
        fComponentData[component].fWidth = sigma;
        return;
    }

    inline double KTPowerFitData::GetScaleErr(unsigned component) const
    {
        return fComponentData[component].fScaleErr;
    }

    inline void KTPowerFitData::SetScaleErr(double a, unsigned component)
    {
        fComponentData[component].fScaleErr = a;
        return;
    }

    inline double KTPowerFitData::GetBackgroundErr(unsigned component) const
    {
        return fComponentData[component].fBackgroundErr;
    }

    inline void KTPowerFitData::SetBackgroundErr(double b, unsigned component)
    {
        fComponentData[component].fBackgroundErr = b;
        return;
    }

    inline double KTPowerFitData::GetCenterErr(unsigned component) const
    {
        return fComponentData[component].fCenterErr;
    }

    inline void KTPowerFitData::SetCenterErr(double z0, unsigned component)
    {
        fComponentData[component].fCenterErr = z0;
        return;
    }

    inline double KTPowerFitData::GetCurvatureErr(unsigned component) const
    {
        return fComponentData[component].fCurvatureErr;
    }

    inline void KTPowerFitData::SetCurvatureErr(double k, unsigned component)
    {
        fComponentData[component].fCurvatureErr = k;
        return;
    }

    inline double KTPowerFitData::GetWidthErr(unsigned component) const
    {
        return fComponentData[component].fWidthErr;
    }

    inline void KTPowerFitData::SetWidthErr(double sigma, unsigned component)
    {
        fComponentData[component].fWidthErr = sigma;
        return;
    }    

    inline bool KTPowerFitData::GetIsValid(unsigned component) const
    {
        return fComponentData[component].fIsValid;
    }

    inline void KTPowerFitData::SetIsValid(int valid, unsigned component)
    {
        fComponentData[component].fIsValid = valid;
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
