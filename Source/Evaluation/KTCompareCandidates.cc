/*
 * KTCompareCandidates.cc
 *
 *  Created on: Apr 9, 2013
 *      Author: nsoblath
 */

#include "KTCompareCandidates.hh"

#include "KTAnalysisCandidates.hh"
#include "KTFactory.hh"
#include "KTMCTruthEvents.hh"
#include "KTPStoreNode.hh"

#include <map>
#include <vector>

using std::map;
using std::string;
using std::vector;

namespace Katydid
{
    static KTDerivedRegistrar< KTProcessor, KTCompareCandidates > sCompCandRegistrar("compare-candidates");

    KTCompareCandidates::KTCompareCandidates(const string& name) :
            KTProcessor(name),
            fTruthAndAnalysisSlot("truth-and-analysis", this, &KTCompareCandidates::CompareTruthAndAnalysis)
    {
    }

    KTCompareCandidates::~KTCompareCandidates()
    {
    }

    Bool_t KTCompareCandidates::Configure(const KTPStoreNode* node)
    {
        return true;
    }

    Bool_t KTCompareCandidates::CompareTruthAndAnalysis(const KTMCTruthEvents& mcEventData, const KTAnalysisCandidates& candidateData)
    {
        const KTMCTruthEvents::EventSet& events = mcEventData.GetEvents();
        const KTAnalysisCandidates::CandidateSet& candidates = candidateData.GetCandidates();

        vector< UInt_t > eventMatches(events.size());
        map< KTAnalysisCandidates::CandidateSet::const_iterator, UInt_t > candidateMatches(candidates.size());

        KTAnalysisCandidates::CandidateSet::const_iterator candStartHere = candidates.begin();

        UInt_t eventCounter = 0;
        for (KTMCTruthEvents::EventSet::const_iterator truthIt = events.begin(); truthIt != events.end(); truthIt++)
        {
            UInt_t candidateCounter = 0;
            for (KTAnalysisCandidates::CandidateSet::const_iterator candIt = candStartHere; candIt != candidates.end(); candidates++)
            {
                Int_t comparison = CompareAnEventToACandidate(*truthIt, *candIt);
                if (comparison == 0)
                {
                    eventMatches[eventCounter] = eventMatches[eventCounter] + 1;
                    candidateMatches[candIt] = candidateMatches[candIt] + 1;
                }
                else if (comparison < 0)
                {
                    candStartHere = candIt;
                }
                candidateCounter++;
            }
            eventCounter++;
        }

        return true;
    }

    Int_t KTCompareCandidates::CompareAnEventToACandidate(const KTMCTruthEvents::Event& event, const KTAnalysisCandidates::Candidate& candidate)
    {
        return 0;
    }


} /* namespace Katydid */
