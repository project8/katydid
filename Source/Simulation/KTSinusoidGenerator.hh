/*
 * KTSinusoidGenerator.hh
 *
 *  Created on: May 3, 2013
 *      Author: nsoblath
 */

#ifndef KTSINUSOIDGENERATOR_HH_
#define KTSINUSOIDGENERATOR_HH_

#include "KTTSGenerator.hh"

namespace Katydid
{
    using namespace Nymph;

    /*!
     @class KTSinusoidGenerator
     @author N. S. Oblath

     @brief Generates a sinusoidal time series

     @details
     Can create a new time series and drive processing, or can add a sinusoidal signal to an existing time series.

     Basic time series formation is dealt with in KTTSGenerator.

     Available configuration options:
     - Inherited from KTTSGenerator
       - "number-of-slices": unsigned -- Number of slices to create (used only if creating new slices)
       - "n-channels": unsigned -- Number of channels per slice to create (used only if creating new slices)
       - "slice-size": unsigned -- Specify the size of the time series (used only if creating new slices)
       - "bin-width": double -- Specify the bin width
       - "time-series-type": string -- Type of time series to produce (options: real [default], fftw)
       - "record-size": unsigned -- Size of the imaginary record that this slice came from (only used to fill in the egg header; does not affect the simulation at all)
     - From KTSinusoidGenerator
       - "frequency": double -- Frequency of the sinusoid
       - "phase": double -- Phase of the sinusoid
       - "amplitude": double -- Amplitude of the sinusoid

     Slots: (inherited from KTTSGenerator)
     - "slice": void (KTDataPtr) -- Add a signal to an existing time series; Requires KTTimeSeriesData; Emits signal "slice" when done.

     Signals: (inherited from KTTSGenerator)
     - "header": void (KTEggHeader*) -- emitted when the egg header is created.
     - "slice": void (KTDataPtr) -- emitted when the new time series is produced or processed.
     - "done": void () --  emitted when the job is complete.
    */
    class KTSinusoidGenerator : public KTTSGenerator
    {
        public:
            KTSinusoidGenerator(const std::string& name = "sinusoid-generator");
            virtual ~KTSinusoidGenerator();

            virtual bool ConfigureDerivedGenerator(const scarab::param_node* node);

            double GetFrequency() const;
            void SetFrequency(double freq);

            double GetPhase() const;
            void SetPhase(double phase);

            double GetAmplitude() const;
            void SetAmplitude(double amp);

        private:
            double fFrequency;
            double fPhase;
            double fAmplitude;

        public:
            virtual bool GenerateTS(KTTimeSeriesData& data);

    };

    inline double KTSinusoidGenerator::GetFrequency() const
    {
        return fFrequency;
    }

    inline void KTSinusoidGenerator::SetFrequency(double freq)
    {
        fFrequency = freq;
        return;
    }

    inline double KTSinusoidGenerator::GetPhase() const
    {
        return fPhase;
    }

    inline void KTSinusoidGenerator::SetPhase(double phase)
    {
        fPhase = phase;
        return;
    }

    inline double KTSinusoidGenerator::GetAmplitude() const
    {
        return fAmplitude;
    }

    inline void KTSinusoidGenerator::SetAmplitude(double amp)
    {
        fAmplitude = amp;
        return;
    }


} /* namespace Katydid */
#endif /* KTSINUSOIDGENERATOR_HH_ */
