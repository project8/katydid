/**
 @file KTConvertToPower.hh
 @brief Contains KTConvertToPower
 @details Converts frequency spectra to power spectra and power spectral densities
 @author: nsoblath
 @date: Aug 1, 2014
 */

#ifndef KTCONVERTTOPOWER_HH_
#define KTCONVERTTOPOWER_HH_

#include "KTProcessor.hh"

#include "KTSlot.hh"


namespace Katydid
{
    
    class KTFrequencySpectrumDataFFTW;
    class KTChirpSpaceDataFFT;
    class KTFrequencySpectrumDataPolar;
    class KTPowerSpectrumData;
    class KTAggregatedFrequencySpectrumDataFFTW;

    /*!
     @class KTConvertToPower
     @author N.S. Oblath

     @brief Converts to power spectra and power spectral densities

     @details

     Configuration name: "convert-to-power"

     Available configuration values:
       none

     Slots:
     - "fs-polar-to-ps": void (Nymph::KTDataPtr) -- Converts a polar FS to a PS; Requires KTFrequencySpectrumDataPolar; Adds KTPowerSpectrumData; Emits signal "ps"
     - "fs-polar-to-psd": void (Nymph::KTDataPtr) -- Converts a polar FS to a PSD; Requires KTFrequencySpectrumDataPolar; Adds KTPowerSpectrumData; Emits signal "psd"
     - "fs-fftw-to-ps": void (Nymph::KTDataPtr) -- Converts an FFTW FS to a PS; Requires KTFrequencySpectrumDataFFTW; Adds KTPowerSpectrumData; Emits signal "ps"
     - "fs-fftw-to-psd": void (Nymph::KTDataPtr) -- Converts an FFTW FS to a PSD; Requires KTFrequencySpectrumDataFFTW; Adds KTPowerSpectrumData; Emits signal "psd"
     - "fs-fft-to-psd": void (Nymph::KTDataPtr) -- Converts an FFT FS to a PSD; Requires KTChirpSpaceDataFFTW; Adds KTPowerSpectrumData; Emits signal "psd"
     - "aggfs-fftw-to-ps": void (Nymph::KTDataPtr) -- Converts an aggregated FFTW FS to a PS; Requires KTAggregatedFrequencySpectrumDataFFTW; Adds KTPowerSpectrumData; Emits signal "ps"
     - "aggfs-fftw-to-psd": void (Nymph::KTDataPtr) -- Converts an FFTW FS to a PSD; Requires KTAggregatedFrequencySpectrumDataFFTW; Adds KTPowerSpectrumData; Emits signal "psd"
     - "psd-to-ps": void (Nymph::KTDataPtr) -- Converts a PSD to a PS (in place); Requires KTPowerSpectrumData; Does not add additional data; Emits signal "ps"
     - "ps-to-psd": void (Nymph::KTDataPtr) -- Converts a PS to a PSD (in place); Requires KTPowerSpectrumData; Does not add additional data; Emits signal "psd"

     Signals:
     - "ps": void (Nymph::KTDataPtr) -- Emitted upon creation of / conversion to a power spectrum; Guarantees KTPowerSpectrumData.
     - "psd": void (Nymph::KTDataPtr) -- Emitted upon creation of / conversion to a power spectral density; Guarantees KTPowerSpectrumData.
    */

    class KTConvertToPower : public Nymph::KTProcessor
    {

        public:
            KTConvertToPower(const std::string& name = "convert-to-power");
            virtual ~KTConvertToPower();

            bool Configure(const scarab::param_node* node);

        private:

        public:
            bool ToPowerSpectrum(KTFrequencySpectrumDataPolar& data);
            bool ToPowerSpectralDensity(KTFrequencySpectrumDataPolar& data);

            bool ToPowerSpectrum(KTFrequencySpectrumDataFFTW& data);
            bool ToPowerSpectralDensity(KTFrequencySpectrumDataFFTW& data);

	    bool ToPowerSpectralDensity(KTChirpSpaceDataFFT& data);
        
            bool ToPowerSpectrum(KTAggregatedFrequencySpectrumDataFFTW& data);
            bool ToPowerSpectralDensity(KTAggregatedFrequencySpectrumDataFFTW& data);

            bool ToPowerSpectrum(KTPowerSpectrumData& data);
            bool ToPowerSpectralDensity(KTPowerSpectrumData& data);

        private:

            //***************
            // Signals
            //***************

        private:
            Nymph::KTSignalData fPowerSpectrumSignal;
            Nymph::KTSignalData fPowerSpectralDensitySignal;

            //***************
            // Slots
            //***************

        private:
            Nymph::KTSlotDataOneType< KTFrequencySpectrumDataPolar > fFSPToPSSlot;
            Nymph::KTSlotDataOneType< KTFrequencySpectrumDataPolar > fFSPToPSDSlot;
            Nymph::KTSlotDataOneType< KTFrequencySpectrumDataFFTW > fFSFToPSSlot;
            Nymph::KTSlotDataOneType< KTFrequencySpectrumDataFFTW > fFSFToPSDSlot;

	    Nymph::KTSlotDataOneType< KTChirpSpaceDataFFT > fCSFToPSDSlot;

            Nymph::KTSlotDataOneType< KTAggregatedFrequencySpectrumDataFFTW > fAggFSFToPSSlot;
            Nymph::KTSlotDataOneType< KTAggregatedFrequencySpectrumDataFFTW > fAggFSFToPSDSlot;
            Nymph::KTSlotDataOneType< KTPowerSpectrumData > fPSDToPSSlot;
            Nymph::KTSlotDataOneType< KTPowerSpectrumData > fPSToPSDSlot;
    };
}
 /* namespace Katydid */
#endif /* KTCONVERTTOPOWER_HH_ */
