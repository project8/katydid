/*
 * KTJSONTypeWriterEgg.cc
 *
 *  Created on: Jan 4, 2013
 *      Author: nsoblath
 */

#include "KTJSONTypeWriterEgg.hh"

#include "KTEggHeader.hh"
#include "KTTIFactory.hh"
//#include "KTLogger.hh"

using std::string;

namespace Katydid
{
    //KTLOGGER(publog, "katydid.output");

    static KTDerivedTIRegistrar< KTJSONTypeWriter, KTJSONTypeWriterEgg > sJSONTWERegistrar;

    KTJSONTypeWriterEgg::KTJSONTypeWriterEgg() :
            KTJSONTypeWriter()
            //KTTypeWriterEgg()
    {
    }

    KTJSONTypeWriterEgg::~KTJSONTypeWriterEgg()
    {
    }


    void KTJSONTypeWriterEgg::RegisterSlots()
    {
        fWriter->RegisterSlot("header", this, &KTJSONTypeWriterEgg::WriteEggHeader);
        return;
    }


    //***********
    // Egg Header
    //***********

    void KTJSONTypeWriterEgg::WriteEggHeader(const KTEggHeader* header)
    {
        using rapidjson::SizeType;

        if (! fWriter->OpenAndVerifyFile()) return;

        KTJSONWriter::JSONMaker* jsonMaker = fWriter->GetJSONMaker();

        jsonMaker->String("egg-header");
        jsonMaker->StartObject();

        jsonMaker->String("filename");
        jsonMaker->String(header->GetFilename().c_str(), (SizeType)header->GetFilename().length());

        jsonMaker->String("acquisition-mode");
        jsonMaker->Uint(header->GetAcquisitionMode());

        jsonMaker->String("n-channels");
        jsonMaker->Uint(header->GetNChannels());

        jsonMaker->String("record-size");
        jsonMaker->Uint((UInt_t)header->GetSliceSize());

        jsonMaker->String("monarch-record-size");
        jsonMaker->Uint((UInt_t)header->GetRecordSize());

        jsonMaker->String("run-duration");
        jsonMaker->Uint(header->GetRunDuration());

        jsonMaker->String("acquisition-rate");
        jsonMaker->Double(header->GetAcquisitionRate());

        jsonMaker->String("timestamp");
        jsonMaker->String(header->GetTimestamp().c_str(), (SizeType)header->GetTimestamp().length());

        jsonMaker->String("description");
        jsonMaker->String(header->GetDescription().c_str(), (SizeType)header->GetDescription().length());

        jsonMaker->String("run-type");
        jsonMaker->Uint(header->GetRunType());

        jsonMaker->String("run-source");
        jsonMaker->Uint(header->GetRunSource());

        jsonMaker->String("format-mode");
        jsonMaker->Uint(header->GetFormatMode());

        jsonMaker->EndObject();

        return;
    }

} /* namespace Katydid */
