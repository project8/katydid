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

namespace Nymph
{
    class KTParamNode;
}
;

namespace Katydid
{
    using namespace Nymph;
    class KTFrequencySpectrumDataFFTW;
    class KTFrequencySpectrumDataPolar;
    class KTFrequencySpectrumFFTW;
    class KTFrequencySpectrumPolar;
    class KTPowerSpectrum;
    class KTPowerSpectrumData;
    //class KTSlidingWindowFSData;
    //class KTSlidingWindowFSDataFFTW;
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
     - "fs-polar": void (KTDataPtr) -- Normalize a frequency spectrum; Requires KTFrequencySpectrumDataPolar and KTGainVariationData; Adds KTNormalizedFSDataPolar; Emits signal norm-fs-polar
     - "fs-fftw": void (KTDataPtr) -- Normalize a frequency spectrum; Requires KTFrequencySpectrumDataFFTW and KTGainVariationData; Adds KTNormalizedFSDataFFTW; Emits signal norm-fs-fftw
     - "ps": void (KTDataPtr) -- Normalizes a power spectrum; Requires KTPowerSpectrumData and KTGainVariationData; Adds KTNormalizedPSData; Emits signal norm-ps
     - "ps-pre": void (KTDataPtr) -- Normalizes a power spectrum based on the pre-calculated gain variation; Requires KTPowerSpectrumData; Adds KTNormalizedPSData; Emits signal norm-ps
     - "gv": void (KTDataPtr) -- Sets the pre-calculated gain-variation data; Requires KTGainVariationData

     Signals:
     - "norm-fs-polar": void (KTDataPtr) emitted upon performance of a normalization of a polar frequency spectrum data object; Guarantees KTNormalizedFSDataPolar
     - "norm-fs-fftw": void (KTDataPtr) emitted upon performance of a normalization of an FFTW frequency spectrum data object; Guarantees KTNormalizedFSDataFFTW
     - "norm-ps": void (KTDataPtr) emitted upon performance of a normalization of a power spectrum data object; Guarantees KTNormalizedPSData
    */

    class KTGainNormalization : public KTProcessor
    {
        public:
            KTGainNormalization(const std::string& name = "gain-normalization");
            virtual ~KTGainNormalization();

            bool Configure(const KTParamNode* node);

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

            bool Normalize(KTPowerSpectrumData& psData);

            bool Normalize(KTFrequencySpectrumDataPolar& fsData, KTGainVariationData& gvData);
            bool Normalize(KTFrequencySpectrumDataFFTW& fsData, KTGainVariationData& gvData);
            bool Normalize(KTPowerSpectrumData& psData, KTGainVariationData& gvdata);

            //void Normalize(KTSlidingWindowFSData* swFSData, const KTGainVariationData* gvData);
            //void Normalize(KTSlidingWindowFSDataFFTW* swFSData, const KTGainVariationData* gvData);

            KTFrequencySpectrumPolar* Normalize(const KTFrequencySpectrumPolar* frequencySpectrum, const KTSpline* spline);
            KTFrequencySpectrumFFTW* Normalize(const KTFrequencySpectrumFFTW* frequencySpectrum, const KTSpline* spline);
            KTPowerSpectrum* Normalize(const KTPowerSpectrum* powerSpectrum, const KTSpline* spline);

        private:
            KTGainVariationData fGVData;
            std::vector< double > fMagnitudeCache;

            //***************
            // Signals
            //***************

        private:
            KTSignalData fFSPolarSignal;
            KTSignalData fFSFFTWSignal;
            KTSignalData fPSSignal;
            //KTSignalData fSWFSSignal;
            //KTSignalData fSWFSFFTWSignal;

            //***************
            // Slots
            //***************

        private:
            KTSlotDataTwoTypes< KTFrequencySpectrumDataPolar, KTGainVariationData > fFSPolarSlot;
            KTSlotDataTwoTypes< KTFrequencySpectrumDataFFTW, KTGainVariationData > fFSFFTWSlot;
            KTSlotDataTwoTypes< KTPowerSpectrumData, KTGainVariationData > fPSSlot;

            KTSlotDataOneType< KTGainVariationData > fPreCalcSlot;

            KTSlotDataOneType< KTPowerSpectrumData > fPSPreCalcSlot;

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
