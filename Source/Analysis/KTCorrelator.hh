/**
 @file KTCorrelator.hh
 @brief Contains KTCorrelator
 @details Correlates frequency spectra from different channels
 @author: N. S. Oblath
 @date: Aug  20, 2012
 */

#ifndef KTCORRELATOR_HH_
#define KTCORRELATOR_HH_

#include "KTFrequencySpectrumDataPolar.hh"
#include "KTPStoreNode.hh"
#include "KTProcessor.hh"

#include "KTSlot.hh"

#include <boost/shared_ptr.hpp>

#include <utility>
#include <vector>

namespace Katydid
{
    class KTCorrelationData;
    class KTFrequencySpectrumDataFFTW;
    class KTFrequencySpectrumDataFFTWCore;
    class KTFrequencySpectrumFFTW;
    class KTNormalizedFSDataPolar;
    class KTNormalizedFSDataFFTW;

    /*!
     @class KTCorrelator
     @author N. S. Oblath

     @brief Correlates frequencey spectra from different channels.

     @details
     
     Available configuration values:
     \li \c "corr-pair": string -- channel pair to be correlated: "[first channel], [second channel]"; e.g. "0, 0" or "0, 1"

      Slots:
     \li \c "fs-polar": void (shared_ptr< KTData >) -- Performs correlations between frequency spectrum components; Requires KTFrequencySpectrumDataPolar; Adds KTCorrelationData
     \li \c "fs-fftw": void (shared_ptr< KTData >) -- Performs correlations between frequency spectrum components; Requires KTFrequencySpectrumDataFFTW; Adds KTCorrelationData
     \li \c "norm-fs-polar": void (shared_ptr< KTData >) -- Performs correlations between frequency spectrum components; Requires KTFSNormalizedDataPolar; Adds KTCorrelationData
     \li \c "norm-fs-fftw": void (shared_ptr< KTData >) -- Performs correlations between frequency spectrum components; Requires KTFSNormalizedDataFFTW; Adds KTCorrelationData

     Signals:
     \li \c "correlation": void (shared_ptr< KTData >) -- Emitted upon performance of a correlation; Guarantees KTCorrelationData
    */
    class KTCorrelator : public KTProcessor
    {
        protected:
            typedef std::vector< UIntPair > PairVector;

        public:
            KTCorrelator(const std::string& name = "correlator");
            virtual ~KTCorrelator();

            Bool_t Configure(const KTPStoreNode* node);

            void AddPair(const UIntPair& pair);
            void SetPairVector(const PairVector& pairs);
            const PairVector& GetPairVector() const;
            void ClearPairs();

        protected:
            PairVector fPairs;

        public:

            Bool_t Correlate(KTFrequencySpectrumDataPolar& data);
            Bool_t Correlate(KTFrequencySpectrumDataFFTW& data);
            Bool_t Correlate(KTNormalizedFSDataPolar& data);
            Bool_t Correlate(KTNormalizedFSDataFFTW& data);

        protected:
            Bool_t CoreCorrelate(KTFrequencySpectrumDataPolarCore& data, KTCorrelationData& newData);
            Bool_t CoreCorrelate(KTFrequencySpectrumDataFFTWCore& data, KTCorrelationData& newData);

            KTFrequencySpectrumPolar* DoCorrelation(const KTFrequencySpectrumPolar* firstSpectrum, const KTFrequencySpectrumPolar* secondSpectrum);
            KTFrequencySpectrumPolar* DoCorrelation(const KTFrequencySpectrumFFTW* firstSpectrum, const KTFrequencySpectrumFFTW* secondSpectrum);

            //***************
            // Signals
            //***************

        private:
            KTSignalData fCorrSignal;

            //***************
            // Slots
            //***************

        private:
            KTSlotDataOneType< KTFrequencySpectrumDataPolar > fFSPolarSlot;
            KTSlotDataOneType< KTFrequencySpectrumDataFFTW > fFSFFTWSlot;
            KTSlotDataOneType< KTNormalizedFSDataPolar > fNormFSPolarSlot;
            KTSlotDataOneType< KTNormalizedFSDataFFTW > fNormFSFFTWSlot;
    };

    inline void KTCorrelator::AddPair(const UIntPair& pair)
    {
        fPairs.push_back(pair);
        return;
    }

    inline void KTCorrelator::SetPairVector(const PairVector& pairs)
    {
        fPairs = pairs;
        return;
    }

    inline const KTCorrelator::PairVector& KTCorrelator::GetPairVector() const
    {
        return fPairs;
    }

    inline void KTCorrelator::ClearPairs()
    {
        fPairs.clear();
        return;
    }

} /* namespace Katydid */
#endif /* KTCORRELATOR_HH_ */
