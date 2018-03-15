/**
 @file KTBilateralFilter.hh
 @brief Contains KTBilateralFilter
 @details Applies a direct bilateral filter to time-freq plane
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
     Approximate implementation of a bilateral filter processor.

     Configuration name: "bilateral-filter"

     Available configuration values:
     - "sigmaPixels": double -- width of gaussian spatial
     - "sigmaRange": double -- width of gaussian for coloring

     Slots:
     - "str-fs-fftw": void (KTDataPtr) -- Applies a bilateral filter; Requires KTFrequencySpectrumDataFFTW; Adds KTBilateralFilteredFSDataFFTW; Emits signal "fs-fftw"

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
            int nIters;


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
            //Nymph::KTSlotDataOneType< KTFrequencySpectrumDataFFTW > fFSFFTWSlot;
            Nymph::KTSlotDataOneType< KTMultiFSDataFFTW > fFSFFTWSlot;

    };

}
 /* namespace Katydid */
#endif /* KTBILATERALFILTER_HH_ */
