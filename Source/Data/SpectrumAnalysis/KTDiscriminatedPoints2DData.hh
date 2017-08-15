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
                Point(double abscissa, double ordinate, double applicate, double threshold) : fAbscissa(abscissa), fOrdinate(ordinate), fApplicate(applicate), fThreshold(threshold) {}
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

            unsigned GetNBinsX() const;
            unsigned GetNBinsY() const;
            double GetBinWidthX() const;
            double GetBinWidthY() const;

            void SetNBinsX(unsigned nBins);
            void SetNBinsY(unsigned nBins);
            void SetBinWidthX(double binWidth);
            void SetBinWidthY(double binWidth);

        private:
            std::vector< PerComponentData > fComponentData;

            unsigned fNBinsX;
            unsigned fNBinsY;
            double fBinWidthX;
            double fBinWidthY;

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

    inline unsigned KTDiscriminatedPoints2DData::GetNBinsX() const
    {
        return fNBinsX;
    }

    inline unsigned KTDiscriminatedPoints2DData::GetNBinsY() const
    {
        return fNBinsY;
    }

    inline double KTDiscriminatedPoints2DData::GetBinWidthX() const
    {
        return fBinWidthX;
    }

    inline double KTDiscriminatedPoints2DData::GetBinWidthY() const
    {
        return fBinWidthY;
    }

    inline void KTDiscriminatedPoints2DData::SetNBinsX(unsigned nBins)
    {
        fNBinsX = nBins;
        return;
    }

    inline void KTDiscriminatedPoints2DData::SetNBinsY(unsigned nBins)
    {
        fNBinsY = nBins;
        return;
    }

    inline void KTDiscriminatedPoints2DData::SetBinWidthX(double binWidth)
    {
        fBinWidthX = binWidth;
        return;
    }

    inline void KTDiscriminatedPoints2DData::SetBinWidthY(double binWidth)
    {
        fBinWidthY = binWidth;
        return;
    }

    class KTDiscriminatedPoints2DDataLabelled : public Nymph::KTExtensibleData< KTDiscriminatedPoints2DDataLabelled >
    {
        public:
            struct Point
            {
                double fAbscissa;
                double fOrdinate;
                double fApplicate;
                double fThreshold;
                int label;
                Point(double abscissa, double ordinate, double applicate, double threshold) : fAbscissa(abscissa), fOrdinate(ordinate), fApplicate(applicate), fThreshold(threshold) {}
            };
            typedef std::map< std::pair< unsigned, unsigned >, Point > SetOfPoints;

        protected:
            struct PerComponentData
            {
                SetOfPoints fPoints;
            };

        public:
            KTDiscriminatedPoints2DDataLabelled();
            virtual ~KTDiscriminatedPoints2DDataLabelled();

            const SetOfPoints& GetSetOfPoints(unsigned component = 0) const;
            double GetThreshold(unsigned component = 0) const;

            unsigned GetNComponents() const;

            void AddPoint(unsigned binX, unsigned binY, const Point& point, unsigned component = 0);
        
            KTDiscriminatedPoints2DDataLabelled& SetNComponents(unsigned channels);

            unsigned GetNBinsX() const;
            unsigned GetNBinsY() const;
            double GetBinWidthX() const;
            double GetBinWidthY() const;

            void SetNBinsX(unsigned nBins);
            void SetNBinsY(unsigned nBins);
            void SetBinWidthX(double binWidth);
            void SetBinWidthY(double binWidth);

        private:
            std::vector< PerComponentData > fComponentData;

            unsigned fNBinsX;
            unsigned fNBinsY;
            double fBinWidthX;
            double fBinWidthY;

        public:
            static const std::string sName;
    };

    inline const KTDiscriminatedPoints2DDataLabelled::SetOfPoints& KTDiscriminatedPoints2DDataLabelled::GetSetOfPoints(unsigned component) const
    {
        return fComponentData[component].fPoints;
    }
/*
    inline double KTDiscriminatedPoints2DDataLabelled::GetThreshold(unsigned component) const
    {
        return fComponentData[component].fThreshold;
    }
*/
    inline unsigned KTDiscriminatedPoints2DDataLabelled::GetNComponents() const
    {
        return unsigned(fComponentData.size());
    }

    inline void KTDiscriminatedPoints2DDataLabelled::AddPoint(unsigned binX, unsigned binY, const Point& point, unsigned component)
    {
        if (component >= fComponentData.size()) fComponentData.resize(component+1);
        fComponentData[component].fPoints.insert(std::make_pair(std::make_pair(binX, binY), point));
    }

    inline KTDiscriminatedPoints2DDataLabelled& KTDiscriminatedPoints2DDataLabelled::SetNComponents(unsigned channels)
    {
        fComponentData.resize(channels);
        return *this;
    }

    inline unsigned KTDiscriminatedPoints2DDataLabelled::GetNBinsX() const
    {
        return fNBinsX;
    }

    inline unsigned KTDiscriminatedPoints2DDataLabelled::GetNBinsY() const
    {
        return fNBinsY;
    }

    inline double KTDiscriminatedPoints2DDataLabelled::GetBinWidthX() const
    {
        return fBinWidthX;
    }

    inline double KTDiscriminatedPoints2DDataLabelled::GetBinWidthY() const
    {
        return fBinWidthY;
    }

    inline void KTDiscriminatedPoints2DDataLabelled::SetNBinsX(unsigned nBins)
    {
        fNBinsX = nBins;
        return;
    }

    inline void KTDiscriminatedPoints2DDataLabelled::SetNBinsY(unsigned nBins)
    {
        fNBinsY = nBins;
        return;
    }

    inline void KTDiscriminatedPoints2DDataLabelled::SetBinWidthX(double binWidth)
    {
        fBinWidthX = binWidth;
        return;
    }

    inline void KTDiscriminatedPoints2DDataLabelled::SetBinWidthY(double binWidth)
    {
        fBinWidthY = binWidth;
        return;
    }

} /* namespace Katydid */

#endif /* KTDISCRIMINATEDPOINTS2DDATA_HH_ */
