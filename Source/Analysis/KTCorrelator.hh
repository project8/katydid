/*
 * KTCorrelator.hh
 *
 *  Created on: Aug 20, 2012
 *      Author: nsoblath
 */

#ifndef KTCORRELATOR_HH_
#define KTCORRELATOR_HH_

#include "KTFrequencySpectrumDataPolar.hh"
#include "KTProcessor.hh"

#include <boost/shared_ptr.hpp>

#include <utility>
#include <vector>

namespace Katydid
{
    class KTCorrelationData : public KTFrequencySpectrumDataPolarCore, public KTExtensibleData< KTCorrelationData >
    {
        public:
            KTCorrelationData() :
                    KTFrequencySpectrumDataPolarCore(),
                    KTExtensibleData< KTCorrelationData >()
            {}
            virtual ~KTCorrelationData()
            {}

            const std::pair< UInt_t, UInt_t >& GetInputPair(UInt_t component = 0) const;

            void SetInputPair(UInt_t first, UInt_t second, UInt_t component = 0);

            KTCorrelationData& SetNComponents(UInt_t components);

        protected:
            std::vector< std::pair< UInt_t, UInt_t > > fWVComponentData;
    };

    inline const std::pair< UInt_t, UInt_t >& KTCorrelationData::GetInputPair(UInt_t component) const
    {
        return fWVComponentData[component];
    }

    inline void KTCorrelationData::SetInputPair(UInt_t first, UInt_t second, UInt_t component)
    {
        if (component >= fSpectra.size()) SetNComponents(component+1);
        fWVComponentData[component].first = first;
        fWVComponentData[component].second = second;
        return;
    }

    inline KTCorrelationData& KTCorrelationData::SetNComponents(UInt_t components)
    {
        fSpectra.resize(components);
        fWVComponentData.resize(components);
        return *this;
    }



    class KTFrequencySpectrumDataFFTW;
    class KTFrequencySpectrumFFTW;

    typedef std::pair< UInt_t, UInt_t > KTCorrelationPair;

    class KTCorrelator : public KTProcessor
    {
        protected:
            typedef KTSignal< void (boost::shared_ptr<KTData>) >::signal CorrelationSignal;
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

            Bool_t Correlate(KTFrequencySpectrumDataPolar& data);
            Bool_t Correlate(KTFrequencySpectrumDataFFTW& data);
            //KTCorrelationData* Correlate(const KTFrequencySpectrumDataPolar* data, const PairVector& pairs);
            //KTCorrelationData* Correlate(const KTFrequencySpectrumDataPolar* data, const KTCorrelationPair& pair);

        protected:
            KTFrequencySpectrumPolar* DoCorrelation(const KTFrequencySpectrumPolar* firstSpectrum, const KTFrequencySpectrumPolar* secondSpectrum);
            KTFrequencySpectrumPolar* DoCorrelation(const KTFrequencySpectrumFFTW* firstSpectrum, const KTFrequencySpectrumFFTW* secondSpectrum);

            //***************
            // Signals
            //***************

        private:
            CorrelationSignal fCorrSignal;

            //***************
            // Slots
            //***************

        public:
            void ProcessFFTData(boost::shared_ptr<KTData>);
            void ProcessFFTWData(boost::shared_ptr<KTData>);
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
