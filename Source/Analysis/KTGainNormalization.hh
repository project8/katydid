/**
 @file KTGainNormalization.hh
 @brief Contains KTGainNormalization
 @details Removes slow variations in the baseline of a histogram
 @author: N. S. Oblath
 @date: Jan 24, 2012
 */

#ifndef KTGAINNORMALIZATION_HH_
#define KTGAINNORMALIZATION_HH_

#include "KTProcessor.hh"

#include <boost/shared_ptr.hpp>

namespace Katydid
{
    class KTEvent;
    class KTFrequencySpectrum;
    class KTFrequencySpectrumData;
    class KTFrequencySpectrumDataFFTW;
    class KTFrequencySpectrumFFTW;
    class KTGainVariationData;
    class KTPStoreNode;
    class KTSlidingWindowFSData;
    class KTSlidingWindowFSDataFFTW;
    class KTSpline;

    class KTGainNormalization : public KTProcessor
    {
        public:
            typedef KTSignal< void (const KTFrequencySpectrumData*) >::signal FSSignal;
            typedef KTSignal< void (const KTFrequencySpectrumDataFFTW*) >::signal FSFFTWSignal;
            typedef KTSignal< void (const KTSlidingWindowFSData*) >::signal SWFSSignal;
            typedef KTSignal< void (const KTSlidingWindowFSDataFFTW*) >::signal SWFSFFTWSignal;

        public:
            KTGainNormalization();
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

            const std::string& GetGVInputDataName() const;
            void SetGVInputDataName(const std::string& name);

            const std::string& GetFSInputDataName() const;
            void SetFSInputDataName(const std::string& name);

            const std::string& GetOutputDataName() const;
            void SetOutputDataName(const std::string& name);

        private:
            Double_t fMinFrequency;
            Double_t fMaxFrequency;
            UInt_t fMinBin;
            UInt_t fMaxBin;
            Bool_t fCalculateMinBin;
            Bool_t fCalculateMaxBin;

            std::string fGVInputDataName;
            std::string fFSInputDataName;
            std::string fOutputDataName;

        public:
            KTFrequencySpectrumData* Normalize(const KTFrequencySpectrumData* fsData, const KTGainVariationData* gvData);
            KTFrequencySpectrumDataFFTW* Normalize(const KTFrequencySpectrumDataFFTW* fsData, const KTGainVariationData* gvData);

            //void Normalize(KTSlidingWindowFSData* swFSData, const KTGainVariationData* gvData);
            //void Normalize(KTSlidingWindowFSDataFFTW* swFSData, const KTGainVariationData* gvData);

            KTFrequencySpectrum* Normalize(const KTFrequencySpectrum* frequencySpectrum, const KTSpline* spline);
            KTFrequencySpectrumFFTW* Normalize(const KTFrequencySpectrumFFTW* frequencySpectrum, const KTSpline* spline);

        private:

            //***************
            // Signals
            //***************

        private:
            FSSignal fFSSignal;
            FSFFTWSignal fFSFFTWSignal;
            SWFSSignal fSWFSSignal;
            SWFSFFTWSignal fSWFSFFTWSignal;

            //***************
            // Slots
            //***************

        public:
            void ProcessEvent(boost::shared_ptr<KTEvent> event);

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

    inline const std::string& KTGainNormalization::GetGVInputDataName() const
    {
        return fGVInputDataName;
    }

    inline void KTGainNormalization::SetGVInputDataName(const std::string& name)
    {
        fGVInputDataName = name;
        return;
    }

    inline const std::string& KTGainNormalization::GetFSInputDataName() const
    {
        return fFSInputDataName;
    }

    inline void KTGainNormalization::SetFSInputDataName(const std::string& name)
    {
        fFSInputDataName = name;
        return;
    }

    inline const std::string& KTGainNormalization::GetOutputDataName() const
    {
        return fOutputDataName;
    }

    inline void KTGainNormalization::SetOutputDataName(const std::string& name)
    {
        fOutputDataName = name;
        return;
    }


} /* namespace Katydid */
#endif /* KTGAINNORMALIZATION_HH_ */
