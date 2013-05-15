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

    /*!
     @class KTGaussianNoiseGenerator
     @author N. S. Oblath

     @brief Generates a time series with Gaussian noise

     @details
     Can create a new time series and drive processing, or can add Gaussian noise to an existing time series.

     Basic time series formation is dealt with in KTTSGenerator.

     Available configuration options:
     - Inherited from KTTSGenerator
       - "n-slices": UInt_t -- Number of slices to create (used only if creating new slices)
       - "n-channels": UInt_t -- Number of channels per slice to create (used only if creating new slices)
       - "time-series-size": UInt_t -- Specify the size of the time series (used only if creating new slices)
       - "bin-width": Double_t -- Specify the bin width
       - "time-series-type": string -- Type of time series to produce (options: real [default], fftw)
       - "record-size": UInt_t -- Size of the imaginary record that this slice came from (only used to fill in the egg header; does not affect the simulation at all)
     - From KTGaussianNoiseGenerator
       - "mean": Double_t -- Mean for the randomly-chosen time-series values
       - "sigma": Double_t -- Standard deviation for the randomly-chosen time-series values

     Slots: (inherited from KTTSGenerator)
     - "slice": void (boost::shared_ptr<KTData>) -- Add a signal to an existing time series; Requires KTTimeSeriesData; Emits signal "slice" when done.

     Signals: (inherited from KTTSGenerator)
     - "header": void (const KTEggHeader*) -- emitted when the egg header is created.
     - "slice": void (boost::shared_ptr<KTData>) -- emitted when the new time series is produced or processed.
     - "done": void () --  emitted when the job is complete.
    */
    class KTGaussianNoiseGenerator : public KTTSGenerator
    {
        public:
            KTGaussianNoiseGenerator(const std::string& name = "gaussian-noise-generator");
            virtual ~KTGaussianNoiseGenerator();

            virtual Bool_t ConfigureDerivedGenerator(const KTPStoreNode* node);

            Double_t GetMean() const;
            void SetMean(Double_t mean);

            Double_t GetSigma() const;
            void SetSigma(Double_t sigma);

        private:
            KTRNGGaussian<> fRNG;

        public:
            virtual Bool_t GenerateTS(KTTimeSeriesData& data);

    };

    inline Double_t KTGaussianNoiseGenerator::GetMean() const
    {
        return fRNG.mean();
    }

    inline void KTGaussianNoiseGenerator::SetMean(Double_t mean)
    {
        fRNG.param(KTRNGGaussian<>::param_type(mean, GetSigma()));
        return;
    }

    inline Double_t KTGaussianNoiseGenerator::GetSigma() const
    {
        return fRNG.sigma();
    }

    inline void KTGaussianNoiseGenerator::SetSigma(Double_t sigma)
    {
        fRNG.param(KTRNGGaussian<>::param_type(GetMean(), sigma));
        return;
    }

} /* namespace Katydid */
#endif /* KTGAUSSIANNOISEGENERATOR_HH_ */
