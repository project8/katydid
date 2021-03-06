/**
 @file KTVariableSpectrumDiscriminator.hh
 @brief Contains KTVariableSpectrumDiscriminator
 @details Discriminates Spectrum using gain variation data
 @author: N. S. Oblath
 @date: Dec 12, 2012
 */

#ifndef KTVARIABLESPECTRUMDISCRIMINATOR_HH_
#define KTVARIABLESPECTRUMDISCRIMINATOR_HH_

#include "KTGainVariationData.hh"
#include "KTProcessor.hh"

#include "KTSlot.hh"

#include <vector>


namespace Katydid
{
    
    class KTConvolvedPowerSpectrumData;
    class KTConvolvedFrequencySpectrumDataFFTW;
    class KTConvolvedFrequencySpectrumDataPolar;
    class KTCorrelationData;
    class KTDiscriminatedPoints1DData;
    class KTDiscriminatedPoints2DData;
    class KTFrequencySpectrumDataFFTW;
    class KTFrequencySpectrumDataFFTWCore;
    class KTFrequencySpectrumDataPolar;
    class KTFrequencySpectrumDataPolarCore;
    class KTFrequencySpectrumFFTW;
    class KTFrequencySpectrumPolar;
    class KTNormalizedFSDataFFTW;
    class KTNormalizedFSDataPolar;
    class KTPowerSpectrum;
    class KTPowerSpectrumData;
    class KTPowerSpectrumDataCore;
    class KTPSCollectionData;
    class KTSpline;
    class KTWignerVilleData;


    /*!
     @class KTVariableSpectrumDiscriminator
     @author N. S. Oblath

     @brief Selects peaky bins with a frequency-dependent threshold.

     @details
     The threshold used for a given bin is calculated based on the input gain variation data.
     This processor can be used in two modes (simultaneously, if desired):
     1. Input data contains both the values to be thresholded, plus the gain variation data to use to form the threshold;
     2. Gain variation data is received ahead of time and applied to all input data.
     For (1), use the Discriminate functions with two arguments (slots without "-pre").
     For (2), set the gain variation data with SetPreCalcGainVar (slot "gv"), and the Discriminate functions with one argument (slots with "-pre").

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
     - "normalize": bool -- Scale mean and variance in sigma-threshold mode

     Slots:
     - "fs-polar": void (Nymph::KTDataPtr) -- Discriminates points above a threshold; Requires KTFrequencySpectrumDataPolar and KTGainVariationData; Adds KTDiscrimiantedPoints1DData
     - "fs-fftw": void (Nymph::KTDataPtr) -- Discriminates points above a threshold; Requires KTFrequencySpectrumDataFFTW and KTGainVariationData; Adds KTDiscrimiantedPoints1DData
     - "conv-ps": void (Nymph::KTDataPtr) -- Discriminates points above a threshold; Requires KTConvolvedPowerSpectrumData and KTGainVariationData; Adds KTDiscriminatedPoints1DData
     - "norm-fs-polar": void (Nymph::KTDataPtr) -- Discriminates points above a threshold; Requires KTFrequencySpectrumDataPolar and KTGainVariationData; Adds KTDiscrimiantedPoints1DData
     - "norm-fs-fftw": void (Nymph::KTDataPtr) -- Discriminates points above a threshold; Requires KTNormalizedFSDataFFTW and KTGainVariationData; Adds KTDiscrimiantedPoints1DData
     - "corr": void (Nymph::KTDataPtr) -- Discriminates points above a threshold; Requires KTCorrelationData and KTGainVariationData; Adds KTDiscrimiantedPoints1DData
     - "wv": void (Nymph::KTDataPtr) -- Discriminates points above a threshold; Requires KTWignerVilleData and KTGainVariationData; Adds KTDiscriminatedPoints1DData
     - "ps": void (Nymph::KTDataPtr) -- Discriminates points above a threshold; Requires KTPowerSpectrumData and KTGainVariationData; Adds KTDiscriminatedPoints1DData
     - "gv": void (Nymph::KTDataPtr) -- Sets the pre-calculated gain-variation data; Requires KTGainVariationData
     - "conv-ps-pre": void (Nymph::KTDataPtr) -- Discriminates points above the pre-calculated threshold; Requires KTConvolvedPowerSpectrumData; Adds DiscriminatedPoints1DData
     - "ps-pre": void (Nymph::KTDataPtr) -- Discriminates points above the pre-calculated threshold; Requires KTPowerSpectrumData; Adds DiscriminatedPoints1DData
     - "spec": void (Nymph::KTDataPtr) -- Discriminates points above a threshold; Requires KTPSCollectionData and KTGainVariationData; Adds KTDiscriminatedPoints2DData
     - "spec-pre": void (Nymph::KTDataPtr) -- Discriminates points above a pre-calculated threshold; Requires KTPSCollectionData; Adds KTDiscriminatedPoints2DData

     Signals:
     - "disc-1d": void (Nymph::KTDataPtr) Emitted upon performance of a discrimination; Guarantees KTDiscriminatedPoints1DData
     - "disc-2d": void (Nymph::KTDataPtr) Emitted upon performance of a discrimination; Guarantees KTDiscriminatedPoints2DData
    */
    class KTVariableSpectrumDiscriminator : public Nymph::KTProcessor
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
            MEMBERVARIABLE_NOSET(bool, CalculateMinBin);
            MEMBERVARIABLE_NOSET(bool, CalculateMaxBin);

            MEMBERVARIABLE(bool, Normalize);
            MEMBERVARIABLE(int, NeighborhoodRadius);

        public:
            bool CheckGVData();
            bool SetPreCalcGainVar(KTGainVariationData& gvData);

