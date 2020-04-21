/*
 * KTSubarrayChannelAggregator.hh
 *
 *  Created on: Apr 14, 2020
 *      Author: P. T. Surukuchi
 */

#ifndef KTSUBARRAYCHANNELAGGREGATOR_HH_
#define KTSUBARRAYCHANNELAGGREGATOR_HH_

#include "KTProcessor.hh"
#include "KTData.hh"
#include "KTPowerSpectrumData.hh"
#include "KTChannelAggregatedData.hh"
#include "KTChannelAggregator.hh"
#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTAxisProperties.hh"

#include "KTMemberVariable.hh"

#include "KTSlot.hh"

#include "KTMath.hh"

namespace Katydid
{

    class KTChannelAggregatedData;
    class KTPowerSpectrumData;

    /*
       @class KTSubarrayChannelAggregator
       @author P. T. Surukuchi

       @brief Multiple channel summation primarily used when Multiple waveguides are places back-to-back Phase-III and IV

       @details
       More details to come.

       Configuration name: "subarray-channel-aggregator"

       Available configuration options:
       - "n-rings": signed int -- Number of axial rings

*/

    class KTSubarrayChannelAggregator : public KTChannelAggregator 
    {
        public:
            KTSubarrayChannelAggregator(const std::string& name = "subarray-channel-aggregator");
            virtual ~KTSubarrayChannelAggregator();

            bool Configure(const scarab::param_node* node);

            MEMBERVARIABLE(int, NRings);

            bool SumChannelVoltageWithPhase(KTFrequencySpectrumDataFFTW& fftwData) override;
        private:

/*        protected:
            Nymph::KTSignalData fSummedFrequencyData;

            //***************
            // Slots
            //***************

        protected:
            Nymph::KTSlotDataOneType< KTFrequencySpectrumDataFFTW > fPhaseChFrequencySumSlot;
*/

    };

}

#endif  /* KTSUBARRAYCHANNELAGGREGATOR_HH_  */
