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
#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTTimeSeriesData.hh"
#include "KTTimeSeriesFFTW.hh"
#include "KTAxisProperties.hh"

#include "KTMemberVariable.hh"

#include "KTSlot.hh"

#include "KTMath.hh"

namespace Katydid
{

    class KTChannelAggregatedData;
    class KTPowerSpectrumData;

    /*
     @class KTChannelAggregator
     @author P. T. Surukuchi

     @brief Multiple channel summation for Phase-III and IV

     @details
     More details to come.

     Configuration name: "channel-aggregator"

     Available configuration options:
     - "active-radius": double -- The active radius of the detection volume
     - "grid-size": signed int -- Size of the grid; If square grid is considered, the number of points in the grid is the square of grid-size
     - "wavelength": double -- Wavelength of the cyclotron motion
     - "use-antispiral-phase-shifts": bool, -- A flag to indicate whether to use antispiral phase shifts

     Slots:
     - "fft": void (Nymph::KTDataPtr) -- Adds channels voltages using FFTW-phase information for appropriate phase addition; Requires KTFrequencySpectrumDataFFTW; Adds summation of the channel results; Emits signal "fft"
     - "ts-fftw": void (Nymph::KTDataPtr) -- Adds channels voltages using FFTW-phase information for appropriate phase addition; Requires KTTimeSeriesData; Adds summation of the channel results; Emits signal "agg-ts"
     Signals:
     - "agg-fft": void (Nymph::KTDataPtr) -- Emitted upon summation of all channels; Guarantees KTFrequencySpectrumDataFFTW
     - "agg-ts": void (Nymph::KTDataPtr) -- Emitted upon summation of all channels; Guarantees KTAggregatedTimeSeresData
     */

    class KTChannelAggregator : public Nymph::KTProcessor
    {
        public:
            KTChannelAggregator(const std::string& name = "channel-aggregator");
            virtual ~KTChannelAggregator();

            bool Configure(const scarab::param_node* node);

            // in meters, should not be hard-coded
            MEMBERVARIABLE(double, ActiveRadius);

            // Get the grid size assuming a square grid
            // Set default to 30 currently
            MEMBERVARIABLE(int, NGrid);

            // PTS:: for 18.6 keV electrons, this somehow needs to come from the data file or config file
            MEMBERVARIABLE(double, Wavelength);

            //For exception handling to make sure the grid is defined before the spectra are assigned.
            MEMBERVARIABLE(bool, IsGridDefined);

      	    //AN electron undergoiing cyclotron motion has a spiral motion and not all receving channels are in phase.
      	    //If selected this option will make sure that there is a relative phase-shift applied
      	    MEMBERVARIABLE(bool,UseAntiSpiralPhaseShifts);
            //Flag to indicate the summation domain. Defaults to false or frequency domain.
            //MEMBERVARIABLE(bool,TimeDomain);

            bool SumChannelVoltageWithPhase(KTFrequencySpectrumDataFFTW& fftwData);
            //Overloaded function for KTTimeSeresData
            bool SumChannelVoltageWithPhase(KTTimeSeriesData& timeData);

        private:

      	    ///map that stores antispiral phase shifts
      	    std::map<int,double> fAntiSpiralPhaseShifts;

            /// Returns the phase shift based on a given point, angle of the channel and the wavelength
            double GetPhaseShift(double xPosition, double yPosition, double wavelength, double channelAngle) const;

            /// Get location of the point in the grid based on the given grid number and the size of the grid.
            /* Returns true if the assigment went well, false if there was some mistake
             */
            bool GetGridLocation(int gridNumber, int gridSize, double &gridLocation);

            /// Apply shift phase to the supplied points based on the phase provided
            bool ApplyPhaseShift(double &realVal, double &imagVal, double phase);

      	    /// Generate antispiral phase shifts and save in fAntiSpiralPhaseShifts vector to be applied to channels
      	    bool GenerateAntiSpiralPhaseShifts(int channelCount);

            /// Convert frquency to wavlength
            double ConvertFrequencyToWavelength(double frequency);

        private:
            //PTS: This needs fixing, currently just setting each element to 0. But why does it have to be done to begin with.
            // Perhaps there is some function in the utilities to do this ?
            bool NullFreqSpectrum(KTFrequencySpectrumFFTW &freqSpectrum);

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
            Nymph::KTSlotDataOneType< KTFrequencySpectrumDataFFTW > fPhaseChFrequencySumSlot;
            Nymph::KTSlotDataOneType< KTTimeSeriesData > fPhaseChTimeSumSlot;
    };

    inline bool KTChannelAggregator::NullFreqSpectrum(KTFrequencySpectrumFFTW &freqSpectrum)
    {
        for (unsigned i = 0; i < freqSpectrum.size(); ++i)
        {
            freqSpectrum.SetRect(i, 0.0, 0.0);
        }
        return true;
    }
}

#endif  /* KTCHANNELAGGREGATOR_HH_  */
