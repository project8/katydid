/*
 * KTEggWriter.cc
 *
 *  Created on: May 29, 2013
 *      Author: nsoblath
 */

#include "KTEggWriter.hh"

#include "KTEggHeader.hh"
#include "KTNOFactory.hh"
#include "KTLogger.hh"
#include "KTPStoreNode.hh"
#include "KTTimeSeriesData.hh"

#include "Monarch.hpp"
#include "MonarchException.hpp"
#include "MonarchHeader.hpp"
#include "MonarchRecord.hpp"

#include <cmath>

using std::string;

namespace Katydid
{
    KTLOGGER(eggwritelog, "katydid.output");

    static KTDerivedNORegistrar< KTWriter, KTEggWriter > sEWriterRegistrar("egg-writer");
    static KTDerivedNORegistrar< KTProcessor, KTEggWriter > sEWProcRegistrar("egg-writer");


    KTEggWriter::KTEggWriter(const std::string& name) :
            KTWriter(name),
            fFilename("output.egg"),
            fFormatMode(sFormatMultiInterleaved),
            fDigitizerFullscale(1.),
            fFileStatus(kClosed),
            fExpectedNChannels(2),
            fExpectedRecordSize(17),
            fEggFile(NULL),
            fHeaderSlot("header", this, &KTEggWriter::WriteHeader),
            fTimeSeriesSlot("ts", this, &KTEggWriter::WriteTSData),
            fDoneSlot("done", this, &KTEggWriter::CloseFile)
    {
    }

    KTEggWriter::~KTEggWriter()
    {
        CloseFile();
        delete fEggFile;
    }

    Bool_t KTEggWriter::Configure(const KTPStoreNode* node)
    {
        if (node == NULL) return false;

        SetFilename(node->GetData<string>("output-file", fFilename));

        if (node->HasData("format-mode"))
        {
            string modeStr(node->GetData<string>("format-mode"));
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
                KTERROR(eggwritelog, "Invalid format mode: <" << modeStr << ">");
                return false;
            }
        }

        SetDigitizerFullscale(node->GetData<Double_t>("digitizer-fullscale", fDigitizerFullscale));

