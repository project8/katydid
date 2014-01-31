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

#include "KTTimeSeriesData.hh"

#include "KTSlot.hh"



namespace Katydid
{
    // Forward declare whatever classes you can
    // input data type . . .
    // output data type . . .
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
     - "min_amp": double -- amplitude value of the lower edge of the lowest bin
     - "max_amp": double -- amplitude value of the upper edge of the highest bin
     - "num-bins": unsigned -- number of evenly spaced bins

     Slots:
     - "ts": void (KTDataPtr) -- Converts a time series to a value distribution; Requires KTTimeSeriesData; Adds KTTimeSeriesDistribution; Emits signal "ts-dist"

     Signals:
     - "ts-dist": void (shared_ptr<KTData>) -- Emitted upon [whatever was done]; Guarantees [output data type].
    */

    class KTAmplitudeCounter : public KTProcessor
    {
        public:
            KTAmplitudeCounter(const std::string& name = "amp-counter");
            virtual ~KTAmplitudeCounter();

            bool Configure(const KTPStoreNode* node);

            // Getters and setters for configurable parameters go here
            unsigned GetNumberOfBins() const;
            void SetNumberOfBins(unsigned nbins);
            double GetMinimumAmplitude() const;
            void SetMinimumAmplitude(double minamp);
            double GetMaximumAmplitude() const;
            void SetMaximumAmplitude(double maxamp);
        private:
            // configurable member parameters go here
            unsigned fNumberOfBins;
            double fMinimumAmplitude;
            double fMaximumAmplitude;

        public:
            // Functions to do the job of the processor go here
            // These allow the processor to be used on data objects manually
            bool AddData(KTTimeSeriesData& data);


        private:
            // Perhaps there are some non-public helper functions?

            // And then any non-configurable member variables

        //***************
        // Signals
        //***************

        private:
            KTSignalData fTSDistSignal;

        //***************
        // Slots
        //***************

        private:
            //KTSlotOneArg< void (const KTEggHeader*) > fHeaderSlot;
            //KTSlotOneArg< void (const KTTimeSeriesData*) > fTSSlot;
            KTSlotDataOneType< KTTimeSeriesData > fTSSlot;

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

    inline double KTAmplitudeCounter::GetMinimumAmplitude() const
    {
        return fMinimumAmplitude;
    }

    inline void KTAmplitudeCounter::SetMinimumAmplitude(double minamp)
    {
        fMinimumAmplitude = minamp;
        return;
    }

    inline double KTAmplitudeCounter::GetMaximumAmplitude() const
    {
        return fMaximumAmplitude;
    }

    inline void KTAmplitudeCounter::SetMaximumAmplitude(double maxamp)
    {
        fMaximumAmplitude = maxamp;
        return;
    }

} /* namespace Katydid */
#endif //KTAMPLITUDECOUNTER_HH_
