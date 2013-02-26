/**
 @file KTGainNormalization.hh
 @brief Contains KTGainNormalization
 @details Normalizes a frequency spectrum using a KTGainVariationData object.
 @author: N. S. Oblath
 @date: Jan 24, 2012
 */

#ifndef KTGAINNORMALIZATION_HH_
#define KTGAINNORMALIZATION_HH_

#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTFrequencySpectrumDataPolar.hh"
#include "KTProcessor.hh"

#include <boost/shared_ptr.hpp>

namespace Katydid
{

    class KTNormalizedFSDataPolar : public KTFrequencySpectrumDataPolarCore, public KTExtensibleData< KTNormalizedFSDataPolar >
    {
        public:
            KTNormalizedFSDataPolar()
            {}
            virtual ~KTNormalizedFSDataPolar()
            {}

            inline KTNormalizedFSDataPolar& SetNComponents(UInt_t components)
            {
                fSpectra.resize(components);
                return *this;
            }
    };

    class KTNormalizedFSDataFFTW : public KTFrequencySpectrumDataFFTWCore, public KTExtensibleData< KTNormalizedFSDataFFTW >
    {
        public:
            KTNormalizedFSDataFFTW()
            {}
            virtual ~KTNormalizedFSDataFFTW()
            {}

            inline KTNormalizedFSDataFFTW& SetNComponents(UInt_t components)
            {
                fSpectra.resize(components);
                return *this;
            }
    };



    class KTData;
    class KTFrequencySpectrumPolar;
    class KTFrequencySpectrumFFTW;
    class KTGainVariationData;
    class KTPStoreNode;
    class KTSlidingWindowFSData;
    class KTSlidingWindowFSDataFFTW;
    class KTSpline;
 

   /*!
     @class KTGainNormalization
     @author N. S. Oblath

     @brief Normalizes a frequency spectrum using a KTGainVariationData object.

     @details
 
     Available configuration values:
     \li \c "min-bin": unsigned -- Set the lower bound of the range that gets normalized by bin number.
     \li \c "max-bin": unsigned -- Set the upper bound of the range that gets nornalized by bin number.
     \li \c "min-frequency": double -- Set the lower bound of the range that gets normalized by frequency.
     \li \c "max-frequency": double -- Set the upper bound of the range that gets normalized by frequency.
     \li \c "gv-input-data-name": string -- name of the gain variation data to use when processing an event
     \li \c "fs-input-data-name": string -- name of the frequency spectrum data to use when processing an event
     \li \c "output-data-name": string -- name to give to the normalized frequency spectrum.

     Slots:
     \li \c "event": void ProcessEvent(boost::shared_ptr<KTEvent>)

     Signals:
     \li \c "gain-norm-fs": void (const KTFrequencySpectrumData*) emitted upon performance of a normalization of a KTFrequencySpectrumData object
     \li \c "gain-norm-fs-fftw": void (const KTFrequencySpectrumDataFFTW*) emitted upon performance normalization of a KTFrequencySpectrumDataFFTW object
     \li \c "gain-norm-sw-fs": void (const KTSlidingWindowFSData*) emitted upon performance of a normalization of a KTSlidingWindowFSData object
     \li \c "gain-norm-sw-fs-fftw": void (const KTSlidingWindowFSDataFFTW*) emitted upon performance of a normalization of a KTSlidingWindowFSDataFFTW object
    */

    class KTGainNormalization : public KTProcessor
    {
        public:
            typedef KTSignalConcept< void (boost::shared_ptr< KTData >) >::signal FSPolarSignal;
            typedef KTSignalConcept< void (boost::shared_ptr< KTData >) >::signal FSFFTWSignal;
            //typedef KTSignalConcept< void (const KTSlidingWindowFSData*) >::signal SWFSSignal;
            //typedef KTSignalConcept< void (const KTSlidingWindowFSDataFFTW*) >::signal SWFSFFTWSignal;

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

        private:
            Double_t fMinFrequency;
            Double_t fMaxFrequency;
            UInt_t fMinBin;
            UInt_t fMaxBin;
            Bool_t fCalculateMinBin;
            Bool_t fCalculateMaxBin;

        public:
            Bool_t Normalize(KTFrequencySpectrumDataPolar& fsData, const KTGainVariationData& gvData);
            Bool_t Normalize(KTFrequencySpectrumDataFFTW& fsData, const KTGainVariationData& gvData);

            //void Normalize(KTSlidingWindowFSData* swFSData, const KTGainVariationData* gvData);
            //void Normalize(KTSlidingWindowFSDataFFTW* swFSData, const KTGainVariationData* gvData);

            KTFrequencySpectrumPolar* Normalize(const KTFrequencySpectrumPolar* frequencySpectrum, const KTSpline* spline);
            KTFrequencySpectrumFFTW* Normalize(const KTFrequencySpectrumFFTW* frequencySpectrum, const KTSpline* spline);

        private:

            //***************
            // Signals
            //***************

        private:
            FSPolarSignal fFSPolarSignal;
            FSFFTWSignal fFSFFTWSignal;
            //SWFSSignal fSWFSSignal;
            //SWFSFFTWSignal fSWFSFFTWSignal;

            //***************
            // Slots
            //***************

        public:
            void ProcessFSPolarData(boost::shared_ptr< KTData > data);
            void ProcessFSFFTWData(boost::shared_ptr< KTData > data);

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
