/**
 @file KTVariableSpectrumDiscriminator.hh
 @brief Contains KTVariableSpectrumDiscriminator
 @details Discriminates Spectrum using gain variation data
 @author: N. S. Oblath
 @date: Dec 12, 2012
 */

#ifndef KTVARIABLESPECTRUMDISCRIMINATOR_HH_
#define KTVARIABLESPECTRUMDISCRIMINATOR_HH_

#include "KTProcessor.hh"

#include "KTSlot.hh"

#include <vector>


namespace Katydid
{
    class KTCorrelationData;
    class KTDiscriminatedPoints1DData;
    class KTFrequencySpectrumDataFFTW;
    class KTFrequencySpectrumDataFFTWCore;
    class KTFrequencySpectrumDataPolar;
    class KTFrequencySpectrumDataPolarCore;
    class KTFrequencySpectrumFFTW;
    class KTFrequencySpectrumPolar;
    class KTGainVariationData;
    class KTNormalizedFSDataFFTW;
    class KTNormalizedFSDataPolar;
    class KTSpline;
    class KTWignerVilleData;


    /*!
     @class KTVariableSpectrumDiscriminator
     @author N. S. Oblath

     @brief Selects peaky bins with a frequency-dependent threshold.

     @details
     The threshold used for a given bin is calculated based on the input gain variation data.
  
     Configuration name: "variable-spectrum-discriminator"

     Please note that the "threshold" values stored in the output KTDiscriminatedPoints1DData are not the actual threshold used,
     since the threshold is a function of frequency.  For SNR thresholds, the "threshold" is actually the value by which the
     gain variation is multiplied to find the threshold for a given bin.  For Sigma thresholds, the "threshold" is number added
     to the gain variation to find the threshold for a given bin.

     Available configuration values:
     - "snr-threshold-power": double -- Provide and use an SNR threshold, assuming the input data is in units of power
     - "snr-threshold-amplitude": double -- Provide and use an SNR threshold, assuming the input data is in units of amplitude
     - "sigma-threshold": double -- Provide and use a number-of-sigma threshold
     - "min-frequency": double -- minimum frequency
     - "max-frequency": double -- maximum frequency
     - "min-bin": unsigned -- minimum frequency by bin
     - "max-bin": unsigned -- maximum frequency by bin

     Slots:
     - "fs-polar": void (KTDataPtr) -- Discriminates points above a threshold; Requires KTFrequencySpectrumDataPolar; Adds KTDiscrimiantedPoints1DData
     - "fs-fftw": void (KTDataPtr) -- Discriminates points above a threshold; Requires KTFrequencySpectrumDataFFTW; Adds KTDiscrimiantedPoints1DData
     - "norm-fs-polar": void (KTDataPtr) -- Discriminates points above a threshold; Requires KTFrequencySpectrumDataPolar; Adds KTDiscrimiantedPoints1DData
     - "norm-fs-fftw": void (KTDataPtr) -- Discriminates points above a threshold; Requires KTNormalizedFSDataFFTW; Adds KTDiscrimiantedPoints1DData
     - "corr": void (KTDataPtr) -- Discriminates points above a threshold; Requires KTCorrelationData; Adds KTDiscrimiantedPoints1DData
     - "wv": void (KTDataPtr) -- Discriminates points above a threshold; Requires KTWignerVilleData; Adds KTDistributedPoints1DData

     Signals:
     - "disc-1d": void (KTDataPtr) Emitted upon performance of a discrimination; Guarantees KTDiscriminatedPoints1DData
    */
    class KTVariableSpectrumDiscriminator : public KTProcessor
    {
        private:
            enum ThresholdMode
            {
                eSNR_Amplitude,
                eSNR_Power,
                eSigma
            };

        public:
            KTVariableSpectrumDiscriminator(const std::string& name = "variable-spectrum-discriminator");
            virtual ~KTVariableSpectrumDiscriminator();

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
            Bool_t Discriminate(KTFrequencySpectrumDataPolar& data, KTGainVariationData& gvData);
            Bool_t Discriminate(KTFrequencySpectrumDataFFTW& data, KTGainVariationData& gvData);
            Bool_t Discriminate(KTNormalizedFSDataPolar& data, KTGainVariationData& gvData);
            Bool_t Discriminate(KTNormalizedFSDataFFTW& data, KTGainVariationData& gvData);
            Bool_t Discriminate(KTCorrelationData& data, KTGainVariationData& gvData);
            Bool_t Discriminate(KTWignerVilleData& data, KTGainVariationData& gvData);

