/*
 * KTCorrelator.hh
 *
 *  Created on: Aug 20, 2012
 *      Author: nsoblath
 */

#ifndef KTCORRELATOR_HH_
#define KTCORRELATOR_HH_

#include "KTProcessor.hh"

#include <utility>
#include <vector>

namespace Katydid
{
    class KTCorrelationData;
    class KTEvent;
    class KTFrequencySpectrum;
    class KTFrequencySpectrumData;
    class KTWriteableData;

    typedef std::pair< UInt_t, UInt_t > KTCorrelationPair;

    class KTCorrelator : public KTProcessor
    {
        protected:
            typedef KTSignal< void (const KTWriteableData*) >::signal CorrelationSignal;
            typedef std::vector< KTCorrelationPair > PairVector;

        public:
            KTCorrelator();
            virtual ~KTCorrelator();

            Bool_t Configure(const KTPStoreNode* node);

            void AddPair(const KTCorrelationPair& pair);
            void SetPairVector(const PairVector& pairs);
            const PairVector& GetPairVector() const;
            void ClearPairs();

        protected:
            PairVector fPairs;

        public:

            KTCorrelationData* Correlate(const KTFrequencySpectrumData* data);
            //KTCorrelationData* Correlate(const KTFrequencySpectrumData* data, const PairVector& pairs);
            //KTCorrelationData* Correlate(const KTFrequencySpectrumData* data, const KTCorrelationPair& pair);

        protected:
            KTFrequencySpectrum* DoCorrelation(const KTFrequencySpectrum* firstSpectrum, const KTFrequencySpectrum* secondSpectrum);

            //***************
            // Signals
            //***************

        private:
            CorrelationSignal fCorrSignal;

            //***************
            // Slots
            //***************

        public:
            void ProcessFFTData(const KTFrequencySpectrumData* tsData);
            void ProcessEvent(KTEvent* event);


    };

    inline void KTCorrelator::AddPair(const KTCorrelationPair& pair)
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
