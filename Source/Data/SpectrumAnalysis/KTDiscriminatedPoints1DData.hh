/*
 * KTDiscriminatedPoints1DData.hh
 *
 *  Created on: Dec 12, 2012
 *      Author: nsoblath
 */

#ifndef KTDISCRIMINATEDPOINTS1DDATA_HH_
#define KTDISCRIMINATEDPOINTS1DDATA_HH_

#include "KTData.hh"

#include <map>
#include <utility>
#include <vector>

namespace Katydid
{
    

    class KTDiscriminatedPoints1DData : public Nymph::KTExtensibleData< KTDiscriminatedPoints1DData >
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
            KTDiscriminatedPoints1DData();
            virtual ~KTDiscriminatedPoints1DData();

            const SetOfPoints& GetSetOfPoints(unsigned component = 0) const;

            unsigned GetNComponents() const;

            void AddPoint(unsigned bin, const Point& point, unsigned component = 0);

            KTDiscriminatedPoints1DData& SetNComponents(unsigned channels);

            unsigned GetNBins() const;
            double GetBinWidth() const;

            void SetNBins(unsigned nBins);
            void SetBinWidth(double binWidth);

        private:
            std::vector< PerComponentData > fComponentData;

            unsigned fNBins;
            double fBinWidth;

        public:
            static const std::string sName;
    };

    inline const KTDiscriminatedPoints1DData::SetOfPoints& KTDiscriminatedPoints1DData::GetSetOfPoints(unsigned component) const
    {
        return fComponentData[component].fPoints;
    }

    inline unsigned KTDiscriminatedPoints1DData::GetNComponents() const
    {
        return unsigned(fComponentData.size());
    }

    inline void KTDiscriminatedPoints1DData::AddPoint(unsigned bin, const Point& point, unsigned component)
    {
        if (component >= fComponentData.size()) fComponentData.resize(component+1);
        fComponentData[component].fPoints.insert(std::make_pair(bin, point));
    }

    inline KTDiscriminatedPoints1DData& KTDiscriminatedPoints1DData::SetNComponents(unsigned channels)
    {
        fComponentData.resize(channels);
        return *this;
    }

    inline unsigned KTDiscriminatedPoints1DData::GetNBins() const
    {
        return fNBins;
    }

    inline double KTDiscriminatedPoints1DData::GetBinWidth() const
    {
        return fBinWidth;
    }

    inline void KTDiscriminatedPoints1DData::SetNBins(unsigned nBins)
    {
        fNBins = nBins;
        return;
    }

    inline void KTDiscriminatedPoints1DData::SetBinWidth(double binWidth)
    {
        fBinWidth = binWidth;
        return;
    }

} /* namespace Katydid */

#endif /* KTDISCRIMINATEDPOINTS1DDATA_HH_ */
