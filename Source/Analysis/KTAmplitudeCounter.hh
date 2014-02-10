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
    class KTPStoreNode;

    /*!
     @class KTAmplitudeCounter
     @author laroque

     @brief creates time series amplitude distribution

     @details
     Takes times series distribution name 

     Configuration name: "amp-counter"

     Available configuration values:
     - "num-bins": unsigned -- number of evenly spaced bins

     Slots:
     - "raw-ts": void (KTDataPtr) -- Converts a raw time series to a value distribution; Requires KTRawTimeSeriesData; Adds KTTimeSeriesDist; Emits signal "ts-dist"

     Signals:
     - "ts-dist": void (KTDataPtr) -- Emitted upon calculation of a time-series distribution; Guarantees KTTimeSeriesDistData
     */

    class KTAmplitudeCounter : public KTProcessor
    {
        public:
            KTAmplitudeCounter(const std::string& name = "amp-counter");
            virtual ~KTAmplitudeCounter();

            bool Configure(const KTPStoreNode* node);

            unsigned GetNumberOfBins() const;
            void SetNumberOfBins(unsigned nbins);

        private:
            unsigned fNumberOfBins;

        public:
            bool AddData(KTRawTimeSeriesData& data);

            bool CountTimeSeries(KTTimeSeriesDist* tsdist, const KTRawTimeSeries* ts);


            //***************
            // Signals
            //***************

        private:
            KTSignalData fTSDistSignal;

            //***************
            // Slots
            //***************

        private:
            KTSlotDataOneType< KTRawTimeSeriesData > fTSSlot;

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

} /* namespace Katydid */
#endif //KTAMPLITUDECOUNTER_HH_
