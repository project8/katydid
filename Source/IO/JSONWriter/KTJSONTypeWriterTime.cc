/*
 * KTJSONTypeWriterTime.cc
 *
 *  Created on: Jan 4, 2013
 *      Author: nsoblath
 */

#include "KTArbitraryMetadata.hh"
#include "KTEggHeader.hh"
#include "KTJSONTypeWriterTime.hh"
#include "KTTIFactory.hh"

//#include "KTLogger.hh"

using std::string;

namespace Katydid
{
    //KTLOGGER(publog, "KTJSONTypeWriterTime");

    static Nymph::KTTIRegistrar< KTJSONTypeWriter, KTJSONTypeWriterTime > sJSONTWERegistrar;

    KTJSONTypeWriterTime::KTJSONTypeWriterTime() :
            KTJSONTypeWriter()
            //KTTypeWriterTime()
    {
    }

    KTJSONTypeWriterTime::~KTJSONTypeWriterTime()
    {
    }


    void KTJSONTypeWriterTime::RegisterSlots()
    {
        fWriter->RegisterSlot("header", this, &KTJSONTypeWriterTime::WriteEggHeader);
        fWriter->RegisterSlot("metadata", this, &KTJSONTypeWriterTime::WriteArbitraryMetadata);
        return;
    }


    void KTJSONTypeWriterTime::WriteArbitraryMetadata(Nymph::KTDataPtr data)
    {

        using rapidjson::SizeType;

        if (! fWriter->OpenAndVerifyFile()) return;

        if (! data) return;

        scarab::param* metadata = data->Of< KTArbitraryMetadata >().GetMetadata();

        KTJSONWriter::JSONMaker* jsonMaker = fWriter->GetJSONMaker();

        jsonMaker->String("run-id");
        jsonMaker->String(metadata->as_node()["run-id"]().as_string().c_str());

        jsonMaker->String("run-parameters");
        jsonMaker->StartObject();
        jsonMaker->String("run-type");
        jsonMaker->String(metadata->as_node()["run-parameters"]["run-type"]().as_string().c_str());
        jsonMaker->String("simulation-type");
        jsonMaker->String(metadata->as_node()["run-parameters"]["simulation-type"]().as_string().c_str());
        jsonMaker->String("simulation-subtype");
        jsonMaker->String(metadata->as_node()["run-parameters"]["simulation-subtype"]().as_string().c_str());
        jsonMaker->String("user-defined-tag");
        jsonMaker->String(metadata->as_node()["run-parameters"]["user-defined-tag"]().as_string().c_str());
        jsonMaker->String("sampling-freq-mega-hz");
        jsonMaker->String(metadata->as_node()["run-parameters"]["sampling-freq-mega-hz"]().as_string().c_str());
        jsonMaker->String("configured-e-min");
        jsonMaker->String(metadata->as_node()["run-parameters"]["configured-e-min"]().as_string().c_str());
        jsonMaker->String("configured-pitch-min");
        jsonMaker->String(metadata->as_node()["run-parameters"]["configured-pitch-min"]().as_string().c_str());
        jsonMaker->String("configured-x-min");
        jsonMaker->String(metadata->as_node()["run-parameters"]["configured-x-min"]().as_string().c_str());
        jsonMaker->String("configured-y-min");
        jsonMaker->String(metadata->as_node()["run-parameters"]["configured-y-min"]().as_string().c_str());
        jsonMaker->String("configured-z-min");
        jsonMaker->String(metadata->as_node()["run-parameters"]["configured-z-min"]().as_string().c_str());
        jsonMaker->EndObject();

        jsonMaker->String("nevents");
        jsonMaker->String(metadata->as_node()["nevents"]().as_string().c_str());

        for (int iEvent = 0; iEvent < metadata->as_node()["nevents"]().as_int(); iEvent++)
        {
            jsonMaker->String(std::to_string(iEvent).c_str());
            jsonMaker->StartObject();
            jsonMaker->String("event-tag");
            jsonMaker->String(metadata->as_node()[std::to_string(iEvent)]["event-tag"]().as_string().c_str());
            jsonMaker->String("kassiopeia-seed");
            jsonMaker->String(metadata->as_node()[std::to_string(iEvent)]["kassiopeia-seed"]().as_string().c_str());
            jsonMaker->String("track-length-seed");
            jsonMaker->String(metadata->as_node()[std::to_string(iEvent)]["track-length-seed"]().as_string().c_str());
            jsonMaker->String("track-delay-seed");
            jsonMaker->String(metadata->as_node()[std::to_string(iEvent)]["track-delay-seed"]().as_string().c_str());
            jsonMaker->String("ntracks");
            jsonMaker->String(metadata->as_node()[std::to_string(iEvent)]["ntracks"]().as_string().c_str());
            for (int iTrack = 0; iTrack < metadata->as_node()[std::to_string(iEvent)]["ntracks"]().as_int(); iTrack++)
            {
                jsonMaker->String(std::to_string(iTrack).c_str());
                jsonMaker->StartObject();
                jsonMaker->String("start-time");
                jsonMaker->Double(metadata->as_node()[std::to_string(iEvent)][std::to_string(iTrack)]["start-time"]().as_double());
                jsonMaker->String("end-time");
                jsonMaker->Double(metadata->as_node()[std::to_string(iEvent)][std::to_string(iTrack)]["end-time"]().as_double());
                jsonMaker->String("energy-ev");
                jsonMaker->Double(metadata->as_node()[std::to_string(iEvent)][std::to_string(iTrack)]["energy-ev"]().as_double());
                jsonMaker->String("start-radius");
                jsonMaker->Double(metadata->as_node()[std::to_string(iEvent)][std::to_string(iTrack)]["start-radius"]().as_double());
                jsonMaker->String("start-radial-phase");
                jsonMaker->Double(metadata->as_node()[std::to_string(iEvent)][std::to_string(iTrack)]["start-radial-phase"]().as_double());
                jsonMaker->String("output-avg-frequency");
                jsonMaker->Double(metadata->as_node()[std::to_string(iEvent)][std::to_string(iTrack)]["output-avg-frequency"]().as_double());
                jsonMaker->String("output-track-start-frequency");
                jsonMaker->Double(metadata->as_node()[std::to_string(iEvent)][std::to_string(iTrack)]["output-track-start-frequency"]().as_double());
                jsonMaker->String("output-inst-start-frequency");
                jsonMaker->Double(metadata->as_node()[std::to_string(iEvent)][std::to_string(iTrack)]["output-inst-start-frequency"]().as_double());
                jsonMaker->String("pitch-angle");
                jsonMaker->Double(metadata->as_node()[std::to_string(iEvent)][std::to_string(iTrack)]["pitch-angle"]().as_double());
                jsonMaker->String("slope");
                jsonMaker->Double(metadata->as_node()[std::to_string(iEvent)][std::to_string(iTrack)]["slope"]().as_double());
                jsonMaker->String("avg-axial-frequency");
                jsonMaker->Double(metadata->as_node()[std::to_string(iEvent)][std::to_string(iTrack)]["avg-axial-frequency"]().as_double());

                jsonMaker->EndObject();


            }

            jsonMaker->EndObject();
        }



//        fWriter->CloseFile();

        return;
    }


