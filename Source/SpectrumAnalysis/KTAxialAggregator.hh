/*
 * KTAxialAggregator.hh
 *
 *  Created on: April 24, 2020
 *      Author: P. T. Surukuchi
 */

#ifndef KTAXIALAGGREGATOR_HH_
#define KTAXIALAGGREGATOR_HH_

#include "KTProcessor.hh"
#include "KTData.hh"
#include "KTPowerSpectrumData.hh"
#include "KTAxialChannelAggregatedData.hh"
#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTAxisProperties.hh"

#include "KTMemberVariable.hh"

#include "KTSlot.hh"

#include "KTMath.hh"

namespace Katydid
{

    /*
     @class KTAxialAggregator
     @author P. T. Surukuchi
     
     @brief Perform summation of channel along the axial direction 
     
     @details
     
     Configuration name: "axial-channel-aggregator"
     
     Available configuration options:
     Slots:
     - "axial-agg-fft": void (Nymph::KTDataPtr) -- Adds channels voltages along the axial direction; Requires KTFrequencySpectrumDataFFTW; Adds summation of the channel results; Emits signal "ax-agg-fft"
     
     Signals:
     - "fft": void (Nymph::KTDataPtr) -- Emitted upon summation of all channels; Guarantees KTAxialAggregatedFrequencySpectrumDataFFTW 
     */

    class KTAxialAggregator : public Nymph::KTProcessor
    {
        public:
            KTAxialAggregator(const std::string& name = "axial-channel-aggregator");
            virtual ~KTAxialAggregator();

            bool Configure(const scarab::param_node* node);

            // Number of axial rings/subarrays
            MEMBERVARIABLE(unsigned, NRings);
            
            virtual bool SumChannelVoltage(KTFrequencySpectrumDataFFTW& fftwData);
      
            //***************
            // Signals
            //***************

        protected:
            Nymph::KTSignalData fAxialSummedFrequencyData;

            //***************
            // Slots
            //***************

        protected:
            Nymph::KTSlotDataOneType< KTFrequencySpectrumDataFFTW > fAxialFrequencySumSlot;
    };

}

#endif  /* KTAXIALAGGREGATOR_HH_*/
