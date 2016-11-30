/**
 @file KTLowPassFilter.hh
 @brief Contains KTLowPassFilter
 @details Applies an RC low-pass filter
 @author: N.S. Oblath
 @date: Nov 3, 2014
 */

#ifndef KTLOWPASSFILTER_HH_
#define KTLOWPASSFILTER_HH_

#include "KTProcessor.hh"

#include "KTMemberVariable.hh"
#include "KTSlot.hh"

#include <cmath>

namespace scarab
{
    class param_node;
}

namespace Katydid
{
    class KTFrequencySpectrumDataFFTW;
    class KTFrequencySpectrumDataPolar;
    class KTFrequencySpectrumFFTW;
    class KTFrequencySpectrumPolar;
    class KTPowerSpectrum;
    class KTPowerSpectrumData;

    /*!
     @class KTLowPassFilter
     @author N.S. Oblath

     @brief Applies a low-pass filter

     @details
     Non-ideal implementation of a low-pass filter processor.

     Configuration name: "low-pass-filter"

     Available configuration values:
     - "rc": double -- RC time constant of the filter

     Slots:
     - "fs-polar": void (KTDataPtr) -- Applies a low-pass filter; Requires KTFrequencySpectrumDataPolar; Adds KTLowPassFilteredFSDataPolar; Emits signal "fs-polar"
     - "fs-fftw": void (KTDataPtr) -- Applies a low-pass filter; Requires KTFrequencySpectrumDataFFTW; Adds KTLowPassFilteredFSDataFFTW; Emits signal "fs-fftw"
     - "ps": void (KTDataPtr) -- Applies a low-pass filter; Requires KTPowerSpectrum; Adds KTLowPassFilteredPSData; Emits signal "ps"

     Signals:
     - "fs-polar": void (KTDataPtr) -- Emitted upon low-pass filtering; Guarantees KTLowPassFilteredFSDataPolar.
     - "fs-fftw": void (KTDataPtr) -- Emitted upon low-pass filtering; Guarantees KTLowPassFilteredFSDataFFTW.
     - "ps": void (KTDataPtr) -- Emitted upon low-pass filtering; Guarantees KTLowPassFilteredPSData.
    */
    class KTLowPassFilter : public Nymph::KTProcessor
    {
        public:
            KTLowPassFilter(const std::string& name = "low-pass-filter");
            virtual ~KTLowPassFilter();

            bool Configure(const scarab::param_node* node);

            MEMBERVARIABLE(double, RC);


        public:
            bool Filter(KTFrequencySpectrumDataPolar& fsData);
            bool Filter(KTFrequencySpectrumDataFFTW& fsData);
            bool Filter(KTPowerSpectrumData& psData);

            KTFrequencySpectrumPolar* Filter(const KTFrequencySpectrumPolar* frequencySpectrum) const;
            KTFrequencySpectrumFFTW* Filter(const KTFrequencySpectrumFFTW* frequencySpectrum) const;
            KTPowerSpectrum* Filter(const KTPowerSpectrum* powerSpectrum) const;

            //***************
            // Signals
            //***************

        private:
            Nymph::KTSignalData fFSPolarSignal;
            Nymph::KTSignalData fFSFFTWSignal;
            Nymph::KTSignalData fPSSignal;

            //***************
            // Slots
            //***************

        private:
            Nymph::KTSlotDataOneType< KTFrequencySpectrumDataPolar > fFSPolarSlot;
            Nymph::KTSlotDataOneType< KTFrequencySpectrumDataFFTW > fFSFFTWSlot;
            Nymph::KTSlotDataOneType< KTPowerSpectrumData > fPSSlot;

    };

}
 /* namespace Katydid */
#endif /* KTLOWPASSFILTER_HH_ */
