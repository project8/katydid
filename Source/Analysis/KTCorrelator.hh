/**
 @file KTCorrelator.hh
 @brief Contains KTCorrelator
 @details Correlates frequency spectra from different channels
 @author: N. S. Oblath
 @date: Aug  20, 2012
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
    class KTEvent;
    class KTFrequencySpectrum;
    class KTFrequencySpectrumData;
    class KTWriteableData;

    typedef std::pair< UInt_t, UInt_t > KTCorrelationPair;


    /*!
     @class KTCorrelator
     @author N. S. Oblath

     @brief Correlates frequencey spectra from different channels.

     @details
     
     Available configuration values:
     \li \c "corr-pair": string -- channel pair to be correlated: "[first channel], [second channel]"; e.g. "0, 0" or "0, 1"
     \li \c "input-data-name": string -- name of the data to find when processing an event
     \li \c "output-data-name": string -- name to give to the data produced by a correlation

      Slots:
     \li \c "event": void ProcessEvent(boost::shared_ptr<KTEvent>)
     \li \c "fft-data": void ProcessTimeSeriesData(const KTTimeSeriesDataReal*)

     Signals:
     \li \c "correlation": void (const KTWriteableData*) emitted upon performance of a correlation.
    */



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
            void ProcessEvent(boost::shared_ptr<KTEvent> event);


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
