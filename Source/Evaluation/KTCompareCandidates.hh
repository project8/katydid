/*
 * KTCompareCandidates.hh
 *
 *  Created on: Apr 9, 2013
 *      Author: nsoblath
 */

#ifndef KTCOMPARECANDIDATES_HH_
#define KTCOMPARECANDIDATES_HH_

#include "KTProcessor.hh"

namespace Katydid
{
    class KTPStoreNode;

    class KTCompareCandidates : public KTProcessor
    {
        public:
            struct TruthElectron
            {
                Double_t fStartTime;
                Double_t fEndTime;
            };

            struct AnalysisCandidate
            {
                Double_t fStartTime;
                Double_t fEndTime;
                Double_t fStartTimeUncert;
                Double_t fEndTimeUncert;
            };

        public:
            KTCompareCandidates(const std::string& name = "compare-candidates");
            virtual ~KTCompareCandidates();

            Bool_t Configure(const KTPStoreNode* node);


    };

} /* namespace Katydid */
#endif /* KTCOMPARECANDIDATES_HH_ */
