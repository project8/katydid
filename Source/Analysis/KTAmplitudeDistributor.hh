/*
 * KTAmplitudeDistributor.hh
 *
 *  Created on: Apr 26, 2013
 *      Author: nsoblath
 */

#ifndef KTAMPLITUDEDISTRIBUTOR_HH_
#define KTAMPLITUDEDISTRIBUTOR_HH_

#include "KTProcessor.hh"

#include "KTAmplitudeDistribution.hh"
#include "KTSlot.hh"

#include <boost/shared_ptr.hpp>

#include <vector>

namespace Katydid
{
    class KTCorrelationData;
    class KTData;
    class KTDiscriminatedPoints1DData;
    class KTEggHeader;
    class KTFrequencySpectrumDataFFTW;
    class KTFrequencySpectrumDataFFTWCore;
    class KTFrequencySpectrumDataPolar;
    class KTFrequencySpectrumDataPolarCore;
    class KTFrequencySpectrumFFTW;
    class KTFrequencySpectrumPolar;
    class KTNormalizedFSDataFFTW;
    class KTNormalizedFSDataPolar;
    class KTWignerVilleData;


    /*!
     @class KTAmplitudeDistributor
     @author N. S. Oblath

     @brief .

     @details


     Available configuration values:
     - "min-frequency": double -- minimum frequency
     - "max-frequency": double -- maximum frequency
     - "min-bin": unsigned -- minimum frequency
     - "max-bin": unsigned -- maximum frequency
     - "dist-n-bins": unsigned -- number of bins for the distribution histogram
     - "use-buffer": bool -- whether to use the buffer (true; default) or pre-specified distribution min and max (false)
     - "buffer-size": unsigned -- number of spectra to store initially to determine the range of the distribution (if not using "dist-min/max")
     - "dist-min": double -- minimum of the distribution (if not using "buffer-size")
     - "dist-max": double -- maximum of the distribution (if not using "buffer-size")

     Slots:
     - Running
       - "fs-polar": void (shared_ptr< KTData >) -- Adds values from a spectrum to the amplitude distribution; Requires KTFrequencySpectrumDataPolar
       - "fs-fftw": void (shared_ptr< KTData >) -- Adds values from a spectrum to the amplitude distribution; Requires KTFrequencySpectrumDataFFTW
       - "norm-fs-polar": void (shared_ptr< KTData >) -- Adds values from a spectrum to the amplitude distribution; Requires KTFrequencySpectrumDataPolar
       - "norm-fs-fftw": void (shared_ptr< KTData >) -- Adds values from a spectrum to the amplitude distribution; Requires KTNormalizedFSDataFFTW
       - "corr": void (shared_ptr< KTData >) -- Adds values from a spectrum to the amplitude distribution; Requires KTCorrelationData
       - "wv": void (shared_ptr< KTData >) -- Adds values from a spectrum to the amplitude distribution; Requires KTWignerVilleData
     - Completion
       - "finish": void () -- Completes the calculation of the amplitude distribution; Emits "amp-dist"

     Signals:
     \li \c "amp-dist": void (shared_ptr< KTData >) Emitted upon completion of an amplitude distribution; Guarantees KTAmplitudeDistributor
    */

    class KTAmplitudeDistributor : public KTProcessor
    {
        public:
            typedef std::vector< Double_t > Spectrum; // indexed over frequency-axis bins
            typedef std::vector< Spectrum > Spectra; // indexed over component
            typedef std::vector< Spectra > Buffer; // indexed over slice number

        public:
            KTAmplitudeDistributor(const std::string& name = "amplitude-distributor");
            virtual ~KTAmplitudeDistributor();

            Bool_t Configure(const KTPStoreNode* node);

            Double_t GetMinFrequency() const;
            void SetMinFrequency(Double_t freq);

            Double_t GetMaxFrequency() const;
            void SetMaxFrequency(Double_t freq);

            UInt_t GetMinBin() const;
            void SetMinBin(UInt_t bin);

            UInt_t GetMaxBin() const;
            void SetMaxBin(UInt_t bin);

            UInt_t GetDistNBins() const;
            void SetDistNBins(UInt_t nBins);

            Bool_t GetUseBuffer() const;
            void SetUseBuffer(Bool_t useBuffer);

            UInt_t GetBufferSize() const;
            void SetBufferSize(UInt_t buffer);

            Double_t GetDistMin() const;
            void SetDistMin(Double_t min);

            Double_t GetDistMax() const;
            void SetDistMax(Double_t max);

        private:
            Double_t fMinFrequency;
            Double_t fMaxFrequency;
            UInt_t fMinBin;
            UInt_t fMaxBin;
            Bool_t fCalculateMinBin;
            Bool_t fCalculateMaxBin;

            UInt_t fDistNBins;
            UInt_t fBufferSize;
            Double_t fDistMin;
            Double_t fDistMax;
            Bool_t fUseBuffer;

