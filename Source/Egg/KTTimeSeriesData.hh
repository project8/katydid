/**
 @file KTTimeSeriesData.hh
 @brief Contains KTTimeSeriesData
 @details Contains the information from a single Egg bundle in the form of a 1-D std::vector of UInt_tegers.
 The data are the time series of the bundle.
 @note Prior to August 24, 2012, this class was called KTBundle.
 @author: N. S. Oblath
 @date: Sep 9, 2011
 */

#ifndef KTTIMESERIESDATA_HH_
#define KTTIMESERIESDATA_HH_

#include "KTWriteableData.hh"

namespace Katydid
{
    class KTTimeSeries;

    class KTTimeSeriesData : public KTWriteableData
    {
        public:
            KTTimeSeriesData();
            virtual ~KTTimeSeriesData();

            virtual UInt_t GetNTimeSeries() const = 0;

            virtual Double_t GetTimeInRun() const = 0;
            virtual ULong64_t GetSliceNumber() const = 0;

            virtual const KTTimeSeries* GetTimeSeries(UInt_t tsNum = 0) const = 0;
            virtual KTTimeSeries* GetTimeSeries(UInt_t tsNum = 0) = 0;

            virtual void SetNTimeSeries(UInt_t num) = 0;

            virtual void SetTimeInRun(Double_t tir) = 0;
            virtual void SetSliceNumber(ULong64_t slice) = 0;

    };

} /* namespace Katydid */

#endif /* KTTIMESERIESDATA_HH_ */
