/*
 * KTCorrelator.hh
 *
 *  Created on: Aug 20, 2012
 *      Author: nsoblath
 */

#ifndef KTCORRELATOR_HH_
#define KTCORRELATOR_HH_

#include "KTProcessor.hh"

#include <boost/shared_ptr.hpp>

#include <utility>
#include <vector>

namespace Katydid
{
    class KTCorrelationData;
    class KTBundle;
    class KTFrequencySpectrum;
    class KTFrequencySpectrumData;
    class KTFrequencySpectrumDataFFTW;
    class KTFrequencySpectrumFFTW;

    typedef std::pair< UInt_t, UInt_t > KTCorrelationPair;

    class KTCorrelator : public KTProcessor
    {
        protected:
            typedef KTSignal< void (const KTCorrelationData*) >::signal CorrelationSignal;
            typedef std::vector< KTCorrelationPair > PairVector;

        public:
            KTCorrelator();
            virtual ~KTCorrelator();

            Bool_t Configure(const KTPStoreNode* node);

            void AddPair(const KTCorrelationPair& pair);
            void SetPairVector(const PairVector& pairs);
            const PairVector& GetPairVector() const;
            void ClearPairs();

            const std::string& GetInputDataName() const;
            void SetInputDataName(const std::string& name);

            const std::string& GetOutputDataName() const;
            void SetOutputDataName(const std::string& name);

        protected:
            PairVector fPairs;

            std::string fInputDataName;
            std::string fOutputDataName;

        public:

            KTCorrelationData* Correlate(const KTFrequencySpectrumData* data);
            KTCorrelationData* Correlate(const KTFrequencySpectrumDataFFTW* data);
            //KTCorrelationData* Correlate(const KTFrequencySpectrumData* data, const PairVector& pairs);
            //KTCorrelationData* Correlate(const KTFrequencySpectrumData* data, const KTCorrelationPair& pair);

        protected:
            KTFrequencySpectrum* DoCorrelation(const KTFrequencySpectrum* firstSpectrum, const KTFrequencySpectrum* secondSpectrum);
            KTFrequencySpectrum* DoCorrelation(const KTFrequencySpectrumFFTW* firstSpectrum, const KTFrequencySpectrumFFTW* secondSpectrum);

            //***************
            // Signals
            //***************

        private:
            CorrelationSignal fCorrSignal;

            //***************
            // Slots
            //***************

        public:
            void ProcessFFTData(const KTFrequencySpectrumData* fsData);
            void ProcessFFTWData(const KTFrequencySpectrumDataFFTW* fsData);
            void ProcessBundle(boost::shared_ptr<KTBundle> bundle);


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

    inline const std::string& KTCorrelator::GetInputDataName() const
    {
        return fInputDataName;
    }

    inline void KTCorrelator::SetInputDataName(const std::string& name)
    {
        fInputDataName = name;
        return;
    }

    inline const std::string& KTCorrelator::GetOutputDataName() const
    {
        return fOutputDataName;
    }

    inline void KTCorrelator::SetOutputDataName(const std::string& name)
    {
        fOutputDataName = name;
        return;
    }


} /* namespace Katydid */
#endif /* KTCORRELATOR_HH_ */
