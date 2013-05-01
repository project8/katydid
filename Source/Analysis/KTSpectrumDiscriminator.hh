/**
 @file KTSpectrumDiscriminator.hh
 @brief Contains KTSpectrumDiscriminator
 @details Discriminates Spectrum
 @author: N. S. Oblath
 @date: Dec 12, 2012
 */

#ifndef KTSPECTRUMDISCRIMINATOR_HH_
#define KTSPECTRUMDISCRIMINATOR_HH_

#include "KTProcessor.hh"

#include "KTSlot.hh"

#include <boost/shared_ptr.hpp>


namespace Katydid
{
    class KTCorrelationData;
    class KTData;
    class KTDiscriminatedPoints1DData;
    class KTFrequencySpectrumDataFFTW;
    class KTFrequencySpectrumDataFFTWCore;
    class KTFrequencySpectrumDataPolar;
    class KTFrequencySpectrumDataPolarCore;
    class KTNormalizedFSDataFFTW;
    class KTNormalizedFSDataPolar;
    //class KTSlidingWindowFSData;
    //class KTSlidingWindowFSDataFFTW;
    class KTWignerVilleData;


    /*!
     @class KTSpectrumDiscriminator
     @author N. S. Oblath

     @brief .

     @details
  
     Configuration name: "spectrum-discriminator"

     Available configuration values:
     - "snr-threshold": double -- snr threshold
     - "sigma-threshold": double -- sigma threshold
     - "min-frequency": double -- minimum frequency
     - "max-frequency": double -- maximum frequency
     - "min-bin": unsigned -- minimum frequency
     - "max-bin": unsigned -- maximum frequency

     Slots:
     - "fs-polar": void (shared_ptr< KTData >) -- Discriminates points above a threshold; Requires KTFrequencySpectrumDataPolar; Adds KTDiscrimiantedPoints1DData
     - "fs-fftw": void (shared_ptr< KTData >) -- Discriminates points above a threshold; Requires KTFrequencySpectrumDataFFTW; Adds KTDiscrimiantedPoints1DData
     - "norm-fs-polar": void (shared_ptr< KTData >) -- Discriminates points above a threshold; Requires KTFrequencySpectrumDataPolar; Adds KTDiscrimiantedPoints1DData
     - "norm-fs-fftw": void (shared_ptr< KTData >) -- Discriminates points above a threshold; Requires KTNormalizedFSDataFFTW; Adds KTDiscrimiantedPoints1DData
     - "corr": void (shared_ptr< KTData >) -- Discriminates points above a threshold; Requires KTCorrelationData; Adds KTDiscrimiantedPoints1DData
     - "wv": void (shared_ptr< KTData >) -- Discriminates points above a threshold; Requires KTWignerVilleData; Adds KTDistributedPoints1DData

     Signals:
     - "disc-1d": void (shared_ptr< KTData >) Emitted upon performance of a discrimination; Guarantees KTDiscriminatedPoints1DData-->
     <!--- "disc-2d": void (shared_ptr< KTData >) Emitted upon performance of a discrimination; Guarantees KTDiscriminatedPoints2DData-->
    */
    class KTSpectrumDiscriminator : public KTProcessor
    {
        private:
            enum ThresholdMode
            {
                eSNR_Amplitude,
                eSNR_Power,
                eSigma
            };

        public:
            KTSpectrumDiscriminator(const std::string& name = "spectrum-discriminator");
            virtual ~KTSpectrumDiscriminator();

            Bool_t Configure(const KTPStoreNode* node);

            Double_t GetSNRThreshold() const;
            void SetSNRAmplitudeThreshold(Double_t thresh);
            void SetSNRPowerThreshold(Double_t thresh);

            Double_t GetSigmaThreshold() const;
            void SetSigmaThreshold(Double_t thresh);

            Double_t GetMinFrequency() const;
            void SetMinFrequency(Double_t freq);

            Double_t GetMaxFrequency() const;
            void SetMaxFrequency(Double_t freq);

            UInt_t GetMinBin() const;
            void SetMinBin(UInt_t bin);

            UInt_t GetMaxBin() const;
            void SetMaxBin(UInt_t bin);

        private:

            Double_t fSNRThreshold;
            Double_t fSigmaThreshold;
            ThresholdMode fThresholdMode;

