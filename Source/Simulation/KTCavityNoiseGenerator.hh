/*
 * KTCavityNoiseGenerator.hh
 *
 *  Created on: May 28, 2025
 *      Author: ehtkarim
*/

#ifndef KTCAVITYNOISEGENERATOR_HH_
#define KTCAVITYNOISEGENERATOR_HH_

#include "KTGaussianNoiseGenerator.hh"

#include <string>

namespace Katydid
{

    /*!
     @class KTCavityNoiseGenerator
     @author E. Karim (Adil)

     @brief Generates a time series with Gaussian noise using the frequency-dependent cavity noise model

     @details
     Can create a new time series and drive processing, or can add Gaussian noise to an existing time series.

     Available configuration options:
     - Inherited from KTTSGenerator
       - "number-of-slices": unsigned -- Number of slices to create (used only if creating new slices)
       - "n-channels": unsigned -- Number of channels per slice to create (used only if creating new slices)
       - "slice-size": unsigned -- Specify the size of the time series (used only if creating new slices)
       - "bin-width": double -- Specify the bin width
       - "time-series-type": string -- Type of time series to produce (options: real [default], fftw)
       - "record-size": unsigned -- Size of the imaginary record that this slice came from (only used to fill in the egg header; does not affect the simulation at all)
     - Inherited from KTGaussianNoiseGenerator
       - "seed": int -- Seed used to generate random noise. If this is omitted then the noise spectrum is irreproducible
     - From KTCavityNoiseGenerator
       - "noise-scaling": double -- Scaling factor of noise amplitude, must be positive-definite. Defaults to unity
       - "cavity": node -- Node for defining parameters of the cavity noise model
          - "f0": -- Cavity Resonance Frequency (in Hz)
          - "Q_L": -- Loaded Cavity Q-factor
          - "Q0": -- Unloaded Cavity Q-factor
          - "A": -- Transmission line attenuation factor. Can take values from 0 to 1.0
          - "T_line_start": -- Temperature at the cavity end of the transmission line (in K)
          - "T_line_end": -- Temperature at the isolator end of the transmission line (in K)
          - "T_cav": -- Physical temperature of the cavity (in K)
          - "T_isol": -- Physical Temperature of the isolator (in K)
          - "epsilon": -- Efficiency factor for the Quantum-Amplifier. Can take values from 0 to 1.0
          - "f_lo": -- Local Oscillator Frequency in the DAQ chain (in Hz)

     Slots: (inherited from KTTSGenerator)
     - "slice": void (Nymph::KTDataPtr) -- Add a signal to an existing time series; Requires KTTimeSeriesData; Emits signal "slice" when done.

     Signals: (inherited from KTTSGenerator)
     - "header": void (KTEggHeader*) -- emitted when the egg header is created.
     - "slice": void (Nymph::KTDataPtr) -- emitted when the new time series is produced or processed.
     - "done": void () --  emitted when the job is complete.
    */
    class KTCavityNoiseGenerator : public KTGaussianNoiseGenerator
    {
        public:
            KTCavityNoiseGenerator(const std::string& name = "cavity-noise-generator");
            virtual ~KTCavityNoiseGenerator();

            virtual bool ConfigureDerivedGenerator(const scarab::param_node* node);

            double GetQL() const;
            void SetQL(double ql);

            double GetQ0() const;
            void SetQ0(double q0);

        protected:
            double fF0;
            double fA;
            double fQL;
            double fQ0;
            double fTLineStart;
            double fTLineEnd;
            double fTCav;
            double fTIsol;
            double fEpsilon;
            double fFLo;

            std::string fTransformFlag;
            double      fNoiseScaling;

            double NoisePSD(double f) const;
            double Eta(double x) const;

        private:
            double fG;    

        public:
            virtual bool GenerateTS(KTTimeSeriesData& data);
    };

    inline double KTCavityNoiseGenerator::GetQL() const
    {
        return fQL;
    }

    inline void KTCavityNoiseGenerator::SetQL(double ql)
    {
        fQL = ql;
        fG = fQ0 / fQL;
        return;
    }

    inline double KTCavityNoiseGenerator::GetQ0() const
    {
        return fQ0;
    }

    inline void KTCavityNoiseGenerator::SetQ0(double q0)
    {
        fQ0 = q0;
        fG = fQ0 / fQL;
        return;
    }

} /* namespace Katydid */

#endif /* KTCAVITYNOISEGENERATOR_HH_ */
