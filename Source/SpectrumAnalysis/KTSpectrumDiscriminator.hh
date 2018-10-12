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


namespace Katydid
{
    
    class KTCorrelationData;
    class KTDiscriminatedPoints1DData;
    class KTFrequencySpectrumFFTW;
    class KTFrequencySpectrumDataFFTW;
    class KTFrequencySpectrumDataFFTWCore;
    class KTFrequencySpectrumPolar;
    class KTFrequencySpectrumDataPolar;
    class KTFrequencySpectrumDataPolarCore;
    class KTNormalizedFSDataFFTW;
    class KTNormalizedFSDataPolar;
    class KTNormalizedPSData;
    class KTPowerSpectrum;
    class KTPowerSpectrumData;
    class KTPowerSpectrumDataCore;
    class KTWignerVilleData;


    /*!
     @class KTSpectrumDiscriminator
     @author N. S. Oblath

     @brief Flat cut on spectrum data objects.

     @details
     The threshold can be specified as a power or amplitude SNR, or as a number of standard deviations (sigma).

     Abscissa values in the output are the bin centers of the frequency axis.
  
     Configuration name: "spectrum-discriminator"

     Available configuration values:
     - "snr-threshold-power": double -- Provide and use an SNR threshold, assuming the input data is in units of power
     - "snr-threshold-amplitude": double -- Provide and use an SNR threshold, assuming the input data is in units of amplitude
     - "sigma-threshold": double -- Provide and use a number-of-sigma threshold
     - "min-frequency": double -- minimum frequency
     - "max-frequency": double -- maximum frequency
     - "min-bin": unsigned -- minimum frequency by bin
     - "max-bin": unsigned -- maximum frequency by bin

     Slots:
     - "corr": void (Nymph::KTDataPtr) -- Discriminates points above a threshold; Requires KTCorrelationData; Adds KTDiscrimiantedPoints1DData
     - "fs-polar": void (Nymph::KTDataPtr) -- Discriminates points above a threshold; Requires KTFrequencySpectrumDataPolar; Adds KTDiscrimiantedPoints1DData
     - "fs-fftw": void (Nymph::KTDataPtr) -- Discriminates points above a threshold; Requires KTFrequencySpectrumDataFFTW; Adds KTDiscrimiantedPoints1DData
     - "norm-fs-polar": void (Nymph::KTDataPtr) -- Discriminates points above a threshold; Requires KTFrequencySpectrumDataPolar; Adds KTDiscrimiantedPoints1DData
     - "norm-fs-fftw": void (Nymph::KTDataPtr) -- Discriminates points above a threshold; Requires KTNormalizedFSDataFFTW; Adds KTDiscrimiantedPoints1DData
     - "norm-ps": void (Nymph::KTDataPtr) -- Discriminates points above a threshold; Requires KTNormalizedPSData; Adds KTDiscriminatedPoints1DData
     - "ps": void (Nymph::KTDataPtr) -- Discriminates points above a threshold; Requires KTPowerSpectrumData; Adds KTDiscriminatedPoints1DData
     - "wv": void (Nymph::KTDataPtr) -- Discriminates points above a threshold; Requires KTWignerVilleData; Adds KTDistributedPoints1DData

     Signals:
     - "disc-1d": void (Nymph::KTDataPtr) Emitted upon performance of a discrimination; Guarantees KTDiscriminatedPoints1DData-->
    */
    class KTSpectrumDiscriminator : public Nymph::KTProcessor
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

            bool Configure(const scarab::param_node* node);

            void SetSNRAmplitudeThreshold(double thresh);
            void SetSNRPowerThreshold(double thresh);
            void SetSigmaThreshold(double thresh);
            void SetMinFrequency(double freq);
            void SetMaxFrequency(double freq);
            void SetMinBin(unsigned bin);
            void SetMaxBin(unsigned bin);

        private:

            MEMBERVARIABLE_NOSET(double, SNRThreshold);
            MEMBERVARIABLE_NOSET(double, SigmaThreshold);
            MEMBERVARIABLE_NOSET(ThresholdMode, ThresholdMode);

