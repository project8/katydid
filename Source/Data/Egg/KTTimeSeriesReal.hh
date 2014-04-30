/*
 * KTTimeSeriesReal.hh
 *
 *  Created on: Sept 4, 2012
 *      Author: nsoblath
 */

#ifndef KTTIMESERIESREAL_HH_
#define KTTIMESERIESREAL_HH_

#include "KTPhysicalArray.hh"
#include "KTTimeSeries.hh"

namespace Katydid
{
    class KTTimeSeriesReal : public KTTimeSeries, public KTPhysicalArray< 1, double >
    {
        public:
            KTTimeSeriesReal();
            KTTimeSeriesReal(size_t nBins, double rangeMin=0., double rangeMax=1.);
            KTTimeSeriesReal(const KTTimeSeriesReal& orig);
            virtual ~KTTimeSeriesReal();

            KTTimeSeriesReal& operator=(const KTTimeSeriesReal& rhs);

            virtual void Scale(double scale);

            virtual unsigned GetNTimeBins() const;
            virtual double GetTimeBinWidth() const;

            virtual void SetValue(unsigned bin, double value);
            virtual double GetValue(unsigned bin) const;

            virtual void Print(unsigned startPrint, unsigned nToPrint) const;

#ifdef ROOT_FOUND
        public:
            virtual TH1D* CreateHistogram(const std::string& name = "hTimeSeries") const;

            virtual TH1D* CreateAmplitudeDistributionHistogram(const std::string& name = "hTimeSeriesDist") const;
#endif
    };

    inline void KTTimeSeriesReal::Scale(double scale)
    {
        this->KTPhysicalArray< 1, double >::operator*=(scale);
        return;
    }

    inline unsigned KTTimeSeriesReal::GetNTimeBins() const
    {
        return this->size();
    }

    inline double KTTimeSeriesReal::GetTimeBinWidth() const
    {
        return this->GetBinWidth();
    }

    inline void KTTimeSeriesReal::SetValue(unsigned bin, double value)
    {
        (*this)(bin) = value;
        return;
    }

    inline double KTTimeSeriesReal::GetValue(unsigned bin) const
    {
        return (*this)(bin);
    }

} /* namespace Katydid */
#endif /* KTTIMESERIESREAL_HH_ */
