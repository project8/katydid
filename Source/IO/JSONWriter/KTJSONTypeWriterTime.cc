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
#include "KTLogger.hh"


using std::string;

namespace Katydid
{
    KTLOGGER(publog, "KTJSONTypeWriterTime");

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
        if (! metadata)
        {
            KTWARN(publog, "Can't find any arbitrary metadata to record.");
            return;
        }

        scarab::param_value tFormat;
        if ( fWriter->GetPrettyJSONFlag() )
        {
            tFormat.set( "pretty" );
        }
        else
        {
            tFormat.set( "compact" );
        }
        scarab::param_node* tStyle = new scarab::param_node();
        tStyle->add("style", tFormat);
        KTDEBUG(publog, "Arbitrary metadata output format:\n" << *tStyle);

        // TO-DO:  Propagate file mode (a, w, r, etc.) from config file to the json writer.
        scarab::param_output_json tOutput;
        bool t_did_write_file = tOutput.write_file( *metadata, fWriter->GetFilename(), tStyle);
        delete tStyle;

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
