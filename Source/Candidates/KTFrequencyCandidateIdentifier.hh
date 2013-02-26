/**
 @file KTFrequencyCandidateIdentifier.hh 
 @brief Contains KTFrequencyCandidateIdentifier
 @details 
 @author: N. S. Oblath
 @date: Dec 17, 2012
 */


#ifndef KTFREQUENCYCANDIDATEIDENTIFIER_HH_
#define KTFREQUENCYCANDIDATEIDENTIFIER_HH_

#include "KTProcessor.hh"

#include "KTCluster1DData.hh"
#include "KTFrequencyCandidateData.hh"

#include <boost/shared_ptr.hpp>


namespace Katydid
{
    class KTCorrelationData;
    class KTFrequencySpectrumDataFFTW;
    class KTFrequencySpectrumDataFFTWCore;
    class KTFrequencySpectrumFFTW;
    class KTFrequencySpectrumDataPolar;
    class KTFrequencySpectrumDataPolarCore;
    class KTFrequencySpectrumPolar;
    class KTNormalizedFSDataFFTW;
    class KTNormalizedFSDataPolar;

 /*!
     @class KTFrequencyCandidateIdentifier
     @author N. S. Oblath

     @brief 

     @details
    

     Available configuration values:
      \li \c "fs-input-data-name": string -- filename for loading/saving FFTW wisdom
     \li \c "cluster-input-data-name": string -- name of the data to find when processing an event
     \li \c "output-data-name": string -- name to give to the data produced by an FFT

     Slots:
     \li \c "event": void ProcessEvent(boost::shared_ptr<KTEvent>)
     \li \c "clusters": void ProcessClusterData(const KTCluster1DData*)

     Signals:
     \li \c "frequency-candidates": void (const KTFrequencyCandidateData*) emitted upon performance of a          .
    */



    class KTFrequencyCandidateIdentifier : public KTProcessor
    {
        protected:
            typedef KTSignalConcept< void (boost::shared_ptr< KTData >) >::signal FCSignal;

        public:
            KTFrequencyCandidateIdentifier();
            virtual ~KTFrequencyCandidateIdentifier();

            Bool_t Configure(const KTPStoreNode* node);

        public:
            Bool_t IdentifyCandidates(KTCluster1DData& clusterData, const KTFrequencySpectrumDataPolar& fsData);
            Bool_t IdentifyCandidates(KTCluster1DData& clusterData, const KTFrequencySpectrumDataFFTW& fsData);
            Bool_t IdentifyCandidates(KTCluster1DData& clusterData, const KTNormalizedFSDataPolar& fsData);
            Bool_t IdentifyCandidates(KTCluster1DData& clusterData, const KTNormalizedFSDataFFTW& fsData);
            Bool_t IdentifyCandidates(KTCluster1DData& clusterData, const KTCorrelationData& fsData);

            KTFrequencyCandidateData::Candidates IdentifyCandidates(const KTCluster1DData::SetOfClusters& clusters, const KTFrequencySpectrumPolar* freqSpec);
            KTFrequencyCandidateData::Candidates IdentifyCandidates(const KTCluster1DData::SetOfClusters& clusters, const KTFrequencySpectrumFFTW* freqSpec);

        private:
            Bool_t CoreIdentifyCandidates(KTCluster1DData& clusterData, const KTFrequencySpectrumDataPolarCore& fsData, KTFrequencyCandidateData& fcData);
            Bool_t CoreIdentifyCandidates(KTCluster1DData& clusterData, const KTFrequencySpectrumDataFFTWCore& fsData, KTFrequencyCandidateData& fcData);

            //***************
            // Signals
            //***************

        private:
            FCSignal fFCSignal;

            //***************
            // Slots
            //***************

        public:
            void ProcessClusterAndFSPolarData(boost::shared_ptr< KTData >);
            void ProcessClusterAndFSFFTWData(boost::shared_ptr< KTData >);
            void ProcessClusterAndNormFSPolarData(boost::shared_ptr< KTData >);
            void ProcessClusterAndNormFSFFTWData(boost::shared_ptr< KTData >);
            void ProcessClusterAndCorrelationData(boost::shared_ptr< KTData >);

    };

} /* namespace Katydid */
#endif /* KTFREQUENCYCANDIDATEIDENTIFIER_HH_ */
