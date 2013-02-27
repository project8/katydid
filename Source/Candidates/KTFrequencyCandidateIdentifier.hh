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
#include "KTSlot.hh"

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
     \li \c "clusters": void (shared_ptr< KTData >) --

     Signals:
     \li \c "frequency-candidates": void (shared_ptr< KTData >) -- Emitted after identifying candidates; Guarantees KTFrequencyCandidateData
    */



    class KTFrequencyCandidateIdentifier : public KTProcessor
    {
        protected:
            typedef KTSignalConcept< void (boost::shared_ptr< KTData >) >::signal FCSignal;

        public:
            KTFrequencyCandidateIdentifier(const std::string& name = "frequency-candidate-identifier");
            virtual ~KTFrequencyCandidateIdentifier();

            Bool_t Configure(const KTPStoreNode* node);

        public:
            Bool_t IdentifyCandidates(KTCluster1DData& clusterData, KTFrequencySpectrumDataPolar& fsData);
            Bool_t IdentifyCandidates(KTCluster1DData& clusterData, KTFrequencySpectrumDataFFTW& fsData);
            Bool_t IdentifyCandidates(KTCluster1DData& clusterData, KTNormalizedFSDataPolar& fsData);
            Bool_t IdentifyCandidates(KTCluster1DData& clusterData, KTNormalizedFSDataFFTW& fsData);
            Bool_t IdentifyCandidates(KTCluster1DData& clusterData, KTCorrelationData& fsData);

            KTFrequencyCandidateData::Candidates IdentifyCandidates(const KTCluster1DData::SetOfClusters& clusters, const KTFrequencySpectrumPolar* freqSpec);
            KTFrequencyCandidateData::Candidates IdentifyCandidates(const KTCluster1DData::SetOfClusters& clusters, const KTFrequencySpectrumFFTW* freqSpec);

        private:
            Bool_t CoreIdentifyCandidates(KTCluster1DData& clusterData, const KTFrequencySpectrumDataPolarCore& fsData, KTFrequencyCandidateData& fcData);
            Bool_t CoreIdentifyCandidates(KTCluster1DData& clusterData, const KTFrequencySpectrumDataFFTWCore& fsData, KTFrequencyCandidateData& fcData);

            //***************
            // Signals
            //***************

        private:
            KTSignalData fFCSignal;

            //***************
            // Slots
            //***************

        private:
            KTSlotDataTwoTypes< KTCluster1DData, KTFrequencySpectrumDataPolar > fFSDataPolarSlot;
            KTSlotDataTwoTypes< KTCluster1DData, KTFrequencySpectrumDataFFTW > fFSDataFFTWSlot;
            KTSlotDataTwoTypes< KTCluster1DData, KTNormalizedFSDataPolar > fFSNormDataPolarSlot;
            KTSlotDataTwoTypes< KTCluster1DData, KTNormalizedFSDataFFTW > fFSNormDataFFTWSlot;
            KTSlotDataTwoTypes< KTCluster1DData, KTCorrelationData > fFSCorrelationDataSlot;

    };

} /* namespace Katydid */
#endif /* KTFREQUENCYCANDIDATEIDENTIFIER_HH_ */
