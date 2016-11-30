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
#include "KTData.hh"
#include "KTFrequencyCandidateData.hh"
#include "KTSlot.hh"



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
    
     Configuration name: "frequency-candidate-identifier"

     Available configuration values:
     - "fs-input-data-name": string -- filename for loading/saving FFTW wisdom
     - "cluster-input-data-name": string -- name of the data to find when processing an event
     - "output-data-name": string -- name to give to the data produced by an FFT

     Slots:
     - "clusters": void (Nymph::KTDataPtr) --

     Signals:
     - "frequency-candidates": void (Nymph::KTDataPtr) -- Emitted after identifying candidates; Guarantees KTFrequencyCandidateData
    */



    class KTFrequencyCandidateIdentifier : public Nymph::KTProcessor
    {
        protected:
            typedef Nymph::KTSignalConcept< void (Nymph::KTDataPtr) >::signal FCSignal;

        public:
            KTFrequencyCandidateIdentifier(const std::string& name = "frequency-candidate-identifier");
            virtual ~KTFrequencyCandidateIdentifier();

            bool Configure(const scarab::param_node* node);

        public:
            bool IdentifyCandidates(KTCluster1DData& clusterData, KTFrequencySpectrumDataPolar& fsData);
            bool IdentifyCandidates(KTCluster1DData& clusterData, KTFrequencySpectrumDataFFTW& fsData);
            bool IdentifyCandidates(KTCluster1DData& clusterData, KTNormalizedFSDataPolar& fsData);
            bool IdentifyCandidates(KTCluster1DData& clusterData, KTNormalizedFSDataFFTW& fsData);
            bool IdentifyCandidates(KTCluster1DData& clusterData, KTCorrelationData& fsData);

            KTFrequencyCandidateData::Candidates IdentifyCandidates(const KTCluster1DData::SetOfClusters& clusters, const KTFrequencySpectrumPolar* freqSpec);
            KTFrequencyCandidateData::Candidates IdentifyCandidates(const KTCluster1DData::SetOfClusters& clusters, const KTFrequencySpectrumFFTW* freqSpec);

        private:
            bool CoreIdentifyCandidates(KTCluster1DData& clusterData, const KTFrequencySpectrumDataPolarCore& fsData, KTFrequencyCandidateData& fcData);
            bool CoreIdentifyCandidates(KTCluster1DData& clusterData, const KTFrequencySpectrumDataFFTWCore& fsData, KTFrequencyCandidateData& fcData);

            //***************
            // Signals
            //***************

        private:
            Nymph::KTSignalData fFCSignal;

            //***************
            // Slots
            //***************

        private:
            Nymph::KTSlotDataTwoTypes< KTCluster1DData, KTFrequencySpectrumDataPolar > fFSDataPolarSlot;
            Nymph::KTSlotDataTwoTypes< KTCluster1DData, KTFrequencySpectrumDataFFTW > fFSDataFFTWSlot;
            Nymph::KTSlotDataTwoTypes< KTCluster1DData, KTNormalizedFSDataPolar > fFSNormDataPolarSlot;
            Nymph::KTSlotDataTwoTypes< KTCluster1DData, KTNormalizedFSDataFFTW > fFSNormDataFFTWSlot;
            Nymph::KTSlotDataTwoTypes< KTCluster1DData, KTCorrelationData > fFSCorrelationDataSlot;

    };

} /* namespace Katydid */
#endif /* KTFREQUENCYCANDIDATEIDENTIFIER_HH_ */