            MEMBERVARIABLE_NOSET(double, MinFrequency);
            MEMBERVARIABLE_NOSET(double, MaxFrequency);
            MEMBERVARIABLE_NOSET(unsigned, MinBin);
            MEMBERVARIABLE_NOSET(unsigned, MaxBin);
            MEMBERVARIABLE(int, NeighborhoodRadius);
            MEMBERVARIABLE_NOSET(bool, CalculateMinBin);
            MEMBERVARIABLE_NOSET(bool, CalculateMaxBin);

        public:
            bool Discriminate(KTFrequencySpectrumDataPolar& data);
            bool Discriminate(KTFrequencySpectrumDataFFTW& data);
            bool Discriminate(KTPowerSpectrumData& data);
            bool Discriminate(KTNormalizedFSDataPolar& data);
            bool Discriminate(KTNormalizedFSDataFFTW& data);
            bool Discriminate(KTNormalizedPSData& data);
            bool Discriminate(KTCorrelationData& data);
            bool Discriminate(KTWignerVilleData& data);

        private:
            struct PerComponentInfo
            {
                double fMean;
                double fVariance;
            };

            bool CoreDiscriminate(KTFrequencySpectrumDataPolarCore& data, KTDiscriminatedPoints1DData& newData, std::vector< PerComponentInfo > pcData);
            bool CoreDiscriminate(KTFrequencySpectrumDataFFTWCore& data, KTDiscriminatedPoints1DData& newData, std::vector< PerComponentInfo > pcData);
            bool CoreDiscriminate(KTPowerSpectrumDataCore& data, KTDiscriminatedPoints1DData& newData, std::vector< PerComponentInfo > pcData);

            void SumAdjacentBinAmplitude(const KTPowerSpectrum* spectrum, double& neighborhoodAmplitude, const unsigned& iBin);
            void SumAdjacentBinAmplitude(const KTFrequencySpectrumFFTW* spectrum, double& neighborhoodAmplitude, const unsigned& iBin);
            void SumAdjacentBinAmplitude(const KTFrequencySpectrumPolar* spectrum, double& neighborhoodAmplitude, const unsigned& iBin);

            //***************
            // Signals
            //***************

        private:
            Nymph::KTSignalData fDiscrim1DSignal;

            //***************
            // Slots
            //***************

        private:
            Nymph::KTSlotDataOneType< KTFrequencySpectrumDataPolar > fFSPolarSlot;
            Nymph::KTSlotDataOneType< KTFrequencySpectrumDataFFTW > fFSFFTWSlot;
            Nymph::KTSlotDataOneType< KTNormalizedFSDataPolar > fNormFSPolarSlot;
            Nymph::KTSlotDataOneType< KTNormalizedFSDataFFTW > fNormFSFFTWSlot;
            Nymph::KTSlotDataOneType< KTNormalizedPSData > fNormPSSlot;
            Nymph::KTSlotDataOneType< KTPowerSpectrumData > fPSSlot;
            Nymph::KTSlotDataOneType< KTCorrelationData > fCorrSlot;
            Nymph::KTSlotDataOneType< KTWignerVilleData > fWVSlot;

    };


    inline void KTSpectrumDiscriminator::SetSNRAmplitudeThreshold(double thresh)
    {
        fSNRThreshold = thresh;
        fThresholdMode = eSNR_Amplitude;
        return;
    }

    inline void KTSpectrumDiscriminator::SetSNRPowerThreshold(double thresh)
    {
        fSNRThreshold = thresh;
        fThresholdMode = eSNR_Power;
        return;
    }

    inline void KTSpectrumDiscriminator::SetSigmaThreshold(double thresh)
    {
        fSigmaThreshold = thresh;
        fThresholdMode = eSigma;
        return;
    }

    inline void KTSpectrumDiscriminator::SetMinFrequency(double freq)
    {
        fMinFrequency = freq;
        fCalculateMinBin = true;
        return;
    }

    inline void KTSpectrumDiscriminator::SetMaxFrequency(double freq)
    {
        fMaxFrequency = freq;
        fCalculateMaxBin = true;
        return;
    }

    inline void KTSpectrumDiscriminator::SetMinBin(unsigned bin)
    {
        fMinBin = bin;
        fCalculateMinBin = false;
        return;
    }

    inline void KTSpectrumDiscriminator::SetMaxBin(unsigned bin)
    {
        fMaxBin = bin;
        fCalculateMaxBin = false;
        return;
    }
} /* namespace Katydid */
#endif /* KTSPECTRUMDISCRIMINATOR_HH_ */
