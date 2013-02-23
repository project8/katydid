/*
 * KTSpectrumDiscriminator.hh
 *
 *  Created on: Dec 12, 2012
 *      Author: nsoblath
 */

#ifndef KTSPECTRUMDISCRIMINATOR_HH_
#define KTSPECTRUMDISCRIMINATOR_HH_

#include "KTProcessor.hh"

#include <boost/shared_ptr.hpp>


namespace Katydid
{
    class KTCorrelationData;
    class KTData;
    class KTFrequencySpectrumDataPolar;
    class KTFrequencySpectrumDataFFTW;
    class KTNormalizedFSDataPolar;
    class KTNormalizedFSDataFFTW;
    //class KTSlidingWindowFSData;
    //class KTSlidingWindowFSDataFFTW;

    class KTSpectrumDiscriminator : public KTProcessor
    {
        public:
            typedef KTSignal< void (boost::shared_ptr< KTData >) >::signal Discrim1DSignal;
            //typedef KTSignal< void (boost::shared_ptr< KTData >) >::signal Discrim2DSignal;

        private:
            enum ThresholdMode
            {
                eSNR,
                eSigma
            };

        public:
            KTSpectrumDiscriminator();
            virtual ~KTSpectrumDiscriminator();

            Bool_t Configure(const KTPStoreNode* node);

            Double_t GetSNRThreshold() const;
            void SetSNRThreshold(Double_t thresh);

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
            //KTDiscriminatedPoints2DData* Discriminate(const KTSlidingWindowFSData* data);
            //KTDiscriminatedPoints2DData* Discriminate(const KTSlidingWindowFSDataFFTW* data);

        private:
            Bool_t CoreDiscriminate(KTFrequencySpectrumDataPolarCore& data, KTDiscriminatedPoints1DData& newData);
            Bool_t CoreDiscriminate(KTFrequencySpectrumDataFFTWCore& data, KTDiscriminatedPoints1DData& newData);


            //***************
            // Signals
            //***************

        private:
            Discrim1DSignal fDiscrim1DSignal;
            //Discrim2DSignal fDiscrim2DSignal;

            //***************
            // Slots
            //***************

        public:
            void ProcessFrequencySpectrumDataPolar(boost::shared_ptr< KTData > data);
            void ProcessFrequencySpectrumDataFFTW(boost::shared_ptr< KTData > data);
            void ProcessNormalizedFSDataPolar(boost::shared_ptr< KTData > data);
            void ProcessNormalizedFSDataFFTW(boost::shared_ptr< KTData > data);
            void ProcessCorrelationData(boost::shared_ptr< KTData > data);
            //void ProcessSlidingWindowFSData(const KTSlidingWindowFSData* data);
            //void ProcessSlidingWindowFSDataFFTW(const KTSlidingWindowFSDataFFTW* data);

    };

    inline Double_t KTSpectrumDiscriminator::GetSNRThreshold() const
    {
        return fSNRThreshold;
    }

    inline void KTSpectrumDiscriminator::SetSNRThreshold(Double_t thresh)
    {
        fSNRThreshold = thresh;
        fThresholdMode = eSNR;
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
