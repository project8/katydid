/**
 @file KTAmplitudeCounter.hh
 @brief Contains KTAmplitudeCounter
 @details Takes time series data and creates distribution of values
 @author: laroque
 @date: Jan 24, 2014
 */

#ifndef KTAMPLITUDECOUNTER_HH_
#define KTAMPLITUDECOUNTER_HH_

#include "KTProcessor.hh"

#include "KTRawTimeSeriesData.hh"
#include "KTTimeSeriesDistData.hh"

#include "KTSlot.hh"


namespace Katydid
{
    
    class KTEggHeader;

    /*!
     @class KTAmplitudeCounter
     @author laroque

     @brief creates time series amplitude distribution

     @details
     Takes times series distribution name 

     Configuration name: "amp-counter"

     Available configuration values:
     - "num-bins": unsigned -- number of evenly spaced bins
     - "range-min": double -- minimum value for the x-axis of the distribution
     - "range-max": double -- maximum value for the x-axis of the distribution

     Slots:
     - "raw-ts": void (Nymph::KTDataPtr) -- Converts a raw time series to a value distribution; Requires KTRawTimeSeriesData; Adds KTTimeSeriesDist; Emits signal "ts-dist"

     Signals:
     - "ts-dist": void (Nymph::KTDataPtr) -- Emitted upon calculation of a time-series distribution; Guarantees KTTimeSeriesDistData
     */

    class KTAmplitudeCounter : public Nymph::KTProcessor
    {
        public:
            KTAmplitudeCounter(const std::string& name = "amp-counter");
            virtual ~KTAmplitudeCounter();

            bool Configure(const scarab::param_node* node);

            unsigned GetNumberOfBins() const;
            void SetNumberOfBins(unsigned nbins);

            double GetRangeMin() const;
            void SetRangeMin(double min);

            double GetRangeMax() const;
            void SetRangeMax(double max);

        private:
            unsigned fNumberOfBins;
            double fRangeMin;
            double fRangeMax;

        public:
            bool AddData(KTRawTimeSeriesData& data);

            bool CountTimeSeries(KTTimeSeriesDist* tsdist, const KTRawTimeSeries* ts);


            //***************
            // Signals
            //***************

        private:
            Nymph::KTSignalData fTSDistSignal;

            //***************
            // Slots
            //***************

        private:
            Nymph::KTSlotDataOneType< KTRawTimeSeriesData > fTSSlot;

    };

    inline unsigned KTAmplitudeCounter::GetNumberOfBins() const
    {
        return fNumberOfBins;
    }

    inline void KTAmplitudeCounter::SetNumberOfBins(unsigned nbins)
    {
        fNumberOfBins = nbins;
        return;
    }

    inline double KTAmplitudeCounter::GetRangeMin() const
    {
        return fRangeMin;
    }

    inline void KTAmplitudeCounter::SetRangeMin(double min)
    {
        fRangeMin = min;
        return;
    }

    inline double KTAmplitudeCounter::GetRangeMax() const
    {
        return fRangeMax;
    }

    inline void KTAmplitudeCounter::SetRangeMax(double max)
    {
        fRangeMax = max;
        return;
    }


} /* namespace Katydid */
#endif //KTAMPLITUDECOUNTER_HH_
