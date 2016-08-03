/*
 * KTGaussianNoiseGenerator.hh
 *
 *  Created on: May 3, 2013
 *      Author: nsoblath
 */

#ifndef KTGAUSSIANNOISEGENERATOR_HH_
#define KTGAUSSIANNOISEGENERATOR_HH_

#include "KTTSGenerator.hh"

#include "KTRandom.hh"

namespace Katydid
{
    using namespace Nymph;

    /*!
     @class KTGaussianNoiseGenerator
     @author N. S. Oblath

     @brief Generates a time series with Gaussian noise

     @details
     Can create a new time series and drive processing, or can add Gaussian noise to an existing time series.

     Basic time series formation is dealt with in KTTSGenerator.

     Available configuration options:
     - Inherited from KTTSGenerator
       - "number-of-slices": unsigned -- Number of slices to create (used only if creating new slices)
       - "n-channels": unsigned -- Number of channels per slice to create (used only if creating new slices)
       - "slice-size": unsigned -- Specify the size of the time series (used only if creating new slices)
       - "bin-width": double -- Specify the bin width
       - "time-series-type": string -- Type of time series to produce (options: real [default], fftw)
       - "record-size": unsigned -- Size of the imaginary record that this slice came from (only used to fill in the egg header; does not affect the simulation at all)
     - From KTGaussianNoiseGenerator
       - "mean": double -- Mean for the randomly-chosen time-series values
       - "sigma": double -- Standard deviation for the randomly-chosen time-series values

     Slots: (inherited from KTTSGenerator)
     - "slice": void (KTDataPtr) -- Add a signal to an existing time series; Requires KTTimeSeriesData; Emits signal "slice" when done.

     Signals: (inherited from KTTSGenerator)
     - "header": void (KTEggHeader*) -- emitted when the egg header is created.
     - "slice": void (KTDataPtr) -- emitted when the new time series is produced or processed.
     - "done": void () --  emitted when the job is complete.
    */
    class KTGaussianNoiseGenerator : public KTTSGenerator
    {
        public:
            KTGaussianNoiseGenerator(const std::string& name = "gaussian-noise-generator");
            virtual ~KTGaussianNoiseGenerator();

            virtual bool ConfigureDerivedGenerator(const scarab::param_node* node);

            double GetMean() const;
            void SetMean(double mean);

            double GetSigma() const;
            void SetSigma(double sigma);

        private:
            KTRNGGaussian<> fRNG;

        public:
            virtual bool GenerateTS(KTTimeSeriesData& data);

    };

    inline double KTGaussianNoiseGenerator::GetMean() const
    {
        return fRNG.mean();
    }

    inline void KTGaussianNoiseGenerator::SetMean(double mean)
    {
        fRNG.param(KTRNGGaussian<>::param_type(mean, GetSigma()));
        return;
    }

    inline double KTGaussianNoiseGenerator::GetSigma() const
    {
        return fRNG.sigma();
    }

    inline void KTGaussianNoiseGenerator::SetSigma(double sigma)
    {
        fRNG.param(KTRNGGaussian<>::param_type(GetMean(), sigma));
        return;
    }

} /* namespace Katydid */
#endif /* KTGAUSSIANNOISEGENERATOR_HH_ */