        public:
            Bool_t Initialize(UInt_t nComponents, UInt_t nFreqBins);

            Bool_t AddValues(KTFrequencySpectrumDataPolar& data);
            Bool_t AddValues(KTFrequencySpectrumDataFFTW& data);
            Bool_t AddValues(KTNormalizedFSDataPolar& data);
            Bool_t AddValues(KTNormalizedFSDataFFTW& data);
            Bool_t AddValues(KTCorrelationData& data);
            Bool_t AddValues(KTWignerVilleData& data);

            void FinishAmpDist();

        private:
            Bool_t CoreAddValues(KTFrequencySpectrumDataPolarCore& data);
            Bool_t CoreAddValues(KTFrequencySpectrumDataFFTWCore& data);

            void (KTAmplitudeDistributor::*fTakeValuesPolar)(const KTFrequencySpectrumPolar*, UInt_t);
            void TakeValuesToBuffer(const KTFrequencySpectrumPolar* spectrum, UInt_t component);
            void TakeValuesToDistributions(const KTFrequencySpectrumPolar* spectrum, UInt_t component);

            void (KTAmplitudeDistributor::*fTakeValuesFFTW)(const KTFrequencySpectrumFFTW*, UInt_t);
            void TakeValuesToBuffer(const KTFrequencySpectrumFFTW* spectrum, UInt_t component);
            void TakeValuesToDistributions(const KTFrequencySpectrumFFTW* spectrum, UInt_t component);

            Bool_t CreateDistributionsEmpty();
            Bool_t CreateDistributionsFromBuffer();

            Double_t fInvDistBinWidth;

            UInt_t fNFreqBins;
            UInt_t fNComponents;

            Buffer fBuffer;
            UInt_t fNBuffered;

            boost::shared_ptr< KTData > fDistributionData;
            KTAmplitudeDistribution* fDistributions;


            //***************
            // Signals
            //***************

        private:
            KTSignalData fAmpDistSignal;

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

            KTSlotNoArg< void () > fCompleteDistributions;

    };

    inline Double_t KTAmplitudeDistributor::GetMinFrequency() const
    {
        return fMinFrequency;
    }

    inline void KTAmplitudeDistributor::SetMinFrequency(Double_t freq)
    {
        fMinFrequency = freq;
        fCalculateMinBin = true;
        return;
    }

    inline Double_t KTAmplitudeDistributor::GetMaxFrequency() const
    {
        return fMaxFrequency;
    }

    inline void KTAmplitudeDistributor::SetMaxFrequency(Double_t freq)
    {
        fMaxFrequency = freq;
        fCalculateMaxBin = true;
        return;
    }

    inline UInt_t KTAmplitudeDistributor::GetMinBin() const
    {
        return fMinBin;
    }

    inline void KTAmplitudeDistributor::SetMinBin(UInt_t bin)
    {
        fMinBin = bin;
        fCalculateMinBin = false;
        return;
    }

    inline UInt_t KTAmplitudeDistributor::GetMaxBin() const
    {
        return fMaxBin;
    }

    inline void KTAmplitudeDistributor::SetMaxBin(UInt_t bin)
    {
        fMaxBin = bin;
        fCalculateMaxBin = false;
        return;
    }

    inline UInt_t KTAmplitudeDistributor::GetDistNBins() const
    {
        return fDistNBins;
    }

    inline void KTAmplitudeDistributor::SetDistNBins(UInt_t nBins)
    {
        fDistNBins = nBins;
        return;
    }

    inline Bool_t KTAmplitudeDistributor::GetUseBuffer() const
    {
        return fUseBuffer;
    }

    inline void KTAmplitudeDistributor::SetUseBuffer(Bool_t useBuffer)
    {
        fUseBuffer = useBuffer;
        return;
    }

    inline UInt_t KTAmplitudeDistributor::GetBufferSize() const
    {
        return fBufferSize;
    }

    inline void KTAmplitudeDistributor::SetBufferSize(UInt_t buffer)
    {
        fBufferSize = buffer;
        return;
    }

    inline Double_t KTAmplitudeDistributor::GetDistMin() const
    {
        return fDistMin;
    }

    inline void KTAmplitudeDistributor::SetDistMin(Double_t min)
    {
        fDistMin = min;
        fInvDistBinWidth = Double_t (fDistNBins) / (fDistMax - fDistMin);
        return;
    }

    inline Double_t KTAmplitudeDistributor::GetDistMax() const
    {
        return fDistMax;
    }

    inline void KTAmplitudeDistributor::SetDistMax(Double_t max)
    {
        fDistMax = max;
        fInvDistBinWidth = Double_t (fDistNBins) / (fDistMax - fDistMin);
        return;
    }

} /* namespace Katydid */
#endif /* KTAMPLITUDEDISTRIBUTOR_HH_ */
