/*
 * KTTimeSeriesFFTW.hh
 *
 *  Created on: Sept 4, 2012
 *      Author: nsoblath
 */

#ifndef KTTIMESERIESFFTW_HH_
#define KTTIMESERIESFFTW_HH_

#include "KTPhysicalArrayFFTW.hh"
#include "KTTimeSeries.hh"

namespace Katydid
{


    class KTTimeSeriesFFTW : public KTTimeSeries, public KTPhysicalArray< 1, fftw_complex >
    {
        public:
            KTTimeSeriesFFTW();
            KTTimeSeriesFFTW(size_t nBins, Double_t rangeMin=0., Double_t rangeMax=1.);
            KTTimeSeriesFFTW(const KTTimeSeriesFFTW& orig);
            virtual ~KTTimeSeriesFFTW();

            KTTimeSeriesFFTW& operator=(const KTTimeSeriesFFTW& rhs);

            virtual UInt_t GetNTimeBins() const;
            virtual Double_t GetTimeBinWidth() const;

            virtual void SetValue(UInt_t bin, Double_t value);
            virtual Double_t GetValue(UInt_t bin) const;

            virtual void Print(UInt_t startPrint, UInt_t nToPrint) const;

#ifdef ROOT_FOUND
        public:
            virtual TH1D* CreateHistogram(const std::string& name = "hTimeSeries") const;

            virtual TH1D* CreateAmplitudeDistributionHistogram(const std::string& name = "hTimeSeriesDist") const;
#endif
    };

    inline UInt_t KTTimeSeriesFFTW::GetNTimeBins() const
    {
        return this->size();
    }

    inline Double_t KTTimeSeriesFFTW::GetTimeBinWidth() const
    {
        return this->GetBinWidth();
    }

    inline void KTTimeSeriesFFTW::SetValue(UInt_t bin, Double_t value)
    {
        (*this)(bin)[0] = value;
        (*this)(bin)[1] = 0.;
        return;
    }

    inline Double_t KTTimeSeriesFFTW::GetValue(UInt_t bin) const
    {
        return (*this)(bin)[0];
    }

} /* namespace Katydid */
#endif /* KTTIMESERIESFFTW_HH_ */
