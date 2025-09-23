/*
 * KTJSONTypeWriterEventAnalysis.cc
 *
 *  Created on: Jan 4, 2013
 *      Author: nsoblath
 */

#include "KTFrequencyCandidate.hh"
#include "KTFrequencyCandidateData.hh"
#include "KTProcessedTrackData.hh"
#include "KTTIFactory.hh"
#include "KTLogger.hh"

#include <sstream>
#include "KTJSONTypeWriterEventAnalysis.hh"



using std::stringstream;
using std::string;

namespace Katydid
{
    KTLOGGER(publog, "KTJSONTypeWriterEventAnalysis");

    static Nymph::KTTIRegistrar< KTJSONTypeWriter, KTJSONTypeWriterEventAnalysis > sJSONTWCRegistrar;

    KTJSONTypeWriterEventAnalysis::KTJSONTypeWriterEventAnalysis() :
            fProcTrackParams( nullptr ),
            KTJSONTypeWriter()
            //KTTypeWriterEventAnalysis()
    {
    }

    KTJSONTypeWriterEventAnalysis::~KTJSONTypeWriterEventAnalysis()
    {
    }


    void KTJSONTypeWriterEventAnalysis::RegisterSlots()
    {
        fWriter->RegisterSlot("frequency-candidates", this, &KTJSONTypeWriterEventAnalysis::WriteFrequencyCandidates);
        fWriter->RegisterSlot("proc-track", this, &KTJSONTypeWriterEventAnalysis::AccumulateProcessedTracks);
        fWriter->RegisterSlot("write-tracks", this, &KTJSONTypeWriterEventAnalysis::WriteProcessedTracks);
        return;
    }


    //*********************
    // Processed Tracks
    //*********************

    void KTJSONTypeWriterEventAnalysis::AccumulateProcessedTracks(Nymph::KTDataPtr data)
    {
        if ( fProcTrackParams == nullptr )
        {
            fProcTrackParams = new scarab::param_array();
        }

        KTProcessedTrackData& ptData = data->Of< KTProcessedTrackData >();

        scarab::param_value tTrackID;
        tTrackID.set( ptData.GetTrackID() );
        scarab::param_value tStartFrequency;
        tStartFrequency.set( ptData.GetStartFrequency() );
        scarab::param_value tSlope;
        tSlope.set( ptData.GetSlope() );

        scarab::param_node tTrackNode;
        tTrackNode.add("track-id", tTrackID);
        tTrackNode.add("start-frequency", tStartFrequency);
        tTrackNode.add("slope", tSlope);

        fProcTrackParams->as_array().push_back( tTrackNode );

        return;

    }

    void KTJSONTypeWriterEventAnalysis::WriteProcessedTracks()
    {
        if ( fProcTrackParams == nullptr )
        {
            return;
        }
        else
        {
            KTDEBUG(publog, "Got the \"done\" signal.  Writing processed tracks to json file.");
        }

        using rapidjson::SizeType;

        if (! fWriter->OpenAndVerifyFile()) return;

        KTJSONWriter::JSONMaker* jsonMaker = fWriter->GetJSONMaker();

        jsonMaker->String("processed-tracks");
        jsonMaker->StartArray();

        for (unsigned iTrack=0; iTrack < fProcTrackParams->size(); iTrack++)
        {
            jsonMaker->StartObject();

            jsonMaker->String("track-id");
            jsonMaker->Int(fProcTrackParams->at(iTrack)->as_node()["track-id"]().as_int());

            jsonMaker->String("start-frequency");
            jsonMaker->Double(fProcTrackParams->at(iTrack)->as_node()["start-frequency"]().as_double());

            jsonMaker->String("slope");
            jsonMaker->Double(fProcTrackParams->at(iTrack)->as_node()["slope"]().as_double());

            jsonMaker->EndObject();
        }

        jsonMaker->EndArray();

        return;
    }



    //*********************
    // Frequency Candidates
    //*********************

    void KTJSONTypeWriterEventAnalysis::WriteFrequencyCandidates(Nymph::KTDataPtr data)
    {
        using rapidjson::SizeType;

        KTFrequencyCandidateData& fcData = data->Of< KTFrequencyCandidateData >();

        unsigned nComponents = fcData.GetNComponents();

        if (! fWriter->OpenAndVerifyFile()) return;

        stringstream conv;

        KTJSONWriter::JSONMaker* jsonMaker = fWriter->GetJSONMaker();

        jsonMaker->String("candidates");
        jsonMaker->StartObject();

        for (unsigned iGroup=0; iGroup < nComponents; iGroup++)
        {
            const KTFrequencyCandidateData::Candidates& candidates = fcData.GetCandidates(iGroup);
            conv << iGroup;
            jsonMaker->String(conv.str().c_str());
            jsonMaker->StartArray();

            for (KTFrequencyCandidateData::Candidates::const_iterator candIt = candidates.begin(); candIt != candidates.end(); candIt++)
            {
                jsonMaker->StartObject();
                jsonMaker->String("first-bin");
                jsonMaker->Uint(candIt->GetFirstBin());

                jsonMaker->String("last-bin");
                jsonMaker->Uint(candIt->GetLastBin());

                jsonMaker->String("mean-frequency");
                jsonMaker->Double(candIt->GetMeanFrequency());
                jsonMaker->EndObject();
            }

            jsonMaker->EndArray();
        }

        jsonMaker->EndObject();

        return;
    }

} /* namespace Katydid */
