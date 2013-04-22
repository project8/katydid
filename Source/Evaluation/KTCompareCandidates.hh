/*
 * KTCompareCandidates.hh
 *
 *  Created on: Apr 9, 2013
 *      Author: nsoblath
 */

#ifndef KTCOMPARECANDIDATES_HH_
#define KTCOMPARECANDIDATES_HH_

#include "KTProcessor.hh"

#include "KTSlot.hh"

namespace Katydid
{
    class KTAnalysisCandidates;
    class KTMCTruthEvents;
    class KTPStoreNode;

    class KTCompareCandidates : public KTProcessor
    {
        public:
            KTCompareCandidates(const std::string& name = "compare-candidates");
            virtual ~KTCompareCandidates();

            Bool_t Configure(const KTPStoreNode* node);

        public:
            Bool_t CompareTruthAndAnalysis(const KTMCTruthEvents& mcEventData, const KTAnalysisCandidates& candidateData);

        private:
            // Return values:
            //   -1 if candidate occurs completely before event
            //    0 if candidate and event overlap
            //    1 if candidate occurs completely after event
            Int_t CompareAnEventToACandidate(const KTMCTruthEvents::Event& event, const KTAnalysisCandidates::Candidate& candidate);

            //***************
            // Slots
            //***************
        private:
            KTSlotDataTwoTypes< KTMCTruthEvents, KTAnalysisCandidates > fTruthAndAnalysisSlot;

    };

} /* namespace Katydid */
#endif /* KTCOMPARECANDIDATES_HH_ */
