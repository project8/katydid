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

#include "KTSlot.hh"

namespace Katydid
{
    class KTFrequencySpectrumDataFFTW;
    class KTFrequencySpectrumDataPolar;
    class KTFrequencySpectrumFFTW;
    class KTFrequencySpectrumPolar;
    class KTGainVariationData;
    class KTParamNode;
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
     - "max-bin": unsigned -- Set the upper bound of the range that gets nornalized by bin number.
     - "min-frequency": double -- Set the lower bound of the range that gets normalized by frequency.
     - "max-frequency": double -- Set the upper bound of the range that gets normalized by frequency.

     Slots:
     - "fs-polar": void (shared_data< KTData >) -- Normalize a frequency spectrum; Requires KTFrequencySpectrumDataPolar and KTGainVariationData; Adds KTNormalizedFSDataPolar
     - "fs-fftw": void (shared_data< KTData >) -- Normalize a frequency spectrum; Requires KTFrequencySpectrumDataFFTW and KTGainVariationData; Adds KTNormalizedFSDataFFTW

     Signals:
     - "norm-fs-polar": void (shared_data< KTData >) emitted upon performance of a normalization of a polar frequency spectrum data object; Guarantees KTNormalizedFSDataPolar
     - "norm-fs-fftw": void (shared_data< KTData >) emitted upon performance normalization of an FFTW frequency spectrum data object; Guarantees KTNormalizedFSDataFFTW
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
            bool Normalize(KTFrequencySpectrumDataPolar& fsData, KTGainVariationData& gvData);
            bool Normalize(KTFrequencySpectrumDataFFTW& fsData, KTGainVariationData& gvData);
            bool Normalize(KTPowerSpectrumData& psData, KTGainVariationData& gvdata);

            //void Normalize(KTSlidingWindowFSData* swFSData, const KTGainVariationData* gvData);
            //void Normalize(KTSlidingWindowFSDataFFTW* swFSData, const KTGainVariationData* gvData);

            KTFrequencySpectrumPolar* Normalize(const KTFrequencySpectrumPolar* frequencySpectrum, const KTSpline* spline);
            KTFrequencySpectrumFFTW* Normalize(const KTFrequencySpectrumFFTW* frequencySpectrum, const KTSpline* spline);
            KTPowerSpectrum* Normalize(const KTPowerSpectrum* powerSpectrum, const KTSpline* spline);

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
