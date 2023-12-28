/*
 * KTJSONTypeWriterTime.cc
 *
 *  Created on: Jan 4, 2013
 *      Author: nsoblath
 */

#include "KTEggHeader.hh"
#include "KTJSONTypeWriterTime.hh"
#include "KTTIFactory.hh"
//#include "logger.hh"

using std::string;

namespace Katydid
{
    //LOGGER(publog, "KTJSONTypeWriterTime");

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
