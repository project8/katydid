/*
 * KTEggWriter.cc
 *
 *  Created on: May 29, 2013
 *      Author: nsoblath
 */

#include "KTEggWriter.hh"

#include "KTEggHeader.hh"
#include "param.hh"
#include "KTSliceHeader.hh"
#include "KTTimeSeriesData.hh"

#include "Monarch.hpp"
#include "MonarchException.hpp"
#include "MonarchHeader.hpp"
#include "MonarchRecord.hpp"

#include <cmath>

using monarch::Monarch;
using monarch::MonarchException;
using monarch::MonarchHeader;

using std::string;

namespace Katydid
{
    LOGGER(eggwritelog, "KTEggWriter");

    KT_REGISTER_WRITER(KTEggWriter, "egg-writer");
    KT_REGISTER_PROCESSOR(KTEggWriter, "egg-writer");


    KTEggWriter::KTEggWriter(const std::string& name) :
            KTWriter(name),
            fFilename("output.egg"),
            fFormatMode(monarch::sFormatMultiInterleaved),
            fDigitizerFullscale(1.),
            fFileStatus(kClosed),
            fExpectedNChannels(2),
            fExpectedRecordSize(17),
            fMonarch(NULL),
            fHeaderSlot("header", this, &KTEggWriter::WriteHeader),
            fTimeSeriesSlot("ts", this, &KTEggWriter::WriteTSData),
            fDoneSlot("done", this, &KTEggWriter::CloseFile)
    {
    }

    KTEggWriter::~KTEggWriter()
    {
        CloseFile();
        delete fMonarch;
    }

    bool KTEggWriter::Configure(const scarab::param_node& node)
    {
        SetFilename(node.get_value("output-file", fFilename));

        if (node.has("format-mode"))
        {
            string modeStr(node.get_value("format-mode"));
            if (modeStr == "separate")
            {
                SetFormatMode(sFormatMultiSeparate);
            }
            else if (modeStr == "interleaved")
            {
                SetFormatMode(sFormatMultiInterleaved);
            }
            else
            {
                LERROR(eggwritelog, "Invalid format mode: <" << modeStr << ">");
                return false;
            }
        }

        SetDigitizerFullscale(node.get_value("digitizer-fullscale", fDigitizerFullscale));

        return true;
    }

    bool KTEggWriter::OpenFile()
    {
        if (fFileStatus != kClosed)
        {
            LERROR(eggwritelog, "File status must be <" << kClosed << ">, but it is currently <" << fFileStatus << ">");
            return false;
        }

        delete fMonarch;
        fMonarch = NULL;

        try
        {
            fMonarch = Monarch::OpenForWriting(fFilename.c_str());
        }
        catch (MonarchException& e)
        {
            LERROR(eggwritelog, "Problem opening the egg file: " << e.what());
            return false;
        }

        fFileStatus = kOpened;
        return true;
    }

    void KTEggWriter::CloseFile()
    {
        if (fFileStatus == kClosed) return;

        try
        {
            fMonarch->Close();
        }
        catch (MonarchException& e)
        {
            LERROR(eggwritelog, "Problem occurred while closing file: " << e.what());
            return;
        }

        delete fMonarch;
        fMonarch = NULL;

        fFileStatus = kClosed;

        return;
    }

    void KTEggWriter::WriteHeader(KTEggHeader* header)
    {
        if (fFileStatus == kClosed)
        {
            if (OpenFile())
                fFileStatus = kOpened;
        }

        if (fFileStatus != kOpened)
        {
            LERROR(eggwritelog, "Cannot write header. File status must be <" << kOpened <<">; currently it is <" << fFileStatus <<">");
            return;
        }

        if (header == NULL)
        {
            LERROR(eggwritelog, "Header object is NULL; no header written");
            return;
        }

        fExpectedNChannels = header->GetAcquisitionMode();
        fExpectedRecordSize = header->GetRecordSize();

        MonarchHeader* monarchHeader = fMonarch->GetHeader();

        monarchHeader->SetFilename(fFilename);
        monarchHeader->SetAcquisitionMode(fExpectedNChannels);
        monarchHeader->SetRecordSize(fExpectedRecordSize);
        monarchHeader->SetRunDuration(header->GetRunDuration()); // in ms
        monarchHeader->SetAcquisitionRate(header->GetAcquisitionRate() * 1.e-6); // TODO: remove this 1e-6 when switch to MHz is made
        monarchHeader->SetTimestamp(header->GetTimestamp());
        monarchHeader->SetDescription(header->GetDescription());
        monarchHeader->SetRunType(header->GetRunType());
        monarchHeader->SetRunSource(header->GetRunSource());
        if (fExpectedNChannels == 1)
        {
            monarchHeader->SetFormatMode(monarch::sFormatSingle);
        }
        else
        {
            monarchHeader->SetFormatMode(fFormatMode);
        }

        try
        {
            fMonarch->WriteHeader();
        }
        catch (MonarchException& e)
        {
            LERROR(eggwritelog, "Unable to write header to file: " << e.what());
            return;
        }

        fFileStatus = kHeaderWritten;

        return;
    }

    bool KTEggWriter::WriteTSData(KTSliceHeader& slHeader, KTTimeSeriesData& tsData)
    {
        if (fFileStatus == kClosed)
        {
            LERROR(eggwritelog, "Cannot write record because the file has not been opened");
            return false;
        }

        if (fFileStatus == kOpened)
        {
            LWARN(eggwritelog, "Writing record on file with no header");
        }
        fFileStatus = kWritingRecords;

        unsigned nComponents = tsData.GetNComponents();
        if (nComponents != fExpectedNChannels)
        {
            LERROR(eggwritelog, "Received data contains " << nComponents << " channels of data; " << fExpectedNChannels << " were expected");
            return false;
        }

        if (fExpectedNChannels > 2)
        {
            LERROR(eggwritelog, "Interleaved record writing is only supported for 2 or fewer channels, not " << fExpectedNChannels);
            return false;
        }

        fMonarch->SetInterface(monarch::sInterfaceSeparate);

        CopyATimeSeries(0, slHeader, tsData, fMonarch->GetRecordSeparateOne());

        if (fExpectedNChannels == 2)
        {
            CopyATimeSeries(1, slHeader, tsData, fMonarch->GetRecordSeparateTwo());
        }

        try
        {
            fMonarch->WriteRecord();
        }
        catch (MonarchException& e)
        {
            LERROR(eggwritelog, "A problem occurred while writing the record (interleaved): " << e.what());
            return false;
        }
        return true;
    }

    bool KTEggWriter::CopyATimeSeries(unsigned component, const KTSliceHeader& slHeader, const KTTimeSeriesData& tsData, MonarchRecord* record)
    {
        const KTTimeSeries* ts = tsData.GetTimeSeries(component);
        if (ts->GetNTimeBins() != fExpectedRecordSize)
        {
            LERROR(eggwritelog, "Time series does not have the correct size (should be " << fExpectedRecordSize << "; provided component 0: " << ts->GetNTimeBins());
            return false;
        }

        record->fAcquisitionId = slHeader.GetAcquisitionID(component);
        record->fRecordId = slHeader.GetRecordID(component);
        record->fTime = slHeader.GetTimeStamp(component);

        double value;
        double scale = 255. / fDigitizerFullscale;
        for (unsigned iBin = 0; iBin < fExpectedRecordSize; iBin++)
        {
            value = ts->GetValue(iBin) * scale;
            if (value >= 256) value = 255.;
            if (value < 0.) value = 0.;
            record->fData[iBin] = DataType(floor(value));
        }
        return true;
    }

} /* namespace Katydid */
