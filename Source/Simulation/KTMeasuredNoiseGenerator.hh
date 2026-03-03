/*
 * KTMeasuredNoiseGenerator.hh
 *
 *  Created on: Sep 17, 2025
 *      Author: ehtkarim
 */

#ifndef KTMEASUREDNOISEGENERATOR_HH_
#define KTMEASUREDNOISEGENERATOR_HH_

#include "KTGaussianNoiseGenerator.hh"

#include <string>
#include <vector>

namespace scarab {
    class param_node;
    class param_array;
}

namespace Katydid
{
    /*!
     @class KTMeasuredNoiseGenerator
     @author E. Karim (Adil)

     @brief Generates additive noise using user-provided (measured) PSD mean and variance

     @details
     Can create a new time series and drive processing, or can add a user-provided (measured) noise spectrum to an existing time series.

     Available configuration options:
     - Inherited from KTTSGenerator
       - "number-of-slices": unsigned -- Number of slices to create (used only if creating new slices)
       - "n-channels": unsigned -- Number of channels per slice to create (used only if creating new slices)
       - "slice-size": unsigned -- Specify the size of the time series (used only if creating new slices)
       - "bin-width": double -- Specify the bin width
       - "time-series-type": string -- Type of time series to produce (options: real [default], fftw)
       - "record-size": unsigned -- Size of the imaginary record that this slice came from (only used to fill in the egg header; does not affect the simulation at all)
     - Inherited from KTCavityNoiseGenerator
       - "noise-scaling": double -- Scaling factor of noise amplitude, must be positive-definite. Defaults to unity
     - From KTMeasuredNoiseGenerator
       - "frequency-units": string -- Unit of the input psd-array frequency. Can be either "Hz", "kHz", "MHz" or "GHz". If the psd-array input file has units defined, this is overriden
       - "fixed-radius": bool -- Mode for which power in each bin is set to exactly match the per-bin power in the input file. If false, complex Gaussian drawing causes additional variance
       - "use-variance": bool -- Mode for which the variance in each bin is set to exactly match the per-bin variance in the input file; phase is random and uniform. If false, the resulting spectrum is stochastic with unknown variance per slice (default)
       - "measured-noise": node -- Node for defining the file directory of the input psd-arrays
          - "psd-file": path -- Directs to the path where the .yaml file is saved with both psd-mean and psd-variance arrays

     Slots: (inherited from KTTSGenerator)
     - "slice": void (Nymph::KTDataPtr) -- Add a signal to an existing time series; Requires KTTimeSeriesData; Emits signal "slice" when done.

     Signals: (inherited from KTTSGenerator)
     - "header": void (KTEggHeader*) -- emitted when the egg header is created.
     - "slice": void (Nymph::KTDataPtr) -- emitted when the new time series is produced or processed.
     - "done": void () --  emitted when the job is complete.
    */
    class KTMeasuredNoiseGenerator : public KTGaussianNoiseGenerator
    {
        public:
            KTMeasuredNoiseGenerator(const std::string& name = "measured-noise-generator");
            virtual ~KTMeasuredNoiseGenerator();

            virtual bool ConfigureDerivedGenerator(const scarab::param_node* node);

        public:
            virtual bool GenerateTS(KTTimeSeriesData& data);

        protected:
            // Simple natural cubic spline for doubles
            class KCubicSpline
            {
                public:
                    KCubicSpline();
                    ~KCubicSpline();

                    void Clear();
                    bool Build(const std::vector< double >& x, const std::vector< double >& y);
                    double Evaluate(double xval) const;
                    bool IsBuilt() const;

                private:
                    std::vector< double > fX;
                    std::vector< double > fA;
                    std::vector< double > fB;
                    std::vector< double > fC;
                    std::vector< double > fD;
            };

            // Helpers for reading arrays and building splines
            bool LoadPointsFromArray(const scarab::param_array& arr, std::vector< double >& xs, std::vector< double >& ys, double freqScale) const;

            bool BuildSplines();

            // Drawing a PSD (W/Hz) sample at |f| using mean and variance splines
            double DrawPSD(double f_abs_hz);

            // Random unit complex (cos, sin) using 2D Gaussians; avoids a new RNG
            void RandomUnitComplex(double& c, double& s);

        protected:
            // Config
            std::string fTransformFlag;   // FFTW estimate/measure flag
            double      fNoiseScaling;    // global scaling > 0
            bool        fUseVariance;     // if true, draw bin powers using provided variance
            bool        fFixedRadius;     // if true, use fixed-radius, random-phase (default true)
            double      fFreqScale;       // units scale for input frequencies (Hz multiplier)
            double      fFreqMinHz;       // required input min frequency (Hz) for validation
            double      fFreqMaxHz;       // required input max frequency (Hz) for validation

            // Spline data
            std::vector< double > fMeanXHz;
            std::vector< double > fMeanY;     // W/Hz
            std::vector< double > fVarY;      // (W/Hz)^2

            KCubicSpline           fMeanSpline;
            KCubicSpline           fVarSpline;

            bool                   fHaveMean;
            bool                   fHaveVar;
    };

} /* namespace Katydid */

#endif /* KTMEASUREDNOISEGENERATOR_HH_ */

