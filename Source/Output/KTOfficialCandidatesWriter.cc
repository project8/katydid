/*
 * KTOfficialCandidatesWriter.cc
 *
 *  Created on: Apr 23, 2013
 *      Author: nsoblath
 */

#include "KTOfficialCandidatesWriter.hh"

#include "KTEggHeader.hh"
#include "KTFactory.hh"
#include "KTLogger.hh"
#include "KTPStoreNode.hh"
#include "KTWaterfallCandidateData.hh"

using std::string;

namespace Katydid
{
    KTLOGGER(publog, "katydid.output");

    static KTDerivedRegistrar< KTWriter, KTOfficialCandidatesWriter > sOCWriterRegistrar("official-candidates-writer");
    static KTDerivedRegistrar< KTProcessor, KTOfficialCandidatesWriter > sOCWProcRegistrar("official-candidates-writer");

    KTOfficialCandidatesWriter::KTOfficialCandidatesWriter(const std::string& name) :
            KTWriter(name),
            fFilename("basic_output.json"),
            fFileMode("w+"),
            fPrettyJSONFlag(true),
            fFile(NULL),
            fFileStream(NULL),
            fJSONMaker(NULL),
            fStatus(kNotOpenedYet),
            fHeaderSlot("header", this, &KTOfficialCandidatesWriter::WriteHeaderInformation),
            fWaterfallCandidateSlot("waterfall-candidate", this, &KTOfficialCandidatesWriter::WriteWaterfallCandidate),
            fStopWritingSlot("stop", this, &KTOfficialCandidatesWriter::CloseFile)
    {
    }

    KTOfficialCandidatesWriter::~KTOfficialCandidatesWriter()
    {
        CloseFile();
    }

    Bool_t KTOfficialCandidatesWriter::Configure(const KTPStoreNode* node)
    {
        // Config-file settings
        if (node != NULL)
        {
            SetFilename(node->GetData<string>("output-file", fFilename));
            SetFileMode(node->GetData<string>("file-mode", fFileMode));
            SetPrettyJSONFlag(node->GetData<Bool_t>("pretty-json", fPrettyJSONFlag));
        }

        return true;
    }

    Bool_t KTOfficialCandidatesWriter::OpenFile()
    {
        if (fStatus != kNotOpenedYet)
        {
            KTERROR(publog, "Status must be <" << kNotOpenedYet << "> to open a file; current status is <" << fStatus << ">");
            return false;
        }

        if (fFilename == "stdout")
        {
            fFileStream = new rapidjson::FileStream(stdout);
        }
        else
        {
            fFile = fopen(fFilename.c_str(), fFileMode.c_str());
            if (fFile == NULL)
            {
                KTERROR(publog, "File did not open\n" <<
                        "\tFilename: " << fFilename <<
                        "\tMode: " << fFileMode);
                return false;
            }
            fFileStream = new rapidjson::FileStream(fFile);
        }

        if (fPrettyJSONFlag)
        {
            fJSONMaker = new KTJSONMakerPretty< rapidjson::FileStream >(*fFileStream);
        }
        else
        {
            fJSONMaker = new KTJSONMakerCompact< rapidjson::FileStream >(*fFileStream);
        }

        fJSONMaker->StartObject();

        fStatus = kPriorToCandidates;

        return true;
    }

    void KTOfficialCandidatesWriter::CloseFile()
    {
        if (fStatus == kWritingCandidates)
        {
            // end the array of candidates
            fJSONMaker->EndArray();
        }

        fStatus = kStopped;

        if (fJSONMaker != NULL)
        {
            fJSONMaker->EndObject();
            delete fJSONMaker;
            fJSONMaker = NULL;
        }
        if (fFileStream != NULL)
        {
            delete fFileStream;
            fFileStream = NULL;
        }
        if (fFile != NULL)
        {
            fclose(fFile);
            fFile = NULL;
        }
        return;
    }

    Bool_t KTOfficialCandidatesWriter::OpenAndVerifyFile()
    {
        if (fFileStream == NULL || fJSONMaker == NULL)
        {
            if (! OpenFile())
            {
                return false;
            }
        }

        if (fStatus == kStopped || fStatus == kNotOpenedYet)
        {
            return false;
        }

        return true;
    }

    void KTOfficialCandidatesWriter::WriteHeaderInformation(const KTEggHeader* header)
    {
        using rapidjson::SizeType;

        if (! OpenAndVerifyFile()) return;

        if (fStatus != kPriorToCandidates) return;

        fJSONMaker->String("eggfile");
        fJSONMaker->String(header->GetFilename().c_str(), (SizeType)header->GetFilename().length());

        fJSONMaker->String("record_size");
        fJSONMaker->Uint((UInt_t)header->GetRecordSize());

        return;
    }

    Bool_t KTOfficialCandidatesWriter::WriteWaterfallCandidate(KTWaterfallCandidateData& wcData)
    {
        using rapidjson::SizeType;

        if (! OpenAndVerifyFile()) return false;

        if (fStatus == kPriorToCandidates)
        {
            fStatus = kWritingCandidates;
            fJSONMaker->String("candidates");
            fJSONMaker->StartArray();
        }

        // start the candidate
        fJSONMaker->StartObject();

        fJSONMaker->String("support");
        fJSONMaker->StartArray();
        fJSONMaker->Uint(wcData.GetStartRecordNumber());
        fJSONMaker->Uint(wcData.GetStartSampleNumber());
        fJSONMaker->Uint(wcData.GetEndRecordNumber());
        fJSONMaker->Uint(wcData.GetEndSampleNumber());
        fJSONMaker->EndArray();

        fJSONMaker->EndObject();
        // end the candidate

        return true;
    }


} /* namespace Katydid */
