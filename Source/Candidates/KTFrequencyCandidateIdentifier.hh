/*
 * KTFrequencyCandidateIdentifier.hh
 *
 *  Created on: Dec 17, 2012
 *      Author: nsoblath
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
    class KTEvent;
    class KTFrequencySpectrum;
    class KTFrequencySpectrumData;
    class KTFrequencySpectrumDataFFTW;
    class KTFrequencySpectrumFFTW;

    class KTFrequencyCandidateIdentifier : public KTProcessor
    {
        protected:
            typedef KTSignal< void (const KTFrequencyCandidateData*) >::signal FCSignal;

        public:
            KTFrequencyCandidateIdentifier();
            virtual ~KTFrequencyCandidateIdentifier();

            Bool_t Configure(const KTPStoreNode* node);

            const std::string& GetFSInputDataName() const;
            void SetFSInputDataName(const std::string& name);

            const std::string& GetClusterInputDataName() const;
            void SetClusterInputDataName(const std::string& name);

            const std::string& GetOutputDataName() const;
            void SetOutputDataName(const std::string& name);

        protected:
            std::string fFSInputDataName;
            std::string fClusterInputDataName;
            std::string fOutputDataName;


        public:
            KTFrequencyCandidateData* IdentifyCandidates(const KTCluster1DData* clusterData, const KTFrequencySpectrumData* fsData);
            KTFrequencyCandidateData* IdentifyCandidates(const KTCluster1DData* clusterData, const KTFrequencySpectrumDataFFTW* fsData);
            KTFrequencyCandidateData* IdentifyCandidates(const KTCluster1DData* clusterData, const KTCorrelationData* fsData);

            KTFrequencyCandidateData::Candidates IdentifyCandidates(const KTCluster1DData::SetOfClusters& clusters, const KTFrequencySpectrum* freqSpec);
            KTFrequencyCandidateData::Candidates IdentifyCandidates(const KTCluster1DData::SetOfClusters& clusters, const KTFrequencySpectrumFFTW* freqSpec);


            //***************
            // Signals
            //***************

        private:
            FCSignal fFCSignal;

            //***************
            // Slots
            //***************

        public:
            void ProcessEvent(boost::shared_ptr<KTEvent> event);
            void ProcessClusterData(const KTCluster1DData* tsData);

    };

    inline const std::string& KTFrequencyCandidateIdentifier::GetFSInputDataName() const
    {
        return fFSInputDataName;
    }

    inline void KTFrequencyCandidateIdentifier::SetFSInputDataName(const std::string& name)
    {
        fFSInputDataName = name;
        return;
    }

    inline const std::string& KTFrequencyCandidateIdentifier::GetClusterInputDataName() const
    {
        return fClusterInputDataName;
    }

    inline void KTFrequencyCandidateIdentifier::SetClusterInputDataName(const std::string& name)
    {
        fClusterInputDataName = name;
        return;
    }

    inline const std::string& KTFrequencyCandidateIdentifier::GetOutputDataName() const
    {
        return fOutputDataName;
    }

    inline void KTFrequencyCandidateIdentifier::SetOutputDataName(const std::string& name)
    {
        fOutputDataName = name;
        return;
    }

} /* namespace Katydid */
#endif /* KTFREQUENCYCANDIDATEIDENTIFIER_HH_ */
