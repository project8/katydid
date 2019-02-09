/**
 @file KTBilateralFilter.hh
 @brief Contains KTBilateralFilter
 @details Applies a direct bilateral filter to time-frequency plane
 @author: N. Buzinsky
 @date: Mar 7, 2018
 */

#ifndef KTBILATERALFILTER_HH_
#define KTBILATERALFILTER_HH_

#include "KTProcessor.hh"

#include "KTMemberVariable.hh"
#include "KTSlot.hh"
#include "KTMultiFSDataFFTW.hh"

#include <cmath>

namespace scarab
{
    class param_node;
}

namespace Katydid
{
    class KTFrequencySpectrumDataFFTW;
    class KTFrequencySpectrumFFTW;

    /*!
     @class KTBilateralFilter
     @author N. Buzinsky

     @brief Applies a bilateral filter

     @details
     Approximate implementation of a bilateral filter processor: https://en.wikipedia.org/wiki/Bilateral_filter 

     Configuration name: "bilateral-filter"

     Available configuration values:
     - "sigma-pixels": double -- width of gaussian for averaging, in pixels
     - "sigma-range": double -- width of gaussian for averaging, in terms of differences in power

     Slots:
     - "str-fs-fftw": void (KTDataPtr) -- Applies a bilateral filter; Requires KTMultiFSDataFFTW; Adds KTBilateralFilteredFSDataFFTW; Emits signal "fs-fftw"

     Signals:
     - "fs-fftw": void (KTDataPtr) -- Emitted upon bilateral filtering; Guarantees KTBilateralFilteredFSDataFFTW. 
    */
    class KTBilateralFilter : public Nymph::KTProcessor
    {
        public:
            KTBilateralFilter(const std::string& name = "bilateral-filter");
            virtual ~KTBilateralFilter();

            bool Configure(const scarab::param_node* node);

            MEMBERVARIABLE(double, SigmaPixels);
            MEMBERVARIABLE(double, SigmaRange);

        public:
            bool Filter(KTMultiFSDataFFTW& fsData);

            KTFrequencySpectrumFFTW* Filter(const KTMultiFSFFTW * frequencySpectrum) const;

            //***************
            // Signals
            //***************

        private:
            Nymph::KTSignalData fFSFFTWSignal;

            //***************
            // Slots
            //***************

        private:
            Nymph::KTSlotDataOneType< KTMultiFSDataFFTW > fFSFFTWSlot;
            double GaussianWeightRange(const fftw_complex &I1, const fftw_complex &I2) const; //Weight due to difference in powers between pixels
            double GaussianWeightPixels(const double &i, const double &j,const double &k,const double &l) const; //Weight between pixels at (i,j) and (k,l) due to spacing


    };

}
 /* namespace Katydid */
#endif /* KTBILATERALFILTER_HH_ */
