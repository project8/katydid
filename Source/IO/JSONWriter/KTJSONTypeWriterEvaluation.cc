/*
 * KTJSONTypeWriterEvaluation.cc
 *
 *  Created on: May 30, 2013
 *      Author: nsoblath
 */

#include "KTJSONTypeWriterEvaluation.hh"

#include "KTCCResults.hh"
#include "KTTIFactory.hh"
//#include "logger.hh"



using std::vector;

namespace Katydid
{
    //LOGGER(publog, "KTJSONTypeWriterEvaluation");

    static Nymph::KTTIRegistrar< KTJSONTypeWriter, KTJSONTypeWriterEvaluation > sJSONTWERegistrar;

    KTJSONTypeWriterEvaluation::KTJSONTypeWriterEvaluation() :
            KTJSONTypeWriter()
    {
    }

    KTJSONTypeWriterEvaluation::~KTJSONTypeWriterEvaluation()
    {
    }


    void KTJSONTypeWriterEvaluation::RegisterSlots()
    {
        fWriter->RegisterSlot("cc-results", this, &KTJSONTypeWriterEvaluation::WriteCCResults);
        return;
    }


    //*********************
    // Frequency Evaluation
    //*********************

    void KTJSONTypeWriterEvaluation::WriteCCResults(Nymph::KTDataPtr data)
    {
        using rapidjson::SizeType;

        KTCCResults& ccrData = data->Of< KTCCResults >();

        if (! fWriter->OpenAndVerifyFile()) return;

        KTJSONWriter::JSONMaker* jsonMaker = fWriter->GetJSONMaker();

        jsonMaker->String("cc-results");
        jsonMaker->StartObject();

        jsonMaker->String("event-length");
        jsonMaker->Double(ccrData.GetEventLength());

        jsonMaker->String("dfdt");
        jsonMaker->Double(ccrData.Getdfdt());

        jsonMaker->String("signal-power");
        jsonMaker->Double(ccrData.GetSignalPower());

        jsonMaker->String("n-events");
        jsonMaker->Uint(ccrData.GetNEvents());

        // n events with x cand vector
        jsonMaker->String("n-events-with-x-cand-matches");
        jsonMaker->StartArray();
        const vector< unsigned >& evVec = ccrData.GetNEventsWithXCandidateMatches();
        for (unsigned nEvents = 0; nEvents < evVec.size(); nEvents++)
        {
            jsonMaker->Uint(evVec[nEvents]);
        }
        jsonMaker->EndArray();

        jsonMaker->String("n-candidates");
        jsonMaker->Uint(ccrData.GetNCandidates());

        // n cand with x ev vector
        jsonMaker->String("n-cands-with-x-event-matches");
        jsonMaker->StartArray();
        const vector< unsigned >& candVec = ccrData.GetNCandidatesWithXEventMatches();
        for (unsigned nCands = 0; nCands < candVec.size(); nCands++)
        {
            jsonMaker->Uint(candVec[nCands]);
        }
        jsonMaker->EndArray();

        jsonMaker->String("efficiency");
        jsonMaker->Double(ccrData.GetEfficiency());

        jsonMaker->String("false-rate");
        jsonMaker->Double(ccrData.GetFalseRate());

        jsonMaker->EndObject();

        return;
    }

} /* namespace Katydid */
