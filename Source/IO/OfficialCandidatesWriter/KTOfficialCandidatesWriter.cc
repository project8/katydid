/*
 * KTOfficialCandidatesWriter.cc
 *
 *  Created on: Apr 23, 2013
 *      Author: nsoblath
 */

#include "KTOfficialCandidatesWriter.hh"

#include "KTEggHeader.hh"
#include "KTProcSummary.hh"
#include "param.hh"
#include "KTWaterfallCandidateData.hh"

using std::string;

namespace Katydid
{
    LOGGER(publog, "KTOfficialCandidatesWriter");

    KT_REGISTER_WRITER(KTOfficialCandidatesWriter, "official-candidates-writer");
    KT_REGISTER_PROCESSOR(KTOfficialCandidatesWriter, "official-candidates-writer");

    KTOfficialCandidatesWriter::KTOfficialCandidatesWriter(const std::string& name) :
            KTWriter(name),
            fFilename("basic_output.json"),
            fFileMode("w+"),
            fPrettyJSONFlag(true),
            fFile(NULL),
            fFileStream(NULL),
            fJSONMaker(NULL),
            fStatus(kNotOpenedYet),
            fSummaryCopy(NULL),
            fWaitingForSummary(false),
            fMutex(),
            fHeaderSlot("header", this, &KTOfficialCandidatesWriter::WriteHeaderInformation),
            fWaterfallCandidateSlot("waterfall-candidate", this, &KTOfficialCandidatesWriter::WriteWaterfallCandidate),
            fStopWritingSlot("stop", this, &KTOfficialCandidatesWriter::CloseFile),
            fSummarySlot("summary", this, &KTOfficialCandidatesWriter::WriteSummaryInformationAndCloseFile),
            fStopCloseAfterSummarySlot("stop-close-after-summary", this, &KTOfficialCandidatesWriter::StopCandidatesAndWaitForSummary),
            fSummaryAfterStopSlot("summary-after-stop", this, &KTOfficialCandidatesWriter::CopySummaryInformationAndWaitForStop)
    {
    }

    KTOfficialCandidatesWriter::~KTOfficialCandidatesWriter()
    {
        CloseFile();
        delete fSummaryCopy;
    }

    bool KTOfficialCandidatesWriter::Configure(const scarab::param_node* node)
    {
        // Config-file settings
        if (node != NULL)
        {
            SetFilename(node->get_value("output-file", fFilename));
            SetFileMode(node->get_value("file-mode", fFileMode));
            SetPrettyJSONFlag(node->get_value<bool>("pretty-json", fPrettyJSONFlag));
        }

        return true;
    }

    bool KTOfficialCandidatesWriter::OpenFile()
    {
        ScopedLock lock(fMutex);

        if (fStatus != kNotOpenedYet)
        {
            LERROR(publog, "Status must be <" << kNotOpenedYet << "> to open a file; current status is <" << fStatus << ">");
            return false;
        }

        char tBuffer[ RAPIDJSON_FILE_BUFFER_SIZE ];
        if (fFilename == "stdout")
        {
            fFileStream = new rapidjson::FileWriteStream(stdout, tBuffer, sizeof(tBuffer));
        }
        else
        {
            fFile = fopen(fFilename.c_str(), fFileMode.c_str());
            if (fFile == NULL)
            {
                LERROR(publog, "File did not open\n" <<
                        "\tFilename: " << fFilename <<
                        "\tMode: " << fFileMode);
                return false;
            }
            fFileStream = new rapidjson::FileWriteStream(fFile, tBuffer, sizeof(tBuffer));
        }

        if (fPrettyJSONFlag)
        {
            fJSONMaker = new KTJSONMakerPretty< rapidjson::FileWriteStream >(*fFileStream);
        }
        else
        {
            fJSONMaker = new KTJSONMakerCompact< rapidjson::FileWriteStream  >(*fFileStream);
        }

        fJSONMaker->StartObject();

        fStatus = kPriorToCandidates;

        return true;
    }

    void KTOfficialCandidatesWriter::CloseFile()
    {
        ScopedLock lock(fMutex);

        EndCandidates();

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

    bool KTOfficialCandidatesWriter::OpenAndVerifyFile()
    {
        ScopedLock lock(fMutex);

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

    void KTOfficialCandidatesWriter::WriteHeaderInformation(KTEggHeader* header)
    {
        using rapidjson::SizeType;

        ScopedLock lock(fMutex);

        if (! OpenAndVerifyFile()) return;

        if (fStatus != kPriorToCandidates) return;

        fJSONMaker->String("eggfile");
        fJSONMaker->String(header->GetFilename().c_str(), (SizeType)header->GetFilename().length());

        fJSONMaker->String("record_size");
        fJSONMaker->Uint((unsigned)header->GetChannelHeader(0)->GetRecordSize());

        return;
    }

    bool KTOfficialCandidatesWriter::WriteWaterfallCandidate(KTWaterfallCandidateData& wcData)
    {
        using rapidjson::SizeType;

        ScopedLock lock(fMutex);

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

    void KTOfficialCandidatesWriter::WriteSummaryInformation(const KTProcSummary* summary)
    {
        ScopedLock lock(fMutex);

        EndCandidates();

        fJSONMaker->String("records_analyzed");
        fJSONMaker->Uint(summary->GetNRecordsProcessed());

        return;
    }

    void KTOfficialCandidatesWriter::WriteSummaryInformationAndCloseFile(const KTProcSummary* summary)
    {
        ScopedLock lock(fMutex);

        WriteSummaryInformation(summary);

        CloseFile();

        return;
    }

    void KTOfficialCandidatesWriter::CopySummaryInformationAndWaitForStop(const KTProcSummary* summary)
    {
        ScopedLock lock(fMutex);

        LDEBUG(publog, "Copying summary information before checking if already waiting to stop candidate writing");

        delete fSummaryCopy;
        fSummaryCopy = new KTProcSummary(*summary);
        // having non-null summary indicates that the writer is waiting for the stop-and-wait signal

        if (fWaitingForSummary)
        {
            LDEBUG(publog, "Writer was waiting for summary; writing summary and closing file");
            WriteSummaryInformationAndCloseFile(fSummaryCopy);
            delete fSummaryCopy;
            fSummaryCopy = NULL;
            fWaitingForSummary = false;
        }
        else
        {
            LDEBUG(publog, "Commencing wait for end of candidate writing");
        }

        return;
    }

    void KTOfficialCandidatesWriter::StopCandidatesAndWaitForSummary()
    {
        ScopedLock lock(fMutex);

        LDEBUG(publog, "Stopping candidate writing before checking if already waiting with summary");

        EndCandidates();
        fWaitingForSummary = true;

        if (fSummaryCopy != NULL)
        {
            LDEBUG(publog, "Writer was waiting to stop candidate writing; stopping, writing summary, and closing file");
            WriteSummaryInformationAndCloseFile(fSummaryCopy);
            delete fSummaryCopy;
            fSummaryCopy = NULL;
            fWaitingForSummary = false;
        }
        else
        {
            LDEBUG(publog, "Commencing wait for summary");
        }

        return;
    }

    void KTOfficialCandidatesWriter::EndCandidates()
    {
        ScopedLock lock(fMutex);

        if (fStatus == kPriorToCandidates)
        {
            fJSONMaker->String("candidates");
            fJSONMaker->StartArray();
            fJSONMaker->EndArray();
        }
        if (fStatus == kWritingCandidates)
        {
            // end the array of candidates
            fJSONMaker->EndArray();
        }

        fStatus = kStopped;

        return;
    }



} /* namespace Katydid */
