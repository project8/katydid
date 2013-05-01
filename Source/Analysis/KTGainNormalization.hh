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

#include <boost/shared_ptr.hpp>

namespace Katydid
{
    class KTData;
    class KTFrequencySpectrumDataFFTW;
    class KTFrequencySpectrumDataPolar;
    class KTFrequencySpectrumFFTW;
    class KTFrequencySpectrumPolar;
    class KTGainVariationData;
    class KTPStoreNode;
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

            Bool_t Configure(const KTPStoreNode* node);

            Double_t GetMinFrequency() const;
            void SetMinFrequency(Double_t freq);

            Double_t GetMaxFrequency() const;
            void SetMaxFrequency(Double_t freq);

            UInt_t GetMinBin() const;
            void SetMinBin(UInt_t bin);

            UInt_t GetMaxBin() const;
            void SetMaxBin(UInt_t bin);

        private:
            Double_t fMinFrequency;
            Double_t fMaxFrequency;
            UInt_t fMinBin;
            UInt_t fMaxBin;
            Bool_t fCalculateMinBin;
            Bool_t fCalculateMaxBin;

        public:
            Bool_t Normalize(KTFrequencySpectrumDataPolar& fsData, KTGainVariationData& gvData);
            Bool_t Normalize(KTFrequencySpectrumDataFFTW& fsData, KTGainVariationData& gvData);

            //void Normalize(KTSlidingWindowFSData* swFSData, const KTGainVariationData* gvData);
            //void Normalize(KTSlidingWindowFSDataFFTW* swFSData, const KTGainVariationData* gvData);

            KTFrequencySpectrumPolar* Normalize(const KTFrequencySpectrumPolar* frequencySpectrum, const KTSpline* spline);
            KTFrequencySpectrumFFTW* Normalize(const KTFrequencySpectrumFFTW* frequencySpectrum, const KTSpline* spline);

            //***************
            // Signals
            //***************

        private:
            KTSignalData fFSPolarSignal;
            KTSignalData fFSFFTWSignal;
            //KTSignalData fSWFSSignal;
            //KTSignalData fSWFSFFTWSignal;

            //***************
            // Slots
            //***************

        private:
            KTSlotDataTwoTypes< KTFrequencySpectrumDataPolar, KTGainVariationData > fFSPolarSlot;
            KTSlotDataTwoTypes< KTFrequencySpectrumDataFFTW, KTGainVariationData > fFSFFTWSlot;

    };

    inline Double_t KTGainNormalization::GetMinFrequency() const
    {
        return fMinFrequency;
    }

    inline void KTGainNormalization::SetMinFrequency(Double_t freq)
    {
        fMinFrequency = freq;
        fCalculateMinBin = true;
        return;
    }

    inline Double_t KTGainNormalization::GetMaxFrequency() const
    {
        return fMaxFrequency;
    }

    inline void KTGainNormalization::SetMaxFrequency(Double_t freq)
    {
        fMaxFrequency = freq;
        fCalculateMaxBin = true;
        return;
    }

    inline UInt_t KTGainNormalization::GetMinBin() const
    {
        return fMinBin;
    }

    inline void KTGainNormalization::SetMinBin(UInt_t bin)
    {
        fMinBin = bin;
        fCalculateMinBin = false;
        return;
    }

    inline UInt_t KTGainNormalization::GetMaxBin() const
    {
        return fMaxBin;
    }

    inline void KTGainNormalization::SetMaxBin(UInt_t bin)
    {
        fMaxBin = bin;
        fCalculateMaxBin = false;
        return;
    }

} /* namespace Katydid */
#endif /* KTGAINNORMALIZATION_HH_ */
