/*
 * KTTimeSeriesFFTW.hh
 *
 *  Created on: Sept 4, 2012
 *      Author: nsoblath
 */

#ifndef KTTIMESERIESFFTW_HH_
#define KTTIMESERIESFFTW_HH_

#include "KTPhysicalArrayComplex.hh"
#include "KTTimeSeries.hh"

#include <initializer_list>

namespace Katydid
{
    


    class KTTimeSeriesFFTW : public KTTimeSeries, public KTPhysicalArray< 1, std::complex<double> >
    {
        public:
            KTTimeSeriesFFTW();
            KTTimeSeriesFFTW(size_t nBins, double rangeMin=0., double rangeMax=1.);
            KTTimeSeriesFFTW(std::initializer_list<double> value, size_t nBins, double rangeMin=0., double rangeMax=1.);
            KTTimeSeriesFFTW(const KTTimeSeriesFFTW& orig);
            virtual ~KTTimeSeriesFFTW();

            KTTimeSeriesFFTW& operator=(const KTTimeSeriesFFTW& rhs);

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

    inline void KTTimeSeriesFFTW::Scale(double scale)
    {
        this->KTPhysicalArray< 1, std::complex<double> >::operator*=(scale);
        return;
    }

    inline unsigned KTTimeSeriesFFTW::GetNTimeBins() const
    {
        return this->size();
    }

    inline double KTTimeSeriesFFTW::GetTimeBinWidth() const
    {
        return this->GetBinWidth();
    }

    inline void KTTimeSeriesFFTW::SetValue(unsigned bin, double value)
    {
        (*this)(bin) = std::complex<double> {value, 0.};
        return;
    }

    inline double KTTimeSeriesFFTW::GetValue(unsigned bin) const
    {
        return (*this)(bin).real();
    }

} /* namespace Katydid */
#endif /* KTTIMESERIESFFTW_HH_ */
