/**
 @file KTGainNormalization.hh
 @brief Contains KTGainNormalization
 @details Normalizes a frequency spectrum using a KTGainVariationData object.
 @author: N. S. Oblath
 @date: Jan 24, 2012
 */

#ifndef KTGAINNORMALIZATION_HH_
#define KTGAINNORMALIZATION_HH_

#include "KTProcessor.hh"

#include "KTGainVariationData.hh"
#include "KTSlot.hh"

namespace Katydid
{
    
    class KTFrequencySpectrumDataFFTW;
    class KTFrequencySpectrumDataPolar;
    class KTFrequencySpectrumFFTW;
    class KTFrequencySpectrumPolar;
    class KTPowerSpectrum;
    class KTPowerSpectrumData;
    class KTSpline;
 

   /*!
     @class KTGainNormalization
     @author N. S. Oblath

     @brief Normalizes a frequency spectrum using a KTGainVariationData object.

     @details
 
     Configuration name: "gain-normalization"

     Available configuration values:
     - "min-bin": unsigned -- Set the lower bound of the range that gets normalized by bin number.
     - "max-bin": unsigned -- Set the upper bound of the range that gets normalized by bin number.
     - "min-frequency": double -- Set the lower bound of the range that gets normalized by frequency.
     - "max-frequency": double -- Set the upper bound of the range that gets normalized by frequency.

     Slots:
     - "fs-polar": void (Nymph::KTDataPtr) -- Normalize a frequency spectrum; Requires KTFrequencySpectrumDataPolar and KTGainVariationData; Adds KTNormalizedFSDataPolar; Emits signal norm-fs-polar
     - "fs-polar-pre": void (Nymph::KTDataPtr) -- Normalize a frequency spectrum based on the pre-calculated gain variation; Requires KTFrequencySpectrumDataPolar; Adds KTNormalizedFSDataPolar; Emits signal norm-fs-polar
     - "fs-fftw": void (Nymph::KTDataPtr) -- Normalize a frequency spectrum; Requires KTFrequencySpectrumDataFFTW and KTGainVariationData; Adds KTNormalizedFSDataFFTW; Emits signal norm-fs-fftw
     - "fs-fftw-pre": void (Nymph::KTDataPtr) -- Normalize a frequency spectrum based on the pre-calculated gain variation; Requires KTFrequencySpectrumDataFFTW; Adds KTNormalizedFSDataFFTW; Emits signal norm-fs-fftw
     - "ps": void (Nymph::KTDataPtr) -- Normalizes a power spectrum; Requires KTPowerSpectrumData and KTGainVariationData; Adds KTNormalizedPSData; Emits signal norm-ps
     - "ps-pre": void (Nymph::KTDataPtr) -- Normalizes a power spectrum based on the pre-calculated gain variation; Requires KTPowerSpectrumData; Adds KTNormalizedPSData; Emits signal norm-ps
     - "gv": void (Nymph::KTDataPtr) -- Sets the pre-calculated gain-variation data; Requires KTGainVariationData

     Signals:
     - "norm-fs-polar": void (Nymph::KTDataPtr) emitted upon performance of a normalization of a polar frequency spectrum data object; Guarantees KTNormalizedFSDataPolar
     - "norm-fs-fftw": void (Nymph::KTDataPtr) emitted upon performance of a normalization of an FFTW frequency spectrum data object; Guarantees KTNormalizedFSDataFFTW
     - "norm-ps": void (Nymph::KTDataPtr) emitted upon performance of a normalization of a power spectrum data object; Guarantees KTNormalizedPSData
    */

    class KTGainNormalization : public Nymph::KTProcessor
    {
        public:
            KTGainNormalization(const std::string& name = "gain-normalization");
            virtual ~KTGainNormalization();

            bool Configure(const scarab::param_node* node);

            double GetMinFrequency() const;
            void SetMinFrequency(double freq);

            double GetMaxFrequency() const;
            void SetMaxFrequency(double freq);

            unsigned GetMinBin() const;
            void SetMinBin(unsigned bin);

            unsigned GetMaxBin() const;
            void SetMaxBin(unsigned bin);

        private:
            double fMinFrequency;
            double fMaxFrequency;
            unsigned fMinBin;
            unsigned fMaxBin;
            bool fCalculateMinBin;
            bool fCalculateMaxBin;

        public:
            bool SetPreCalcGainVar(KTGainVariationData& gvData);

            bool Normalize(KTFrequencySpectrumDataPolar& fsData);
            bool Normalize(KTFrequencySpectrumDataFFTW& fsData);
            bool Normalize(KTPowerSpectrumData& psData);

            bool Normalize(KTFrequencySpectrumDataPolar& fsData, KTGainVariationData& gvData);
            bool Normalize(KTFrequencySpectrumDataFFTW& fsData, KTGainVariationData& gvData);
            bool Normalize(KTPowerSpectrumData& psData, KTGainVariationData& gvdata);

            KTFrequencySpectrumPolar* Normalize(const KTFrequencySpectrumPolar* frequencySpectrum, const KTSpline* spline, const KTSpline* varSpline);
            KTFrequencySpectrumFFTW* Normalize(const KTFrequencySpectrumFFTW* frequencySpectrum, const KTSpline* spline, const KTSpline* varSpline);
            KTPowerSpectrum* Normalize(const KTPowerSpectrum* powerSpectrum, const KTSpline* spline, const KTSpline* varSpline);

        private:
            KTGainVariationData fGVData;
            std::vector< double > fMagnitudeCache;
            double fMeanGV;
            double fMeanGVVariance;

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
            Nymph::KTSlotDataTwoTypes< KTFrequencySpectrumDataPolar, KTGainVariationData > fFSPolarSlot;
            Nymph::KTSlotDataTwoTypes< KTFrequencySpectrumDataFFTW, KTGainVariationData > fFSFFTWSlot;
            Nymph::KTSlotDataTwoTypes< KTPowerSpectrumData, KTGainVariationData > fPSSlot;

            Nymph::KTSlotDataOneType< KTGainVariationData > fPreCalcSlot;

            Nymph::KTSlotDataOneType< KTFrequencySpectrumDataPolar > fFSPolarPreCalcSlot;
            Nymph::KTSlotDataOneType< KTFrequencySpectrumDataFFTW > fFSFFTWPreCalcSlot;
            Nymph::KTSlotDataOneType< KTPowerSpectrumData > fPSPreCalcSlot;

    };

    inline double KTGainNormalization::GetMinFrequency() const
    {
        return fMinFrequency;
    }

    inline void KTGainNormalization::SetMinFrequency(double freq)
    {
        fMinFrequency = freq;
        fCalculateMinBin = true;
        return;
    }

    inline double KTGainNormalization::GetMaxFrequency() const
    {
        return fMaxFrequency;
    }

    inline void KTGainNormalization::SetMaxFrequency(double freq)
    {
        fMaxFrequency = freq;
        fCalculateMaxBin = true;
        return;
    }

    inline unsigned KTGainNormalization::GetMinBin() const
    {
        return fMinBin;
    }

    inline void KTGainNormalization::SetMinBin(unsigned bin)
    {
        fMinBin = bin;
        fCalculateMinBin = false;
        return;
    }

    inline unsigned KTGainNormalization::GetMaxBin() const
    {
        return fMaxBin;
    }

    inline void KTGainNormalization::SetMaxBin(unsigned bin)
    {
        fMaxBin = bin;
        fCalculateMaxBin = false;
        return;
    }

} /* namespace Katydid */
#endif /* KTGAINNORMALIZATION_HH_ */