            bool Discriminate(KTConvolvedPowerSpectrumData& data);
            bool Discriminate(KTPowerSpectrumData& data);
            bool Discriminate(KTPSCollectionData& data);

            bool Discriminate(KTFrequencySpectrumDataPolar& data, KTGainVariationData& gvData);
            bool Discriminate(KTFrequencySpectrumDataFFTW& data, KTGainVariationData& gvData);
            bool Discriminate(KTConvolvedPowerSpectrumData& data, KTGainVariationData& gvData);
            bool Discriminate(KTNormalizedFSDataPolar& data, KTGainVariationData& gvData);
            bool Discriminate(KTNormalizedFSDataFFTW& data, KTGainVariationData& gvData);
            bool Discriminate(KTCorrelationData& data, KTGainVariationData& gvData);
            bool Discriminate(KTWignerVilleData& data, KTGainVariationData& gvData);
            bool Discriminate(KTPowerSpectrumData& data, KTGainVariationData& gvData);
            bool Discriminate(KTPSCollectionData& data, KTGainVariationData& gvData);

            bool DiscriminateSpectrum(const KTFrequencySpectrumPolar* spectrum, const KTSpline* spline, const KTSpline* varSpline, KTDiscriminatedPoints1DData& newData, unsigned component=0);
            bool DiscriminateSpectrum(const KTFrequencySpectrumFFTW* spectrum, const KTSpline* spline, const KTSpline* varSpline, KTDiscriminatedPoints1DData& newData, unsigned component=0);
            bool DiscriminateSpectrum(const KTPowerSpectrum* spectrum, const KTSpline* spline, const KTSpline* varSpline, KTDiscriminatedPoints1DData& newData, unsigned component=0);

        private:
            bool CoreDiscriminate(KTFrequencySpectrumDataPolarCore& data, KTGainVariationData& gvData, KTDiscriminatedPoints1DData& newData);
            bool CoreDiscriminate(KTFrequencySpectrumDataFFTWCore& data, KTGainVariationData& gvData, KTDiscriminatedPoints1DData& newData);
            bool CoreDiscriminate(KTPowerSpectrumDataCore& data, KTGainVariationData& gvData, KTDiscriminatedPoints1DData& newData);

            void SumAdjacentBinAmplitude(const KTPowerSpectrum* spectrum, double& neighborhoodAmplitude, const unsigned& iBin);
            void SumAdjacentBinAmplitude(const KTFrequencySpectrumFFTW* spectrum, double& neighborhoodAmplitude, const unsigned& iBin);
            void SumAdjacentBinAmplitude(const KTFrequencySpectrumPolar* spectrum, double& neighborhoodAmplitude, const unsigned& iBin);


            KTGainVariationData fGVData;
            std::vector< double > fMagnitudeCache;

            //***************
            // Signals
            //***************

        private:
            Nymph::KTSignalData fDiscrim1DSignal;
            Nymph::KTSignalData fDiscrim2DSignal;

            //***************
            // Slots
            //***************

        private:
            Nymph::KTSlotDataTwoTypes< KTFrequencySpectrumDataPolar, KTGainVariationData > fFSPolarSlot;
            Nymph::KTSlotDataTwoTypes< KTFrequencySpectrumDataFFTW, KTGainVariationData > fFSFFTWSlot;
            Nymph::KTSlotDataTwoTypes< KTConvolvedPowerSpectrumData, KTGainVariationData > fConvPSSlot;
            Nymph::KTSlotDataTwoTypes< KTNormalizedFSDataPolar, KTGainVariationData > fNormFSPolarSlot;
            Nymph::KTSlotDataTwoTypes< KTNormalizedFSDataFFTW, KTGainVariationData > fNormFSFFTWSlot;
            Nymph::KTSlotDataTwoTypes< KTCorrelationData, KTGainVariationData > fCorrSlot;
            Nymph::KTSlotDataTwoTypes< KTWignerVilleData, KTGainVariationData > fWVSlot;
            Nymph::KTSlotDataTwoTypes< KTPowerSpectrumData, KTGainVariationData > fPSSlot;
            Nymph::KTSlotDataTwoTypes< KTPSCollectionData, KTGainVariationData > fSpecSlot;

            Nymph::KTSlotDataOneType< KTGainVariationData > fPreCalcSlot;

            Nymph::KTSlotDataOneType< KTConvolvedPowerSpectrumData > fConvPSPreCalcSlot;
            Nymph::KTSlotDataOneType< KTPowerSpectrumData > fPSPreCalcSlot;

            Nymph::KTSlotDataOneType< KTPSCollectionData > fSpecPreCalcSlot;

    };


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

    inline void KTVariableSpectrumDiscriminator::SetSigmaThreshold(double thresh)
    {
        fSigmaThreshold = thresh;
        fThresholdMode = eSigma;
        return;
    }

    inline void KTVariableSpectrumDiscriminator::SetMinFrequency(double freq)
    {
        fMinFrequency = freq;
        fCalculateMinBin = true;
        return;
    }

    inline void KTVariableSpectrumDiscriminator::SetMaxFrequency(double freq)
    {
        fMaxFrequency = freq;
        fCalculateMaxBin = true;
        return;
    }

    inline void KTVariableSpectrumDiscriminator::SetMinBin(unsigned bin)
    {
        fMinBin = bin;
        fCalculateMinBin = false;
        return;
    }

    inline void KTVariableSpectrumDiscriminator::SetMaxBin(unsigned bin)
    {
        fMaxBin = bin;
        fCalculateMaxBin = false;
        return;
    }


} /* namespace Katydid */
#endif /* KTVARIABLESPECTRUMDISCRIMINATOR_HH_ */
