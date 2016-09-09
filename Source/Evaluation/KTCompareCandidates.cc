/*
 * KTCompareCandidates.cc
 *
 *  Created on: Apr 9, 2013
 *      Author: nsoblath
 */

#include "KTCompareCandidates.hh"

#include "KTData.hh"
#include "KTCCResults.hh"
#include "param.hh"

#include <sstream>
#include <vector>



using std::string;
using std::vector;

namespace Katydid
{
    KT_REGISTER_PROCESSOR(KTCompareCandidates, "compare-candidates");

    KTLOGGER(cclog, "KTCompareCandidates");

    KTCompareCandidates::KTCompareCandidates(const string& name) :
            KTProcessor(name),
            fAssumeSparseCandidates(false),
            fTruthAndAnalysisSlot("truth-vs-analysis", this, &KTCompareCandidates::CompareTruthAndAnalysis),
            fCCResultsSignal("cc-results", this)
    {
    }

    KTCompareCandidates::~KTCompareCandidates()
    {
    }

    bool KTCompareCandidates::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return false;

        SetAssumeSparseCandidates(node->get_value<bool>("assume-sparse-candidates", fAssumeSparseCandidates));

        return true;
    }

    bool KTCompareCandidates::CompareTruthAndAnalysis(KTMCTruthEvents& mcEventData, KTAnalysisCandidates& candidateData)
    {
        const KTMCTruthEvents::EventSet& events = mcEventData.GetEvents();
        const KTAnalysisCandidates::CandidateSet& candidates = candidateData.GetCandidates();

        if (events.size() == 0)
        {
            KTINFO(cclog, "No events are present");
	    //  return true;
        }
        if (candidates.size() == 0)
        {
            KTINFO(cclog, "No candidates are present");
	    //   return true;
        }

        const unsigned eventRecordSize = mcEventData.GetRecordSize();
        const unsigned candidateRecordSize = candidateData.GetRecordSize();

        const unsigned nRecords = mcEventData.GetNRecords();
        if (nRecords != candidateData.GetNRecords())
        {
            KTERROR(cclog, "The number of records simulated (" << nRecords << ") does not match the number of records analyzed (" << candidateData.GetNRecords() << ")");
            return false;
        }

        // eventMatches: each position in the vector represents one of the events; this vector records how many candidates matched each event.
        vector< unsigned > eventMatches(events.size());
        // candidateMatches: each position in the vector represents one of the candidates; this vector records how many events matched each candidate.
        vector< unsigned > candidateMatches(candidates.size());

        //uint64_t candidateSampleSum = 0;
        //uint64_t eventSampleSum = 0;
        //uint64_t candidateCorrectSampleSum = 0;

        unsigned eventCounter = 0;
        bool continueEventLoop = true;
        unsigned candidateCounter = 0;
        KTAnalysisCandidates::CandidateSet::const_iterator candStartHere = candidates.begin();
        unsigned candidateCounterStart = 0;
        int comparison = 0;
        KTDEBUG(cclog, "Comparing candidates to events");
        for (KTMCTruthEvents::EventSet::const_iterator truthIt = events.begin(); truthIt != events.end() && continueEventLoop; truthIt++)
        {
            candidateCounter = candidateCounterStart;
            for (KTAnalysisCandidates::CandidateSet::const_iterator candIt = candStartHere; candIt != candidates.end(); candIt++)
            {
                comparison = CompareAnEventToACandidate(*truthIt, *candIt, eventRecordSize, candidateRecordSize);
                if (comparison == 0)
                {
                    eventMatches[eventCounter] = eventMatches[eventCounter] + 1;
                    candidateMatches[candidateCounter] = candidateMatches[candidateCounter] + 1;
                    //KTDEBUG(cclog, "Match found:\n" <<
                    //        "\tEvent " << eventCounter << "): " << truthIt->fStartRecord << ", " << truthIt->fStartSample << ", " << truthIt->fEndRecord << ", " << truthIt->fEndSample << '\n' <<
                    //        "\tCandidate " << candidateCounter << "): " << candIt->fStartRecord << ", " << candIt->fStartSample << ", " << candIt->fEndRecord << ", " << candIt->fEndSample << '\n');
                }
                else if (comparison < 0)
                {
                    //KTDEBUG(cclog, "Candidate before event:\n" <<
                    //        "\tEvent: " << truthIt->fStartRecord << ", " << truthIt->fStartSample << ", " << truthIt->fEndRecord << ", " << truthIt->fEndSample << '\n' <<
                    //        "\tCandidate: " << candIt->fStartRecord << ", " << candIt->fStartSample << ", " << candIt->fEndRecord << ", " << candIt->fEndSample << '\n');
                    if (fAssumeSparseCandidates)
                    {
                        // this candidate is earlier than any remaining events, so it can be skipped for following events
                        candStartHere = candIt;
                        candidateCounterStart = candidateCounter;
                        // move up to the next event as the place to start
                        candStartHere++;
                        candidateCounterStart++;
                        // if this is the last candidate, than there's no use in doing any more comparisons to events
                        if (candStartHere == candidates.end())
                        {
                            continueEventLoop = false;
                            // if we're here, the candidate loop will complete after this cycle
                        }
                    }
                }
                else // comparison > 0
                {
                    //KTDEBUG(cclog, "Candidate after event:\n" <<
                    //        "\tEvent: " << truthIt->fStartRecord << ", " << truthIt->fStartSample << ", " << truthIt->fEndRecord << ", " << truthIt->fEndSample << '\n' <<
                    //        "\tCandidate: " << candIt->fStartRecord << ", " << candIt->fStartSample << ", " << candIt->fEndRecord << ", " << candIt->fEndSample << '\n');
                    // all remaining candidates are after the current truth event, so they can be skipped
                    break;
                }
                candidateCounter++;
            }
            eventCounter++;
        }

        Nymph::KTDataPtr dataPtr(new Nymph::KTData());
        dataPtr->SetLastData(true);
        KTCCResults& ccrData = dataPtr->Of< KTCCResults >();

        ccrData.SetEventLength(mcEventData.GetEventLength());
        ccrData.Setdfdt(mcEventData.Getdfdt());
        ccrData.SetSignalPower(mcEventData.GetSignalPower());

        ccrData.SetNEvents(events.size());
        ccrData.SetNCandidates(candidates.size());

        // iterate through eventMatches and candidateMatches to collect interesting statistics
        unsigned largestNumberOfMatches = 0;
        unsigned nEventsWithAtLeastOneCandidateMatch = 0;
        ccrData.ResizeNEventsWithXCandidateMatches(candidates.size()); // the largest size this should be is the number of candidates
        for (unsigned iEvent = 0; iEvent < eventMatches.size(); iEvent++)
        {
            if (eventMatches[iEvent] > largestNumberOfMatches)
            {
                largestNumberOfMatches = eventMatches[iEvent];
            }
            if (eventMatches[iEvent] > 0)
            {
                nEventsWithAtLeastOneCandidateMatch++;
            }
            ccrData.IncrementNEventsWithXCandidateMatches(eventMatches[iEvent]);
            //nEventsWithCandidateMatches[eventMatches[iEvent]] = nEventsWithCandidateMatches[eventMatches[iEvent]] + 1;
        }
        //nEventsWithCandidateMatches.resize(largestNumberOfMatches + 1);
        ccrData.ResizeNEventsWithXCandidateMatches(largestNumberOfMatches + 1);

        KTPROG(cclog, "Number of events: " << ccrData.GetNEvents());
        KTINFO(cclog, "Largest number of candidates matching an event: " << largestNumberOfMatches);
        std::stringstream textHist1;
        const vector< unsigned >& nEventsWithXCandidateMatches = ccrData.GetNEventsWithXCandidateMatches();
        for (unsigned iNEvents = 0; iNEvents < nEventsWithXCandidateMatches.size(); iNEvents++)
        {
            textHist1 << iNEvents << ": " << nEventsWithXCandidateMatches[iNEvents] << '\n';
        }
        KTINFO(cclog, "Number of events (y axis) with a given number of candidate matches (x axis):\n" << textHist1.str());

        ccrData.SetEfficiency(double(nEventsWithAtLeastOneCandidateMatch) / double(events.size()));
        KTPROG(cclog, "Detection efficiency (# events with at least 1 match / # events): " << ccrData.GetEfficiency());


        largestNumberOfMatches = 0;
        ccrData.ResizeNCandidatesWithXEventMatches(events.size()+1); //  the largest size this should be is the number of events
        if (ccrData.GetNCandidatesWithXEventMatches().size() > 0)
        {
            for (unsigned iCandidate = 0; iCandidate < candidateMatches.size(); iCandidate++)
            {
                if (candidateMatches[iCandidate] > largestNumberOfMatches)
                {
                    largestNumberOfMatches = candidateMatches[iCandidate];
                }
                ccrData.IncrementNCandidatesWithXEventMatches(candidateMatches[iCandidate]);
            }
            ccrData.ResizeNCandidatesWithXEventMatches(largestNumberOfMatches + 1);
        }
        KTPROG(cclog, "Number of candidates: " << ccrData.GetNCandidates());
        KTINFO(cclog, "Largest number of events matching a candidate: " << largestNumberOfMatches);
        std::stringstream textHist2;
        const vector< unsigned >& nCandidatesWithXEventMatches = ccrData.GetNCandidatesWithXEventMatches();
        for (unsigned iNCandidates = 0; iNCandidates < nCandidatesWithXEventMatches.size(); iNCandidates++)
        {
            textHist2 << iNCandidates << ": " << nCandidatesWithXEventMatches[iNCandidates] << '\n';
        }
        KTINFO(cclog, "Number of candidates (y axis) with a given number of event matches (x axis):\n" << textHist2.str());

        ccrData.SetFalseRate(1.e6 * double(nCandidatesWithXEventMatches[0]) / (double(nRecords) * double(eventRecordSize)));
        KTPROG(cclog, "False rate (10^6 * # candidates not matching events / # of samples simulated): " << ccrData.GetFalseRate());

        fCCResultsSignal(dataPtr);

        return true;
    }

    int KTCompareCandidates::CompareAnEventToACandidate(const KTMCTruthEvents::Event& event, const KTAnalysisCandidates::Candidate& candidate, unsigned eventRecordSize, unsigned candidateRecordSize) const
    {
        /* more readable version
        uint64_t candidateSampleStart = candidate.fStartRecord * candidateRecordSize + candidate.fStartSample;
        uint64_t eventSampleEnd = event.fEndRecord * eventRecordSize + event.fEndSample;
        if (candidateSampleStart > eventSampleEnd) return 1;

        uint64_t candidateSampleEnd = candidate.fEndRecord * candidateRecordSize + candidate.fEndSample;
        uint64_t eventSampleStart = event.fStartRecord * eventRecordSize + event.fStartSample;
        if (candidateSampleEnd < eventSampleStart) return -1;
        */

        if (candidate.fStartRecord * candidateRecordSize + candidate.fStartSample >
                event.fEndRecord * eventRecordSize + event.fEndSample)
            return 1;

        if (candidate.fEndRecord * candidateRecordSize + candidate.fEndSample <
                event.fStartRecord * eventRecordSize + event.fStartSample)
            return -1;

        return 0;
    }


} /* namespace Katydid */