        return true;
    }

    Bool_t KTEggWriter::OpenFile()
    {
        if (fFileStatus != kClosed)
        {
            KTERROR(eggwritelog, "File status must be <" << kClosed << ">, but it is currently <" << fFileStatus << ">");
            return false;
        }

        delete fEggFile;
        fEggFile = NULL;

        try
        {
            fEggFile = Monarch::OpenForWriting(fFilename.c_str());
        }
        catch (MonarchException& e)
        {
            KTERROR(eggwritelog, "Problem opening the egg file: " << e.what());
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
            fEggFile->Close();
        }
        catch (MonarchException& e)
        {
            KTERROR(eggwritelog, "Problem occurred while closing file: " << e.what());
            return;
        }

        delete fEggFile;
        fEggFile = NULL;

        fFileStatus = kClosed;

        return;
    }

    void KTEggWriter::WriteHeader(const KTEggHeader* header)
    {
        if (fFileStatus == kClosed)
        {
            if (OpenFile())
                fFileStatus = kOpened;
        }

        if (fFileStatus != kOpened)
        {
            KTERROR(eggwritelog, "Cannot write header. File status must be <" << kOpened <<">; currently it is <" << fFileStatus <<">");
            return;
        }

        if (header == NULL)
        {
            KTERROR(eggwritelog, "Header object is NULL; no header written");
            return;
        }

        fExpectedNChannels = header->GetAcquisitionMode();
        fExpectedRecordSize = header->GetRecordSize();

        MonarchHeader* monarchHeader = fEggFile->GetHeader();

        monarchHeader->SetFilename(fFilename);
        monarchHeader->SetAcquisitionMode(fExpectedNChannels);
        monarchHeader->SetRecordSize(fExpectedRecordSize);
        monarchHeader->SetRunDuration(header->GetRunDuration());
        monarchHeader->SetAcquisitionRate(header->GetAcquisitionRate() * 1.e-6); // TODO: remove this 1e-6 when switch to MHz is made
        monarchHeader->SetTimestamp(header->GetTimestamp());
        monarchHeader->SetDescription(header->GetDescription());
        monarchHeader->SetRunType(header->GetRunType());
        monarchHeader->SetRunSource(header->GetRunSource());
        if (fExpectedNChannels == 1)
        {
            monarchHeader->SetFormatMode(sFormatSingle);
        }
        else
        {
            monarchHeader->SetFormatMode(fFormatMode);
        }

        try
        {
            fEggFile->WriteHeader();
        }
        catch (MonarchException& e)
        {
            KTERROR(eggwritelog, "Unable to write header to file: " << e.what());
            return;
        }

        fFileStatus = kHeaderWritten;

        return;
    }

    Bool_t KTEggWriter::WriteTSData(KTTimeSeriesData& tsData)
    {
        if (fFileStatus == kClosed)
        {
            KTERROR(eggwritelog, "Cannot write record because the file has not been opened");
            return false;
        }

        if (fFileStatus == kOpened)
        {
            KTWARN(eggwritelog, "Writing record on file with no header");
        }
        fFileStatus = kWritingRecords;

        UInt_t nComponents = tsData.GetNComponents();
        if (nComponents != fExpectedNChannels)
        {
            KTERROR(eggwritelog, "Received data contains " << nComponents << " channels of data; " << fExpectedNChannels << " were expected");
            return false;
        }

        if (fFormatMode == sFormatMultiInterleaved) return WriteTSDataInterleaved(tsData);
        else if (fFormatMode == sFormatMultiSeparate || fFormatMode == sFormatSingle) return WriteTSDataSeparate(tsData);
        else
        {
            KTERROR(eggwritelog, "Unable to write records in mode <" << fFormatMode << ">");
            return false;
        }
    }

    Bool_t KTEggWriter::WriteTSDataInterleaved(KTTimeSeriesData& tsData)
    {
        if (fExpectedNChannels != 2)
        {
            KTERROR(eggwritelog, "Interleaved record writing is only supported for 2 channels, not " << fExpectedNChannels);
            return false;
        }

        KTTimeSeries* ts0 = tsData.GetTimeSeries(0);
        KTTimeSeries* ts1 = tsData.GetTimeSeries(1);

        if (ts0->GetNTimeBins() != fExpectedRecordSize || ts1->GetNTimeBins() != fExpectedRecordSize)
        {
            KTERROR(eggwritelog, "Time series do not have the correct size (should be " << fExpectedRecordSize << "; provided component 0: " << ts0->GetNTimeBins() << "; provided component 1: " << ts1->GetNTimeBins());
            return false;
        }

        DataType* dataPtr = fEggFile->GetRecordInterleaved()->fData;

        Double_t value0, value1;
        Double_t scale = 255. / fDigitizerFullscale;
        for (UInt_t iBin = 0; iBin < fExpectedRecordSize; iBin++)
        {
            /*
            value0 = ts0->GetValue(iBin) * scale;
            value1 = ts1->GetValue(iBin) * scale;
            if (value0 >= 256) value0 = 255.;
            if (value1 >= 256) value1 = 255.;
            if (value0 < 0.) value0 = 0.;
            if (value1 < 0.) value1 = 0.;
            dataPtr[2 * iBin] = DataType(floor(value0));
            dataPtr[2 * iBin + 1] = DataType(floor(value1));
            */
            dataPtr[2 * iBin] = (DataType)1.;
            dataPtr[2 * iBin + 1] = (DataType)50.;
        }

        try
        {
            fEggFile->WriteRecord();
        }
        catch (MonarchException& e)
        {
            KTERROR(eggwritelog, "A problem occurred while writing the record (interleaved): " << e.what());
            return false;
        }

        return true;
    }

    Bool_t KTEggWriter::WriteTSDataSeparate(KTTimeSeriesData& tsData)
    {
        if (fExpectedNChannels > 2)
        {
            KTERROR(eggwritelog, "Interleaved record writing is only supported for 2 or fewer channels, not " << fExpectedNChannels);
            return false;
        }

        KTTimeSeries* ts = tsData.GetTimeSeries(0);

        if (ts->GetNTimeBins() != fExpectedRecordSize)
        {
            KTERROR(eggwritelog, "Time series does not have the correct size (should be " << fExpectedRecordSize << "; provided component 0: " << ts->GetNTimeBins());
            return false;
        }

        DataType* dataPtr = fEggFile->GetRecordSeparateOne()->fData;
        Double_t value;
        for (UInt_t iBin = 0; iBin < fExpectedRecordSize; iBin++)
        {
            value = ts->GetValue(iBin) / fDigitizerFullscale;
            if (value >= 256) value = 255.;
            if (value < 0.) value = 0.;
            dataPtr[iBin] = UInt_t(floor(value));
        }

        if (fExpectedNChannels == 2)
        {
            ts = tsData.GetTimeSeries(1);

            if (ts->GetNTimeBins() != fExpectedRecordSize)
            {
                KTERROR(eggwritelog, "Time series does not have the correct size (should be " << fExpectedRecordSize << "; provided component 1: " << ts->GetNTimeBins());
                return false;
            }

            dataPtr = fEggFile->GetRecordSeparateOne()->fData;
            for (UInt_t iBin = 0; iBin < fExpectedRecordSize; iBin++)
            {
                value = ts->GetValue(iBin) / fDigitizerFullscale;
                if (value >= 256) value = 255.;
                if (value < 0.) value = 0.;
                dataPtr[iBin] = UInt_t(floor(value));
            }
        }

        try
        {
            fEggFile->WriteRecord();
        }
        catch (MonarchException& e)
        {
            KTERROR(eggwritelog, "A problem occurred while writing the record (interleaved): " << e.what());
            return false;
        }

        return true;
    }

} /* namespace Katydid */
