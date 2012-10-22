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
    class KTTimeSeriesReal : public KTTimeSeries, public KTPhysicalArray< 1, Double_t >
    {
        public:
            KTTimeSeriesReal();
            KTTimeSeriesReal(size_t nBins, Double_t rangeMin=0., Double_t rangeMax=1.);
            KTTimeSeriesReal(const KTTimeSeriesReal& orig);
            virtual ~KTTimeSeriesReal();

            KTTimeSeriesReal& operator=(const KTTimeSeriesReal& rhs);

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

    inline UInt_t KTTimeSeriesReal::GetNTimeBins() const
    {
        return this->size();
    }

    inline Double_t KTTimeSeriesReal::GetTimeBinWidth() const
    {
        return this->GetBinWidth();
    }

    inline void KTTimeSeriesReal::SetValue(UInt_t bin, Double_t value)
    {
        (*this)(bin) = value;
        return;
    }

    inline Double_t KTTimeSeriesReal::GetValue(UInt_t bin) const
    {
        return (*this)(bin);
    }

} /* namespace Katydid */
#endif /* KTTIMESERIESREAL_HH_ */
