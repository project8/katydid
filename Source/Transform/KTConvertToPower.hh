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
    class KTFrequencySpectrumDataPolar;
    class KTParamNode;
    class KTPowerSpectrumData;

    /*!
     @class KTConvertToPower
     @author N.S. Oblath

     @brief Converts to power spectra and power spectral densities

     @details

     Configuration name: "convert-to-power"

     Available configuration values:
       none

     Slots:
     - "fs-polar-to-ps": void (KTDataPtr) -- Converts a polar FS to a PS; Requires KTFrequencySpectrumDataPolar; Adds KTPowerSpectrumData; Emits signal "ps"
     - "fs-polar-to-psd": void (KTDataPtr) -- Converts a polar FS to a PSD; Requires KTFrequencySpectrumDataPolar; Adds KTPowerSpectrumData; Emits signal "psd"
     - "fs-fftw-to-ps": void (KTDataPtr) -- Converts an FFTW FS to a PS; Requires KTFrequencySpectrumDataFFTW; Adds KTPowerSpectrumData; Emits signal "ps"
     - "fs-fftw-to-psd": void (KTDataPtr) -- Converts an FFTW FS to a PSD; Requires KTFrequencySpectrumDataFFTW; Adds KTPowerSpectrumData; Emits signal "psd"
     - "psd-to-ps": void (KTDataPtr) -- Converts a PSD to a PS (in place); Requires KTPowerSpectrumData; Does not add additional data; Emits signal "ps"
     - "ps-to-psd": void (KTDataPtr) -- Converts a PS to a PSD (in place); Requires KTPowerSpectrumData; Does not add additional data; Emits signal "psd"

     Signals:
     - "ps": void (KTDataPtr) -- Emitted upon creation of / conversion to a power spectrum; Guarantees KTPowerSpectrumData.
     - "psd": void (KTDataPtr) -- Emitted upon creation of / conversion to a power spectral density; Guarantees KTPowerSpectrumData.
    */

    class KTConvertToPower : public KTProcessor
    {

        public:
            KTConvertToPower(const std::string& name = "convert-to-power");
            virtual ~KTConvertToPower();

            bool Configure(const KTParamNode* node);

        private:

        public:
            bool ToPowerSpectrum(KTFrequencySpectrumDataPolar& data);
            bool ToPowerSpectralDensity(KTFrequencySpectrumDataPolar& data);

            bool ToPowerSpectrum(KTFrequencySpectrumDataFFTW& data);
            bool ToPowerSpectralDensity(KTFrequencySpectrumDataFFTW& data);

            bool ToPowerSpectrum(KTPowerSpectrumData& data);
            bool ToPowerSpectralDensity(KTPowerSpectrumData& data);

        private:

            //***************
            // Signals
            //***************

        private:
            KTSignalData fPowerSpectrumSignal;
            KTSignalData fPowerSpectralDensitySignal;

            //***************
            // Slots
            //***************

        private:
            KTSlotDataOneType< KTFrequencySpectrumDataPolar > fFSPToPSSlot;
            KTSlotDataOneType< KTFrequencySpectrumDataPolar > fFSPToPSDSlot;
            KTSlotDataOneType< KTFrequencySpectrumDataFFTW > fFSFToPSSlot;
            KTSlotDataOneType< KTFrequencySpectrumDataFFTW > fFSFToPSDSlot;
            KTSlotDataOneType< KTPowerSpectrumData > fPSDToPSSlot;
            KTSlotDataOneType< KTPowerSpectrumData > fPSToPSDSlot;
    };
}
 /* namespace Katydid */
#endif /* KTCONVERTTOPOWER_HH_ */
