/*
 * KTAggregatedChannelOptimizer.hh
 *
 *  Created on: Apr 21, 2019
 *      Author: P. T. Surukuchi
 */

#ifndef KTAGGREGATEDCHANNELOPTIMIZER_HH_
#define KTAGGREGATEDCHANNELOPTIMIZER_HH_

#include "KTProcessor.hh"
#include "KTData.hh"
#include "KTChannelAggregatedData.hh"
#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTAxisProperties.hh"

#include "KTSlot.hh"

#include "KTMath.hh"

namespace Katydid
{

    class KTAggregatedFrequencySpectrumDataFFTW;
    class KTFrequencySpectrumDataFFTW;
    class KTAggregatedTimeSeriesData;

    /*
     @class KTAggregatedChannelOptimizer
     @author P. T. Surukuchi

     @brief Finds the point that optimizes the aggreagted channels

     @details

     Slots:
     - "agg-fft": void (Nymph::KTDataPtr) -- Finds the point that optimizes the aggregated channels; Requires KTAggregatedFrequencySpectrumDataFFTW;Finds the point that optimizes the aggreagted channels ; Emits signal "fft"
     - "agg-ts": void (Nymph::KTDataPtr) -- Finds the point that optimizes the aggregated channels; Requires KTAggregatedTimeSeriesData;Finds the point that optimizes the aggreagted channels ; Emits signal "agg-ts"

     Signals:
     - "agg-fft": void (Nymph::KTDataPtr) -- Emitted upon finding the optimized point ; Guarantees KTFrequencySpectrumDataFFTW
     - "agg-ts": void (Nymph::KTDataPtr) -- Emitted upon finding the optimized point ; Guarantees KTAggregatedTimeSeriesData
     */

    class KTAggregatedChannelOptimizer : public Nymph::KTProcessor
    {
        public:
            KTAggregatedChannelOptimizer(const std::string& name = "aggregated-channel-optimizer");
            virtual ~KTAggregatedChannelOptimizer();

            bool Configure(const scarab::param_node* node);

            bool FindOptimumSum( KTAggregatedFrequencySpectrumDataFFTW& aggData);
            bool FindOptimumSum( KTAggregatedTimeSeriesData& aggData);

        private:

            //***************
            // Signals
            //***************

        private:
            Nymph::KTSignalData fSummedFrequencyData;
            Nymph::KTSignalData fSummedTimeData;

            //***************
            // Slots
            //***************

        private:
            Nymph::KTSlotDataOneType< KTAggregatedFrequencySpectrumDataFFTW > fOptimalFreqSumSlot;
            Nymph::KTSlotDataOneType< KTAggregatedTimeSeriesData > fOptimalTimeSumSlot;
    };
}

#endif  /* KTAGGREGATEDCHANNELOPTIMIZER_HH_  */
