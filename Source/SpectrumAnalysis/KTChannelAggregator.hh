/*
 * KTChannelAggregator.hh
 *
 *  Created on: Jan 25, 2019
 *      Author: P. T. Surukuchi
 */

#ifndef KTCHANNELAGGREGATOR_HH_
#define KTCHANNELAGGREGATOR_HH_

#include "KTProcessor.hh"
#include "KTData.hh"
#include "KTPowerSpectrumData.hh"
#include "KTChannelAggregatedData.hh"
#include "KTAxialChannelAggregatedData.hh"
#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTAxisProperties.hh"
#include "KTFSCDCRESUtils.hh"

#include "KTMemberVariable.hh"

#include "KTSlot.hh"

#include "KTMath.hh"

namespace Katydid
{

    class KTChannelAggregatedData;
    class KTAxialChannelAggregatedData;
    class KTPowerSpectrumData;

    /*
     @class KTChannelAggregator
     @author P. T. Surukuchi
     
     @brief Multiple channel summation for Phase-III and IV
     
     @details
     More details to come.
     
     Configuration name: "channel-aggregator"
     
     Available configuration options:
     - "grid-size": signed int -- Size of the grid; If square grid is considered, the number of points in the grid is the square of grid-size
     - "use-grid-text-file": bool, -- Check whether to define the grid based on an input text file 
     - "grid-text-file": : std::string, -- Text file which has the user-defined grid positions 
     - "min-freq": double -- The minimum frequency value above which the channel aggregated spectrum is calculated. This particularly helps in enforcing bramforming around the central peak and avoiding side bands
     - "max-freq": double -- The maximum frequency value below which the channel aggregated spectrum is calculated
     - "n-rings": signed int -- Number of axial rings
 
     Slots:
     - "fft": void (Nymph::KTDataPtr) -- Adds channels voltages using FFTW-phase information for appropriate phase addition; Requires KTFrequencySpectrumDataFFTW; Adds summation of the channel results; Emits signal "fft"
     
     Signals:
     - "fft": void (Nymph::KTDataPtr) -- Emitted upon summation of all channels; Guarantees KTFrequencySpectrumDataFFTW
     */

    class KTChannelAggregator : public Nymph::KTProcessor
    {
        public:
            KTChannelAggregator(const std::string& name = "channel-aggregator");
            virtual ~KTChannelAggregator();

            bool Configure(const scarab::param_node* node);

            KTFSCDCRESUtils fscdCRESUtils;

            // Get the grid size assuming a square grid
            // Set default to 30 currently
            MEMBERVARIABLE(int, NGrid);

            //For exception handling to make sure the grid is defined before the spectra are assigned.
            MEMBERVARIABLE(bool, IsGridDefined);
            
            // A boolean value to check whether the grid should be defined by the user
            MEMBERVARIABLE(bool, IsUserDefinedGrid);

            // A boolean to run channel aggreagator when only a partial ring is defined
            MEMBERVARIABLE(bool, IsPartialRing);

            // If IsPartialRing is true, the total number of channels is the product of the number of channels extracted from the egg file and the PartialRingMultiplicity
            MEMBERVARIABLE(int, PartialRingMultiplicity)

            // The text file to be used for the user-defined grid
            MEMBERVARIABLE(std::string, UserDefinedGridFile);

    	    //The minimum frequency value above which the channel aggregated spectrum is calculated
            MEMBERVARIABLE(double, SummationMinFreq);

	        //The maximum frequency value below which the channel aggregated spectrum is calculated
            MEMBERVARIABLE(double, SummationMaxFreq);

            // Number of axial rings/subarrays
            MEMBERVARIABLE(int, NRings);

            virtual bool SumChannelVoltageWithPhase(KTFrequencySpectrumDataFFTW& fftwData);
            virtual bool SumChannelVoltageWithPhase(KTAxialAggregatedFrequencySpectrumDataFFTW& fftwData);

        protected:

	        ///map that stores antispiral phase shifts
	        std::map<int,double> fAntiSpiralPhaseShifts; 

            /// Define the grid based (one per ring)
            /*If fIsUserDefinedGrid it true, the grid is defined based on text file defined by fUserDefinedGridFile. 
             * If fIsUserDefinedGrid is false, defines a square grid with fNGrid*fNGrid points
             * Returns total number of grid points defined
             */
            int DefineGrid(KTAggregatedFrequencySpectrumDataFFTW &newAggFreqData);

	        /// Generate antispiral phase shifts and save in fAntiSpiralPhaseShifts vector to be applied to channels 
	        //bool GenerateAntiSpiralPhaseShifts(int channelCount);

            virtual bool PerformPhaseSummation(KTFrequencySpectrumDataFFTWCore& fftwData,KTAggregatedFrequencySpectrumDataFFTW& newAggFreqData);
            //***************
            // Signals
            //***************

        protected:
            Nymph::KTSignalData fSummedFrequencyData;

            //***************
            // Slots
            //***************

        protected:
            Nymph::KTSlotDataOneType< KTFrequencySpectrumDataFFTW > fPhaseChFrequencySumSlot;
            Nymph::KTSlotDataOneType< KTAxialAggregatedFrequencySpectrumDataFFTW> fAxialSumSlot;
    };

}

#endif  /* KTCHANNELAGGREGATOR_HH_  */
