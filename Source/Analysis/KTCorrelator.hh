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

#include <utility>
#include <vector>

namespace Katydid
{
    class KTFrequencySpectrumData;
    class KTFrequencySpectrum;

    typedef std::pair< UInt_t, UInt_t > KTCorrelationPair;

    class KTCorrelator : public KTProcessor, public KTConfigurable
    {
        public:
            KTCorrelator();
            virtual ~KTCorrelator();

            Bool_t Configure(const KTPStoreNode* node);

            Bool_t Correlate(const KTFrequencySpectrumData* data, std::vector< KTCorrelationPair > pairs);
            Bool_t Correlate(const KTFrequencySpectrumData* data, KTCorrelationPair pair);

        protected:
            KTFrequencySpectrum* DoCorrelation(const KTFrequencySpectrum* firstSpectrum, const KTFrequencySpectrum* secondSpectrum);

    };


} /* namespace Katydid */
#endif /* KTCORRELATOR_HH_ */
