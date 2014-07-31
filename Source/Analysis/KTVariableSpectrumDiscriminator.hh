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

     Abscissa values in the output data are the bin centers on the frequency axis.
  
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

            bool Configure(const KTParamNode* node);

            double GetSNRThreshold() const;
            void SetSNRAmplitudeThreshold(double thresh);
            void SetSNRPowerThreshold(double thresh);

            double GetSigmaThreshold() const;
            void SetSigmaThreshold(double thresh);

            double GetMinFrequency() const;
            void SetMinFrequency(double freq);

            double GetMaxFrequency() const;
            void SetMaxFrequency(double freq);

            unsigned GetMinBin() const;
            void SetMinBin(unsigned bin);

            unsigned GetMaxBin() const;
            void SetMaxBin(unsigned bin);

        private:

            double fSNRThreshold;
            double fSigmaThreshold;
            ThresholdMode fThresholdMode;

            double fMinFrequency;
            double fMaxFrequency;
            unsigned fMinBin;
            unsigned fMaxBin;
            bool fCalculateMinBin;
            bool fCalculateMaxBin;

        public:
            bool Discriminate(KTFrequencySpectrumDataPolar& data, KTGainVariationData& gvData);
            bool Discriminate(KTFrequencySpectrumDataFFTW& data, KTGainVariationData& gvData);
            bool Discriminate(KTNormalizedFSDataPolar& data, KTGainVariationData& gvData);
            bool Discriminate(KTNormalizedFSDataFFTW& data, KTGainVariationData& gvData);
            bool Discriminate(KTCorrelationData& data, KTGainVariationData& gvData);
            bool Discriminate(KTWignerVilleData& data, KTGainVariationData& gvData);

            bool DiscriminateSpectrum(const KTFrequencySpectrumPolar* spectrum, const KTSpline* spline, KTDiscriminatedPoints1DData&newData, unsigned component=0);
            bool DiscriminateSpectrum(const KTFrequencySpectrumFFTW* spectrum, const KTSpline* spline, KTDiscriminatedPoints1DData&newData, unsigned component=0);

        private:
            bool CoreDiscriminate(KTFrequencySpectrumDataPolarCore& data, KTGainVariationData& gvData, KTDiscriminatedPoints1DData& newData);
            bool CoreDiscriminate(KTFrequencySpectrumDataFFTWCore& data, KTGainVariationData& gvData, KTDiscriminatedPoints1DData& newData);

            std::vector< double > fMagnitudeCache;

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

    inline double KTVariableSpectrumDiscriminator::GetSNRThreshold() const
    {
        return fSNRThreshold;
    }

    inline void KTVariableSpectrumDiscriminator::SetSNRAmplitudeThreshold(double thresh)
    {
        fSNRThreshold = thresh;
        fThresholdMode = eSNR_Amplitude;
        return;
    }

    inline void KTVariableSpectrumDiscriminator::SetSNRPowerThreshold(double thresh)
    {
        fSNRThreshold = thresh;
        fThresholdMode = eSNR_Power;
        return;
    }

    inline double KTVariableSpectrumDiscriminator::GetSigmaThreshold() const
    {
        return fSigmaThreshold;
    }

    inline void KTVariableSpectrumDiscriminator::SetSigmaThreshold(double thresh)
    {
        fSigmaThreshold = thresh;
        fThresholdMode = eSigma;
        return;
    }

    inline double KTVariableSpectrumDiscriminator::GetMinFrequency() const
    {
        return fMinFrequency;
    }

    inline void KTVariableSpectrumDiscriminator::SetMinFrequency(double freq)
    {
        fMinFrequency = freq;
        fCalculateMinBin = true;
        return;
    }

    inline double KTVariableSpectrumDiscriminator::GetMaxFrequency() const
    {
        return fMaxFrequency;
    }

    inline void KTVariableSpectrumDiscriminator::SetMaxFrequency(double freq)
    {
        fMaxFrequency = freq;
        fCalculateMaxBin = true;
        return;
    }

    inline unsigned KTVariableSpectrumDiscriminator::GetMinBin() const
    {
        return fMinBin;
    }

    inline void KTVariableSpectrumDiscriminator::SetMinBin(unsigned bin)
    {
        fMinBin = bin;
        fCalculateMinBin = false;
        return;
    }

    inline unsigned KTVariableSpectrumDiscriminator::GetMaxBin() const
    {
        return fMaxBin;
    }

    inline void KTVariableSpectrumDiscriminator::SetMaxBin(unsigned bin)
    {
        fMaxBin = bin;
        fCalculateMaxBin = false;
        return;
    }
} /* namespace Katydid */
#endif /* KTVARIABLESPECTRUMDISCRIMINATOR_HH_ */