    //***********
    // Egg Header
    //***********

    void KTJSONTypeWriterTime::WriteEggHeader(Nymph::KTDataPtr headerPtr)
    {
        using rapidjson::SizeType;

        if (! fWriter->OpenAndVerifyFile()) return;

        KTEggHeader& header = headerPtr->Of< KTEggHeader >();

        KTJSONWriter::JSONMaker* jsonMaker = fWriter->GetJSONMaker();

        jsonMaker->String("egg-header");
        jsonMaker->StartObject();

        jsonMaker->String("filename");
        jsonMaker->String(header.GetFilename().c_str(), (SizeType)header.GetFilename().length());

        jsonMaker->String("acquisition-mode");
        jsonMaker->Uint(header.GetAcquisitionMode());

        jsonMaker->String("n-channels");
        jsonMaker->Uint(header.GetNChannels());

        jsonMaker->String("record-size");
        jsonMaker->Uint((unsigned)header.GetChannelHeader(0)->GetRecordSize());

        jsonMaker->String("raw-slice-size");
        jsonMaker->Uint((unsigned)header.GetChannelHeader(0)->GetSliceSize());

        jsonMaker->String("slice-size");
        jsonMaker->Uint((unsigned)header.GetChannelHeader(0)->GetSliceSize());

        jsonMaker->String("slice-stride");
        jsonMaker->Uint((unsigned)header.GetChannelHeader(0)->GetSliceStride());

        jsonMaker->String("monarch-record-size");
        jsonMaker->Uint((unsigned)header.GetChannelHeader(0)->GetRecordSize());

        jsonMaker->String("run-duration");
        jsonMaker->Uint(header.GetRunDuration());

        jsonMaker->String("acquisition-rate");
        jsonMaker->Double(header.GetAcquisitionRate());

        jsonMaker->String("timestamp");
        jsonMaker->String(header.GetTimestamp().c_str(), (SizeType)header.GetTimestamp().length());

        jsonMaker->String("description");
        jsonMaker->String(header.GetDescription().c_str(), (SizeType)header.GetDescription().length());

        jsonMaker->String("data-type-size");
        jsonMaker->Uint(header.GetChannelHeader(0)->GetDataTypeSize());

        jsonMaker->String("bit-depth");
        jsonMaker->Uint(header.GetChannelHeader(0)->GetBitDepth());

        jsonMaker->String("voltage-offset");
        jsonMaker->Double(header.GetChannelHeader(0)->GetVoltageOffset());

        jsonMaker->String("voltage-range");
        jsonMaker->Double(header.GetChannelHeader(0)->GetVoltageRange());

        jsonMaker->EndObject();

        return;
    }

} /* namespace Katydid */
