/*
 * KTCorrelator.hh
 *
 *  Created on: Aug 20, 2012
 *      Author: nsoblath
 */

#ifndef KTCORRELATOR_HH_
#define KTCORRELATOR_HH_

#include "KTProcessor.hh"
#include "KTConfigurable.hh"

#include "KTFFTTypes.hh"

#include <utility>
#include <vector>

namespace Katydid
{
    class KTFrequencySpectrumData;
    class KTWriteableData;

    typedef std::pair< UInt_t, UInt_t > KTCorrelationPair;

    class KTCorrelator : public KTProcessor, public KTConfigurable
    {
        protected:
            typedef KTSignal< void (const KTWriteableData*) >::signal CorrelationSignal;
            typedef std::vector< KTCorrelationPair > PairVector;

        public:
            KTCorrelator();
            virtual ~KTCorrelator();

            Bool_t Configure(const KTPStoreNode* node);

        protected:
            PairVector fPairs;

        public:

            Bool_t Correlate(const KTFrequencySpectrumData* data, const PairVector& pairs);
            Bool_t Correlate(const KTFrequencySpectrumData* data, const KTCorrelationPair& pair);

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
            //void ProcessEvent(const KTEvent* event);


    };


} /* namespace Katydid */
#endif /* KTCORRELATOR_HH_ */