            Bool_t DiscriminateSpectrum(const KTFrequencySpectrumPolar* spectrum, const KTSpline* spline, KTDiscriminatedPoints1DData&newData, UInt_t component=0);
            Bool_t DiscriminateSpectrum(const KTFrequencySpectrumFFTW* spectrum, const KTSpline* spline, KTDiscriminatedPoints1DData&newData, UInt_t component=0);

        private:
            Bool_t CoreDiscriminate(KTFrequencySpectrumDataPolarCore& data, KTGainVariationData& gvData, KTDiscriminatedPoints1DData& newData);
            Bool_t CoreDiscriminate(KTFrequencySpectrumDataFFTWCore& data, KTGainVariationData& gvData, KTDiscriminatedPoints1DData& newData);

            std::vector< Double_t > fMagnitudeCache;

            //***************
            // Signals
            //***************

        private:
            KTSignalData fDiscrim1DSignal;

            //***************
            // Slots
            //***************

        private:
            KTSlotDataTwoTypes< KTFrequencySpectrumDataPolar, KTGainVariationData > fFSPolarSlot;
            KTSlotDataTwoTypes< KTFrequencySpectrumDataFFTW, KTGainVariationData > fFSFFTWSlot;
            KTSlotDataTwoTypes< KTNormalizedFSDataPolar, KTGainVariationData > fNormFSPolarSlot;
            KTSlotDataTwoTypes< KTNormalizedFSDataFFTW, KTGainVariationData > fNormFSFFTWSlot;
            KTSlotDataTwoTypes< KTCorrelationData, KTGainVariationData > fCorrSlot;
            KTSlotDataTwoTypes< KTWignerVilleData, KTGainVariationData > fWVSlot;

    };

    inline Double_t KTVariableSpectrumDiscriminator::GetSNRThreshold() const
    {
        return fSNRThreshold;
    }

    inline void KTVariableSpectrumDiscriminator::SetSNRAmplitudeThreshold(Double_t thresh)
    {
        fSNRThreshold = thresh;
        fThresholdMode = eSNR_Amplitude;
        return;
    }

    inline void KTVariableSpectrumDiscriminator::SetSNRPowerThreshold(Double_t thresh)
    {
        fSNRThreshold = thresh;
        fThresholdMode = eSNR_Power;
        return;
    }

    inline Double_t KTVariableSpectrumDiscriminator::GetSigmaThreshold() const
    {
        return fSigmaThreshold;
    }

    inline void KTVariableSpectrumDiscriminator::SetSigmaThreshold(Double_t thresh)
    {
        fSigmaThreshold = thresh;
        fThresholdMode = eSigma;
        return;
    }

    inline Double_t KTVariableSpectrumDiscriminator::GetMinFrequency() const
    {
        return fMinFrequency;
    }

    inline void KTVariableSpectrumDiscriminator::SetMinFrequency(Double_t freq)
    {
        fMinFrequency = freq;
        fCalculateMinBin = true;
        return;
    }

    inline Double_t KTVariableSpectrumDiscriminator::GetMaxFrequency() const
    {
        return fMaxFrequency;
    }

    inline void KTVariableSpectrumDiscriminator::SetMaxFrequency(Double_t freq)
    {
        fMaxFrequency = freq;
        fCalculateMaxBin = true;
        return;
    }

    inline UInt_t KTVariableSpectrumDiscriminator::GetMinBin() const
    {
        return fMinBin;
    }

    inline void KTVariableSpectrumDiscriminator::SetMinBin(UInt_t bin)
    {
        fMinBin = bin;
        fCalculateMinBin = false;
        return;
    }

    inline UInt_t KTVariableSpectrumDiscriminator::GetMaxBin() const
    {
        return fMaxBin;
    }

    inline void KTVariableSpectrumDiscriminator::SetMaxBin(UInt_t bin)
    {
        fMaxBin = bin;
        fCalculateMaxBin = false;
        return;
    }
} /* namespace Katydid */
#endif /* KTVARIABLESPECTRUMDISCRIMINATOR_HH_ */