            Double_t fMinFrequency;
            Double_t fMaxFrequency;
            UInt_t fMinBin;
            UInt_t fMaxBin;
            Bool_t fCalculateMinBin;
            Bool_t fCalculateMaxBin;

        public:
            Bool_t Discriminate(KTFrequencySpectrumDataPolar& data);
            Bool_t Discriminate(KTFrequencySpectrumDataFFTW& data);
            Bool_t Discriminate(KTNormalizedFSDataPolar& data);
            Bool_t Discriminate(KTNormalizedFSDataFFTW& data);
            Bool_t Discriminate(KTCorrelationData& data);
            Bool_t Discriminate(KTWignerVilleData& data);
            //KTDiscriminatedPoints2DData* Discriminate(const KTSlidingWindowFSData* data);
            //KTDiscriminatedPoints2DData* Discriminate(const KTSlidingWindowFSDataFFTW* data);

        private:
            Bool_t CoreDiscriminate(KTFrequencySpectrumDataPolarCore& data, KTDiscriminatedPoints1DData& newData);
            Bool_t CoreDiscriminate(KTFrequencySpectrumDataFFTWCore& data, KTDiscriminatedPoints1DData& newData);


            //***************
            // Signals
            //***************

        private:
            KTSignalData fDiscrim1DSignal;
            //KTSignalData fDiscrim2DSignal;

            //***************
            // Slots
            //***************

        private:
            KTSlotDataOneType< KTFrequencySpectrumDataPolar > fFSPolarSlot;
            KTSlotDataOneType< KTFrequencySpectrumDataFFTW > fFSFFTWSlot;
            KTSlotDataOneType< KTNormalizedFSDataPolar > fNormFSPolarSlot;
            KTSlotDataOneType< KTNormalizedFSDataFFTW > fNormFSFFTWSlot;
            KTSlotDataOneType< KTCorrelationData > fCorrSlot;
            KTSlotDataOneType< KTWignerVilleData > fWVSlot;

    };

    inline Double_t KTSpectrumDiscriminator::GetSNRThreshold() const
    {
        return fSNRThreshold;
    }

    inline void KTSpectrumDiscriminator::SetSNRAmplitudeThreshold(Double_t thresh)
    {
        fSNRThreshold = thresh;
        fThresholdMode = eSNR_Amplitude;
        return;
    }

    inline void KTSpectrumDiscriminator::SetSNRPowerThreshold(Double_t thresh)
    {
        fSNRThreshold = thresh;
        fThresholdMode = eSNR_Power;
        return;
    }

    inline Double_t KTSpectrumDiscriminator::GetSigmaThreshold() const
    {
        return fSigmaThreshold;
    }

    inline void KTSpectrumDiscriminator::SetSigmaThreshold(Double_t thresh)
    {
        fSigmaThreshold = thresh;
        fThresholdMode = eSigma;
        return;
    }

    inline Double_t KTSpectrumDiscriminator::GetMinFrequency() const
    {
        return fMinFrequency;
    }

    inline void KTSpectrumDiscriminator::SetMinFrequency(Double_t freq)
    {
        fMinFrequency = freq;
        fCalculateMinBin = true;
        return;
    }

    inline Double_t KTSpectrumDiscriminator::GetMaxFrequency() const
    {
        return fMaxFrequency;
    }

    inline void KTSpectrumDiscriminator::SetMaxFrequency(Double_t freq)
    {
        fMaxFrequency = freq;
        fCalculateMaxBin = true;
        return;
    }

    inline UInt_t KTSpectrumDiscriminator::GetMinBin() const
    {
        return fMinBin;
    }

    inline void KTSpectrumDiscriminator::SetMinBin(UInt_t bin)
    {
        fMinBin = bin;
        fCalculateMinBin = false;
        return;
    }

    inline UInt_t KTSpectrumDiscriminator::GetMaxBin() const
    {
        return fMaxBin;
    }

    inline void KTSpectrumDiscriminator::SetMaxBin(UInt_t bin)
    {
        fMaxBin = bin;
        fCalculateMaxBin = false;
        return;
    }
} /* namespace Katydid */
#endif /* KTSPECTRUMDISCRIMINATOR_HH_ */
