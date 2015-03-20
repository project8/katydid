/*
 * KTEgg3Reader.cc
 *
 *  Created on: Jan 7, 2015
 *      Author: nsoblath
 */

#include "KTEgg3Reader.hh"

#include "KTEggHeader.hh"
#include "KTEggProcessor.hh"
#include "KTLogger.hh"
#include "KTSliceHeader.hh"
#include "KTRawTimeSeriesData.hh"
#include "KTRawTimeSeries.hh"
#include "KTTimeSeriesData.hh"
#include "KTTimeSeriesFFTW.hh"

#include "M3Monarch.hh"
#include "M3DataInterface.hh"
#include "M3Exception.hh"

using namespace monarch3;

using std::map;
using std::string;
using std::vector;

namespace Katydid
{
    KTLOGGER(eggreadlog, "KTEgg3Reader");

    KT_REGISTER_EGGREADER(KTEgg3Reader, "egg3");

    KTEgg3Reader::KTEgg3Reader() :
            KTEggReader(),
            fSliceSize(1024),
            fStride(0),
            fStartTime(0.),
            fHatchNextSlicePtr(NULL),
            fMonarch(NULL),
            fStream0(NULL),
            fHeaderPtr(new KTData()),
            fHeader(fHeaderPtr->Of< KTEggHeader >()),
            fReadState(),
            fGetTimeInRun(&KTEgg3Reader::GetTimeInRunFirstCall),
            fT0Offset(0),
            fSampleRateUnitsInHz(1.e6),
            fRecordSize(0),
            fBinWidth(0.),
            fSliceNumber(0)
    {
        fReadState.fStatus = MonarchReadState::kInvalid;
        fReadState.fAcquisitionID = 0;
        fReadState.fReadPtrOffset = 0;
        fReadState.fReadPtrRecordOffset = 0;
        fReadState.fSliceStartPtrOffset = 0;
        fReadState.fAbsoluteRecordOffset = 0;
    }

    KTEgg3Reader::~KTEgg3Reader()
    {
        if (fMonarch != NULL)
        {
            delete fMonarch;
        }
    }

    bool KTEgg3Reader::Configure(const KTEggProcessor& eggProc)
    {
        SetSliceSize(eggProc.GetSliceSize());
        SetStride(eggProc.GetStride());
        SetStartTime(eggProc.GetStartTime());
        return true;
    }

    KTDataPtr KTEgg3Reader::BreakEgg(const string& filename)
    {
        if (fStride == 0) fStride = fSliceSize;

        if (fMonarch != NULL)
        {
            delete fMonarch;
        }

        // open the file
        KTINFO(eggreadlog, "Opening egg file <" << filename << ">");
        try
        {
            fMonarch = Monarch3::OpenForReading(filename);
        }
        catch (M3Exception& e)
        {
            KTERROR(eggreadlog, "Unable to break egg: " << e.what());
            return KTDataPtr();

        }

        KTDEBUG(eggreadlog, "File open; reading header");
        try
        {
            fMonarch->ReadHeader();
        }
        catch (M3Exception& e)
        {
            KTERROR(eggreadlog, "Header was not read correctly: " << e.what() << '\n' <<
                    "Egg breaking aborted.");
            delete fMonarch;
            fMonarch = NULL;
            return KTDataPtr();
        }
        CopyHeaderInformation(fMonarch->GetHeader());
        fHeader.SetRawSliceSize(fSliceSize);
        fHeader.SetSliceSize(fSliceSize);
        fHeader.SetSliceStride(fStride);

        KTDEBUG(eggreadlog, "Parsed header:\n" << fHeader);

        const M3StreamHeader& stream0Header = fMonarch->GetHeader()->GetStreamHeaders()[0];
        if (stream0Header.GetDataFormat() == sAnalog && stream0Header.GetSampleSize() == 2)
        {
            // complex, analog data is currently readable
            KTDEBUG(eggreadlog, "Setting hatch-next-slice function to complex, analog");
            fHatchNextSlicePtr = &KTEgg3Reader::HatchNextSliceComplex;
        }
        else if (stream0Header.GetDataFormat() == sDigitizedUS && stream0Header.GetSampleSize() == 1)
        {
            // real, digitized data is currently readable
            KTDEBUG(eggreadlog, "Setting hatch-next-slice function to real, digitized");
            fHatchNextSlicePtr = &KTEgg3Reader::HatchNextSliceRealUnsigned;
        }
        else if (stream0Header.GetDataFormat() == sDigitizedS && stream0Header.GetSampleSize() == 1)
        {
            // real, digitized data is currently readable
            KTDEBUG(eggreadlog, "Setting hatch-next-slice function to real, digitized");
            fHatchNextSlicePtr = &KTEgg3Reader::HatchNextSliceRealSigned;
        }
        else
        {
            KTERROR(eggreadlog, "Cannot read data with data format <" << stream0Header.GetDataFormat() << "> and sample size <" << stream0Header.GetSampleSize() << ">");
            delete fMonarch;
            fMonarch = NULL;
            return KTDataPtr();
        }

        fRecordSize = fHeader.GetRecordSize();
        fBinWidth = 1. / fHeader.GetAcquisitionRate();

        fHeader.SetMinimumFrequency(0.0);
        fHeader.SetMaximumFrequency(fHeader.GetAcquisitionRate()/2.0);
        fHeader.SetCenterFrequency((fHeader.GetMaximumFrequency() - fHeader.GetMinimumFrequency())/2.0);

        // by default, start the read state at the beginning of the run
        fReadState.fStatus = MonarchReadState::kAtStartOfRun;
        fReadState.fAcquisitionID = 0;
        fReadState.fReadPtrOffset = 0;
        fReadState.fAbsoluteRecordOffset = 0;

        // skip forward in the run if fStartTime is non-zero
        if (fStartTime > 0.)
        {
            double recordLength = fRecordSize * fBinWidth; // seconds
            unsigned recordSkips = (unsigned)(fStartTime / recordLength);
            fReadState.fAbsoluteRecordOffset = recordSkips;
            fReadState.fReadPtrOffset = (unsigned)((fStartTime - (double)recordSkips * recordLength) / fBinWidth);
            fReadState.fSliceStartPtrOffset = fReadState.fReadPtrOffset;
        }

        fSliceNumber = 0;

        // set the stream pointer to stream 0
        fStream0 = fMonarch->GetStream(0);

        return fHeaderPtr;
    }

    KTDataPtr KTEgg3Reader::HatchNextSliceRealUnsigned()
    {
        unsigned recordSize = fHeader.GetRecordSize();

        if (fMonarch == NULL)
        {
            KTERROR(eggreadlog, "Monarch file has not been opened");
            return KTDataPtr();
        }
        if (fReadState.fStatus == MonarchReadState::kInvalid)
        {
            KTERROR(eggreadlog, "Read state status is <invalid>. Did you hatch the egg first?");
            return KTDataPtr();
        }

        if (fReadState.fStatus == MonarchReadState::kAtStartOfRun)
        {
            KTDEBUG(eggreadlog, "Reading first record");
            // if we're at the beginning of the run, load the first records
            if (! fStream0->ReadRecord(fReadState.fAbsoluteRecordOffset))
            {
                KTERROR(eggreadlog, "File appears to contain no slices.");
                return KTDataPtr();
            }
            fSliceNumber = 0;
        }
        else
        {
            ++fSliceNumber;

            if (fReadState.fStatus == MonarchReadState::kReachedNextRecord)
            {
                KTDEBUG(eggreadlog, "Slice and record boundaries coincided; reading new record before creating new slice");
                // if the slice boundary and record boundary coincide, we need to load the next record because the final position
                // of the read pointer from the previous slice is at the sample after the end of the previous slice,
                // which in this case would be the start of the next record.
                if (! fStream0->ReadRecord())
                {
                    KTWARN(eggreadlog, "End of egg file reached after reading new records (or something else went wrong)");
                    return KTDataPtr();
                }
                ++(fReadState.fAbsoluteRecordOffset);
                fReadState.fReadPtrOffset = 0;
                ++(fReadState.fReadPtrRecordOffset);
                fReadState.fStatus = MonarchReadState::kContinueReading;
            }

            // shift the slice start pointer by the stride
            // note that this pointer refers to the record in which the previous slice started
            fReadState.fSliceStartPtrOffset += fStride;
            unsigned sliceStartRecordOffset = 0; // how many records to shift to the start of the slice
            while (fReadState.fSliceStartPtrOffset >= recordSize)
            {
                fReadState.fSliceStartPtrOffset -= recordSize;
                ++sliceStartRecordOffset;
            }

            // Calculate whether we need to move the read pointer to a different record by subtracing the number
            // of records read in the last slice (fReadPtrRecordOffset)
            // If this is 0, it doesn't need to be moved
            // If it's != 0, then it needs to be reduced by 1 because of how the offset number in Monarch::ReadRecord is used (offset=0 will advance to the next record; offset=-1 will read the same record)
            int readPtrRecordOffsetShift = int(sliceStartRecordOffset) - int(fReadState.fReadPtrRecordOffset);
            if (readPtrRecordOffsetShift != 0)
            {
                // change the absolute record offset first because it should be done before the adjustment to Monarch::ReadRecord offset counting is made
                fReadState.fAbsoluteRecordOffset += readPtrRecordOffsetShift;
                --readPtrRecordOffsetShift;
                KTDEBUG(eggreadlog, "Reading new record with offset " << readPtrRecordOffsetShift);
                // move the read pointer to the slice start pointer (first move monarch to the correct record)
                if (! fStream0->ReadRecord(readPtrRecordOffsetShift))
                {
                    KTWARN(eggreadlog, "End of egg file reached after reading new records (or something else went wrong)");
                    return KTDataPtr();
                }
            }
            // Move the read pointer to the slice start pointer within the record
            fReadState.fReadPtrRecordOffset = 0;
            fReadState.fReadPtrOffset = fReadState.fSliceStartPtrOffset;
        }

        KTDataPtr newData(new KTData());

        // Fill out slice header information
        KTSliceHeader& sliceHeader = newData->Of< KTSliceHeader >().SetNComponents(fHeader.GetNChannels());
        if (fReadState.fStatus == MonarchReadState::kAtStartOfRun)
        {
            sliceHeader.SetIsNewAcquisition(true);
        }
        else
        {
            sliceHeader.SetIsNewAcquisition(false);
        }
        sliceHeader.SetSampleRate(fHeader.GetAcquisitionRate());
        sliceHeader.SetRawSliceSize(fSliceSize);
        sliceHeader.SetSliceSize(fSliceSize);
        sliceHeader.CalculateBinWidthAndSliceLength();
        sliceHeader.SetNonOverlapFrac((double)fStride / (double)fSliceSize);
        sliceHeader.SetTimeInRun(GetTimeInRun());
        sliceHeader.SetSliceNumber(fSliceNumber);
        sliceHeader.SetStartRecordNumber(fReadState.fAbsoluteRecordOffset);
        sliceHeader.SetStartSampleNumber(fReadState.fReadPtrOffset);
        sliceHeader.SetRecordSize(fHeader.GetRecordSize());
        KTDEBUG(eggreadlog, sliceHeader << "\nNote: some fields may not be filled in correctly yet");

        // Setup pointers to monarch and new katydid records
        unsigned nChannels = fHeader.GetNChannels();
        vector< const M3Record* > monarchRecords(nChannels);
        // the elements of monarchRecordData will need to be deleted
        vector< const M3DataReader< uint64_t >* > monarchRecordData(nChannels);
        vector< KTRawTimeSeries* > newRecords(nChannels);
        for (unsigned iChannel = 0; iChannel < nChannels; ++iChannel)
        {
            monarchRecords[iChannel] = fStream0->GetChannelRecord(iChannel);
            sliceHeader.SetAcquisitionID(fStream0->GetAcquisitionId(), iChannel);
            sliceHeader.SetRecordID(monarchRecords[iChannel]->GetRecordId(), iChannel);
            sliceHeader.SetTimeStamp(monarchRecords[iChannel]->GetTime(), iChannel);
            // TODO: here we assume digitized data
            monarchRecordData[iChannel] = new M3DataReader< uint64_t >(monarchRecords[iChannel]->GetData(), fHeader.GetDataTypeSize(), monarch3::sDigitizedUS);

            //tsData->SetTimeSeries(new vector< DataType >(monarchRecord->fDataPtr, monarchRecord->fDataPtr+header->GetSliceSize()), iChannel);
            newRecords[iChannel] = new KTRawTimeSeries(fSliceSize, 0., double(fSliceSize) * sliceHeader.GetBinWidth());
        }

        KTDEBUG(eggreadlog, "Time in run: " << GetTimeInRun() << " s\n" <<
                "\tBin width = " << fBinWidth << '\n' <<
                "\tMonarch record size = " << fRecordSize << '\n' <<
                "\tRecord offset = " << fReadState.fAbsoluteRecordOffset << '\n' <<
                "\tSlice start pointer offset = " << fReadState.fSliceStartPtrOffset << '\n' <<
                "\tRead pointer record offset = " << fReadState.fReadPtrRecordOffset << '\n' <<
                "\tRead pointer offset = " << fReadState.fReadPtrOffset);

        if (fReadState.fStatus == MonarchReadState::kAtStartOfRun)
        {
            fReadState.fStatus = MonarchReadState::kContinueReading;
        }

        // Loop over bins
        for (unsigned iBin = 0; iBin < fSliceSize; ++iBin)
        {
            if (fReadState.fStatus == MonarchReadState::kReachedNextRecord)
            {
                KTDEBUG(eggreadlog, "Reading new record mid-slice");
                // try reading the next record
                if (! fStream0->ReadRecord())
                {
                    // the end of the file has been reached or there was some other error preventing the reading of the next record
                    KTWARN(eggreadlog, "End of egg file reached after reading new records (or something else went wrong)");
                    for (unsigned iChannel = 0; iChannel < nChannels; ++iChannel)
                    {
                        delete newRecords[iChannel];
                    }
                    return KTDataPtr();
                }
                ++(fReadState.fAbsoluteRecordOffset);
                fReadState.fReadPtrOffset = 0;
                ++(fReadState.fReadPtrRecordOffset);

                // check if the acquisition ID has changed on any channel
                if (fReadState.fAcquisitionID != fStream0->GetAcquisitionId())
                {
                    KTDEBUG(eggreadlog, "Acquisition ID change; resetting slice to start with this monarch record.\n"
                            << "\tNumber of unused bins: " << iBin - 1);
                    // this means that a new monarch record has come up, and it has a different acquisition id.
                    // in this situation we need to start the time series over with the new monarch record
                    // first, update the read state
                    fReadState.fAcquisitionID = fStream0->GetAcquisitionId();
                    fReadState.fReadPtrOffset = 0;
                    fReadState.fReadPtrRecordOffset = 0;
                    fReadState.fSliceStartPtrOffset = 0;
                    // reset slice data
                    sliceHeader.SetIsNewAcquisition(true);
                    sliceHeader.SetStartRecordNumber(fReadState.fAbsoluteRecordOffset);
                    sliceHeader.SetStartSampleNumber(fReadState.fReadPtrOffset);
                    for (unsigned iChannel = 0; iChannel < nChannels; ++iChannel)
                    {
                        sliceHeader.SetAcquisitionID(fStream0->GetAcquisitionId(), iChannel);
                        sliceHeader.SetRecordID(monarchRecords[iChannel]->GetRecordId(), iChannel);
                        sliceHeader.SetTimeStamp(monarchRecords[iChannel]->GetTime(), iChannel);
                    }
                    // reset bin count to 0
                    iBin = 0;
                    // change the time in run since we're going back to the beginning of the record
                    sliceHeader.SetTimeInRun(GetTimeInRun());
                    KTDEBUG(eggreadlog, "Correction to time in run: " << GetTimeInRun() << " s\n" <<
                            "\tBin width = " << fBinWidth << '\n' <<
                            "\tMonarch record size = " << fRecordSize << '\n' <<
                            "\tRecord offset = " << fReadState.fAbsoluteRecordOffset << '\n' <<
                            "\tSlice start pointer offset = " << fReadState.fSliceStartPtrOffset << '\n' <<
                            "\tRead pointer record offset = " << fReadState.fReadPtrRecordOffset << '\n' <<
                            "\tRead pointer offset = " << fReadState.fReadPtrOffset);
                    sliceHeader.SetSliceNumber(fSliceNumber);
                }

                fReadState.fStatus = MonarchReadState::kContinueReading;
            }

            // Read the data from the records
            for (unsigned iChannel = 0; iChannel < nChannels; ++iChannel)
            {
                // set the data
                (*newRecords[iChannel])(iBin) = monarchRecordData[iChannel]->at(fReadState.fReadPtrOffset);
            }

            // advance the pointer for the next bin
            ++(fReadState.fReadPtrOffset);

            // check if we've reached the end of a monarch record
            if (fReadState.fReadPtrOffset >= recordSize)
            {
                KTDEBUG(eggreadlog, "End of Monarch record reached.");
                fReadState.fStatus = MonarchReadState::kReachedNextRecord;
            }
        } // end loop over bins

        sliceHeader.SetEndRecordNumber(fReadState.fAbsoluteRecordOffset);
        sliceHeader.SetEndSampleNumber(fReadState.fReadPtrOffset - 1);

        // delete the monarchRecordData objects
        while(! monarchRecordData.empty())
        {
            delete monarchRecordData.back();
            monarchRecordData.pop_back();
        }

        // finally, set the records in the new data object
        KTRawTimeSeriesData& tsData = newData->Of< KTRawTimeSeriesData >().SetNComponents(nChannels);
        for (unsigned iChannel = 0; iChannel < nChannels; ++iChannel)
        {
            tsData.SetTimeSeries(newRecords[iChannel], iChannel);
        }

        return newData;
    }

    KTDataPtr KTEgg3Reader::HatchNextSliceRealSigned()
    {
        unsigned recordSize = fHeader.GetRecordSize();

        if (fMonarch == NULL)
        {
            KTERROR(eggreadlog, "Monarch file has not been opened");
            return KTDataPtr();
        }
        if (fReadState.fStatus == MonarchReadState::kInvalid)
        {
            KTERROR(eggreadlog, "Read state status is <invalid>. Did you hatch the egg first?");
            return KTDataPtr();
        }

        if (fReadState.fStatus == MonarchReadState::kAtStartOfRun)
        {
            KTDEBUG(eggreadlog, "Reading first record");
            // if we're at the beginning of the run, load the first records
            if (! fStream0->ReadRecord(fReadState.fAbsoluteRecordOffset))
            {
                KTERROR(eggreadlog, "File appears to contain no slices.");
                return KTDataPtr();
            }
            fSliceNumber = 0;
        }
        else
        {
            ++fSliceNumber;

            if (fReadState.fStatus == MonarchReadState::kReachedNextRecord)
            {
                KTDEBUG(eggreadlog, "Slice and record boundaries coincided; reading new record before creating new slice");
                // if the slice boundary and record boundary coincide, we need to load the next record because the final position
                // of the read pointer from the previous slice is at the sample after the end of the previous slice,
                // which in this case would be the start of the next record.
                if (! fStream0->ReadRecord())
                {
                    KTWARN(eggreadlog, "End of egg file reached after reading new records (or something else went wrong)");
                    return KTDataPtr();
                }
                ++(fReadState.fAbsoluteRecordOffset);
                fReadState.fReadPtrOffset = 0;
                ++(fReadState.fReadPtrRecordOffset);
                fReadState.fStatus = MonarchReadState::kContinueReading;
            }

            // shift the slice start pointer by the stride
            // note that this pointer refers to the record in which the previous slice started
            fReadState.fSliceStartPtrOffset += fStride;
            unsigned sliceStartRecordOffset = 0; // how many records to shift to the start of the slice
            while (fReadState.fSliceStartPtrOffset >= recordSize)
            {
                fReadState.fSliceStartPtrOffset -= recordSize;
                ++sliceStartRecordOffset;
            }

            // Calculate whether we need to move the read pointer to a different record by subtracing the number
            // of records read in the last slice (fReadPtrRecordOffset)
            // If this is 0, it doesn't need to be moved
            // If it's != 0, then it needs to be reduced by 1 because of how the offset number in Monarch::ReadRecord is used (offset=0 will advance to the next record; offset=-1 will read the same record)
            int readPtrRecordOffsetShift = int(sliceStartRecordOffset) - int(fReadState.fReadPtrRecordOffset);
            if (readPtrRecordOffsetShift != 0)
            {
                // change the absolute record offset first because it should be done before the adjustment to Monarch::ReadRecord offset counting is made
                fReadState.fAbsoluteRecordOffset += readPtrRecordOffsetShift;
                --readPtrRecordOffsetShift;
                KTDEBUG(eggreadlog, "Reading new record with offset " << readPtrRecordOffsetShift);
                // move the read pointer to the slice start pointer (first move monarch to the correct record)
                if (! fStream0->ReadRecord(readPtrRecordOffsetShift))
                {
                    KTWARN(eggreadlog, "End of egg file reached after reading new records (or something else went wrong)");
                    return KTDataPtr();
                }
            }
            // Move the read pointer to the slice start pointer within the record
            fReadState.fReadPtrRecordOffset = 0;
            fReadState.fReadPtrOffset = fReadState.fSliceStartPtrOffset;
        }

        KTDataPtr newData(new KTData());

        // Fill out slice header information
        KTSliceHeader& sliceHeader = newData->Of< KTSliceHeader >().SetNComponents(fHeader.GetNChannels());
        if (fReadState.fStatus == MonarchReadState::kAtStartOfRun)
        {
            sliceHeader.SetIsNewAcquisition(true);
        }
        else
        {
            sliceHeader.SetIsNewAcquisition(false);
        }
        sliceHeader.SetSampleRate(fHeader.GetAcquisitionRate());
        sliceHeader.SetRawSliceSize(fSliceSize);
        sliceHeader.SetSliceSize(fSliceSize);
        sliceHeader.CalculateBinWidthAndSliceLength();
        sliceHeader.SetNonOverlapFrac((double)fStride / (double)fSliceSize);
        sliceHeader.SetTimeInRun(GetTimeInRun());
        sliceHeader.SetSliceNumber(fSliceNumber);
        sliceHeader.SetStartRecordNumber(fReadState.fAbsoluteRecordOffset);
        sliceHeader.SetStartSampleNumber(fReadState.fReadPtrOffset);
        sliceHeader.SetRecordSize(fHeader.GetRecordSize());
        KTDEBUG(eggreadlog, sliceHeader << "\nNote: some fields may not be filled in correctly yet");

        // Setup pointers to monarch and new katydid records
        unsigned nChannels = fHeader.GetNChannels();
        vector< const M3Record* > monarchRecords(nChannels);
        // the elements of monarchRecordData will need to be deleted
        vector< const M3DataReader< int64_t >* > monarchRecordData(nChannels);
        vector< KTRawTimeSeries* > newRecords(nChannels);
        for (unsigned iChannel = 0; iChannel < nChannels; ++iChannel)
        {
            monarchRecords[iChannel] = fStream0->GetChannelRecord(iChannel);
            sliceHeader.SetAcquisitionID(fStream0->GetAcquisitionId(), iChannel);
            sliceHeader.SetRecordID(monarchRecords[iChannel]->GetRecordId(), iChannel);
            sliceHeader.SetTimeStamp(monarchRecords[iChannel]->GetTime(), iChannel);
            // TODO: here we assume digitized data
            monarchRecordData[iChannel] = new M3DataReader< int64_t >(monarchRecords[iChannel]->GetData(), fHeader.GetDataTypeSize(), monarch3::sDigitizedS);

            //tsData->SetTimeSeries(new vector< DataType >(monarchRecord->fDataPtr, monarchRecord->fDataPtr+header->GetSliceSize()), iChannel);
            newRecords[iChannel] = new KTRawTimeSeries(fSliceSize, 0., double(fSliceSize) * sliceHeader.GetBinWidth());
        }

        KTDEBUG(eggreadlog, "Time in run: " << GetTimeInRun() << " s\n" <<
                "\tBin width = " << fBinWidth << '\n' <<
                "\tMonarch record size = " << fRecordSize << '\n' <<
                "\tRecord offset = " << fReadState.fAbsoluteRecordOffset << '\n' <<
                "\tSlice start pointer offset = " << fReadState.fSliceStartPtrOffset << '\n' <<
                "\tRead pointer record offset = " << fReadState.fReadPtrRecordOffset << '\n' <<
                "\tRead pointer offset = " << fReadState.fReadPtrOffset);

        if (fReadState.fStatus == MonarchReadState::kAtStartOfRun)
        {
            fReadState.fStatus = MonarchReadState::kContinueReading;
        }

        // Loop over bins
        for (unsigned iBin = 0; iBin < fSliceSize; ++iBin)
        {
            if (fReadState.fStatus == MonarchReadState::kReachedNextRecord)
            {
                KTDEBUG(eggreadlog, "Reading new record mid-slice");
                // try reading the next record
                if (! fStream0->ReadRecord())
                {
                    // the end of the file has been reached or there was some other error preventing the reading of the next record
                    KTWARN(eggreadlog, "End of egg file reached after reading new records (or something else went wrong)");
                    for (unsigned iChannel = 0; iChannel < nChannels; ++iChannel)
                    {
                        delete newRecords[iChannel];
                    }
                    return KTDataPtr();
                }
                ++(fReadState.fAbsoluteRecordOffset);
                fReadState.fReadPtrOffset = 0;
                ++(fReadState.fReadPtrRecordOffset);

                // check if the acquisition ID has changed on any channel
                if (fReadState.fAcquisitionID != fStream0->GetAcquisitionId())
                {
                    KTDEBUG(eggreadlog, "Acquisition ID change; resetting slice to start with this monarch record.\n"
                            << "\tNumber of unused bins: " << iBin - 1);
                    // this means that a new monarch record has come up, and it has a different acquisition id.
                    // in this situation we need to start the time series over with the new monarch record
                    // first, update the read state
                    fReadState.fAcquisitionID = fStream0->GetAcquisitionId();
                    fReadState.fReadPtrOffset = 0;
                    fReadState.fReadPtrRecordOffset = 0;
                    fReadState.fSliceStartPtrOffset = 0;
                    // reset slice data
                    sliceHeader.SetIsNewAcquisition(true);
                    sliceHeader.SetStartRecordNumber(fReadState.fAbsoluteRecordOffset);
                    sliceHeader.SetStartSampleNumber(fReadState.fReadPtrOffset);
                    for (unsigned iChannel = 0; iChannel < nChannels; ++iChannel)
                    {
                        sliceHeader.SetAcquisitionID(fStream0->GetAcquisitionId(), iChannel);
                        sliceHeader.SetRecordID(monarchRecords[iChannel]->GetRecordId(), iChannel);
                        sliceHeader.SetTimeStamp(monarchRecords[iChannel]->GetTime(), iChannel);
                    }
                    // reset bin count to 0
                    iBin = 0;
                    // change the time in run since we're going back to the beginning of the record
                    sliceHeader.SetTimeInRun(GetTimeInRun());
                    KTDEBUG(eggreadlog, "Correction to time in run: " << GetTimeInRun() << " s\n" <<
                            "\tBin width = " << fBinWidth << '\n' <<
                            "\tMonarch record size = " << fRecordSize << '\n' <<
                            "\tRecord offset = " << fReadState.fAbsoluteRecordOffset << '\n' <<
                            "\tSlice start pointer offset = " << fReadState.fSliceStartPtrOffset << '\n' <<
                            "\tRead pointer record offset = " << fReadState.fReadPtrRecordOffset << '\n' <<
                            "\tRead pointer offset = " << fReadState.fReadPtrOffset);
                    sliceHeader.SetSliceNumber(fSliceNumber);
                }

                fReadState.fStatus = MonarchReadState::kContinueReading;
            }

            // Read the data from the records
            for (unsigned iChannel = 0; iChannel < nChannels; ++iChannel)
            {
                // set the data
                (*newRecords[iChannel])(iBin) = monarchRecordData[iChannel]->at(fReadState.fReadPtrOffset);
            }

            // advance the pointer for the next bin
            ++(fReadState.fReadPtrOffset);

            // check if we've reached the end of a monarch record
            if (fReadState.fReadPtrOffset >= recordSize)
            {
                KTDEBUG(eggreadlog, "End of Monarch record reached.");
                fReadState.fStatus = MonarchReadState::kReachedNextRecord;
            }
        } // end loop over bins

        sliceHeader.SetEndRecordNumber(fReadState.fAbsoluteRecordOffset);
        sliceHeader.SetEndSampleNumber(fReadState.fReadPtrOffset - 1);

        // delete the monarchRecordData objects
        while(! monarchRecordData.empty())
        {
            delete monarchRecordData.back();
            monarchRecordData.pop_back();
        }

        // finally, set the records in the new data object
        KTRawTimeSeriesData& tsData = newData->Of< KTRawTimeSeriesData >().SetNComponents(nChannels);
        for (unsigned iChannel = 0; iChannel < nChannels; ++iChannel)
        {
            tsData.SetTimeSeries(newRecords[iChannel], iChannel);
        }

        return newData;
    }

    KTDataPtr KTEgg3Reader::HatchNextSliceComplex()
    {
        unsigned recordSize = fHeader.GetRecordSize();

        if (fMonarch == NULL)
        {
            KTERROR(eggreadlog, "Monarch file has not been opened");
            return KTDataPtr();
        }
        if (fReadState.fStatus == MonarchReadState::kInvalid)
        {
            KTERROR(eggreadlog, "Read state status is <invalid>. Did you hatch the egg first?");
            return KTDataPtr();
        }

        if (fReadState.fStatus == MonarchReadState::kAtStartOfRun)
        {
            KTDEBUG(eggreadlog, "Reading first record");
            // if we're at the beginning of the run, load the first records
            if (! fStream0->ReadRecord(fReadState.fAbsoluteRecordOffset))
            {
                KTERROR(eggreadlog, "File appears to contain no slices.");
                return KTDataPtr();
            }
            fSliceNumber = 0;
        }
        else
        {
            ++fSliceNumber;

            if (fReadState.fStatus == MonarchReadState::kReachedNextRecord)
            {
                KTDEBUG(eggreadlog, "Slice and record boundaries coincided; reading new record before creating new slice");
                // if the slice boundary and record boundary coincide, we need to load the next record because the final position
                // of the read pointer from the previous slice is at the sample after the end of the previous slice,
                // which in this case would be the start of the next record.
                if (! fStream0->ReadRecord())
                {
                    KTWARN(eggreadlog, "End of egg file reached after reading new records (or something else went wrong)");
                    return KTDataPtr();
                }
                ++(fReadState.fAbsoluteRecordOffset);
                fReadState.fReadPtrOffset = 0;
                ++(fReadState.fReadPtrRecordOffset);
                fReadState.fStatus = MonarchReadState::kContinueReading;
            }

            // shift the slice start pointer by the stride
            // note that this pointer refers to the record in which the previous slice started
            fReadState.fSliceStartPtrOffset += fStride;
            unsigned sliceStartRecordOffset = 0; // how many records to shift to the start of the slice
            while (fReadState.fSliceStartPtrOffset >= recordSize)
            {
                fReadState.fSliceStartPtrOffset -= recordSize;
                ++sliceStartRecordOffset;
            }

            // Calculate whether we need to move the read pointer to a different record by subtracing the number
            // of records read in the last slice (fReadPtrRecordOffset)
            // If this is 0, it doesn't need to be moved
            // If it's != 0, then it needs to be reduced by 1 because of how the offset number in Monarch::ReadRecord is used (offset=0 will advance to the next record; offset=-1 will read the same record)
            int readPtrRecordOffsetShift = int(sliceStartRecordOffset) - int(fReadState.fReadPtrRecordOffset);
            if (readPtrRecordOffsetShift != 0)
            {
                // change the absolute record offset first because it should be done before the adjustment to Monarch::ReadRecord offset counting is made
                fReadState.fAbsoluteRecordOffset += readPtrRecordOffsetShift;
                --readPtrRecordOffsetShift;
                KTDEBUG(eggreadlog, "Reading new record with offset " << readPtrRecordOffsetShift);
                // move the read pointer to the slice start pointer (first move monarch to the correct record)
                if (! fStream0->ReadRecord(readPtrRecordOffsetShift))
                {
                    KTWARN(eggreadlog, "End of egg file reached after reading new records (or something else went wrong)");
                    return KTDataPtr();
                }
            }
            // Move the read pointer to the slice start pointer within the record
            fReadState.fReadPtrRecordOffset = 0;
            fReadState.fReadPtrOffset = fReadState.fSliceStartPtrOffset;
        }

        KTDataPtr newData(new KTData());

        // Fill out slice header information
        KTSliceHeader& sliceHeader = newData->Of< KTSliceHeader >().SetNComponents(fHeader.GetNChannels());
        if (fReadState.fStatus == MonarchReadState::kAtStartOfRun)
        {
            sliceHeader.SetIsNewAcquisition(true);
        }
        else
        {
            sliceHeader.SetIsNewAcquisition(false);
        }
        sliceHeader.SetSampleRate(fHeader.GetAcquisitionRate());
        sliceHeader.SetRawSliceSize(fSliceSize);
        sliceHeader.SetSliceSize(fSliceSize);
        sliceHeader.CalculateBinWidthAndSliceLength();
        sliceHeader.SetNonOverlapFrac((double)fStride / (double)fSliceSize);
        sliceHeader.SetTimeInRun(GetTimeInRun());
        sliceHeader.SetSliceNumber(fSliceNumber);
        sliceHeader.SetStartRecordNumber(fReadState.fAbsoluteRecordOffset);
        sliceHeader.SetStartSampleNumber(fReadState.fReadPtrOffset);
        sliceHeader.SetRecordSize(fHeader.GetRecordSize());
        KTDEBUG(eggreadlog, sliceHeader << "\nNote: some fields may not be filled in correctly yet");

        // Setup pointers to monarch and new katydid records
        unsigned nChannels = fHeader.GetNChannels();
        vector< const M3Record* > monarchRecords(nChannels);
        // the elements of monarchRecordData will need to be deleted
        vector< const M3ComplexDataReader< fftw_complex >* > monarchRecordData(nChannels);
        vector< KTTimeSeriesFFTW* > newRecords(nChannels);
        for (unsigned iChannel = 0; iChannel < nChannels; ++iChannel)
        {
            monarchRecords[iChannel] = fStream0->GetChannelRecord(iChannel);
            sliceHeader.SetAcquisitionID(fStream0->GetAcquisitionId(), iChannel);
            sliceHeader.SetRecordID(monarchRecords[iChannel]->GetRecordId(), iChannel);
            sliceHeader.SetTimeStamp(monarchRecords[iChannel]->GetTime(), iChannel);
            // TODO: here we assume analog data
            monarchRecordData[iChannel] = new M3ComplexDataReader< fftw_complex >(monarchRecords[iChannel]->GetData(), fHeader.GetDataTypeSize(), monarch3::sAnalog, 2);

            //tsData->SetTimeSeries(new vector< DataType >(monarchRecord->fDataPtr, monarchRecord->fDataPtr+header->GetSliceSize()), iChannel);
            newRecords[iChannel] = new KTTimeSeriesFFTW(fSliceSize, 0., double(fSliceSize) * sliceHeader.GetBinWidth());
        }

        KTDEBUG(eggreadlog, "Time in run: " << GetTimeInRun() << " s\n" <<
                "\tBin width = " << fBinWidth << '\n' <<
                "\tMonarch record size = " << fRecordSize << '\n' <<
                "\tRecord offset = " << fReadState.fAbsoluteRecordOffset << '\n' <<
                "\tSlice start pointer offset = " << fReadState.fSliceStartPtrOffset << '\n' <<
                "\tRead pointer record offset = " << fReadState.fReadPtrRecordOffset << '\n' <<
                "\tRead pointer offset = " << fReadState.fReadPtrOffset);

        if (fReadState.fStatus == MonarchReadState::kAtStartOfRun)
        {
            fReadState.fStatus = MonarchReadState::kContinueReading;
        }

        // Loop over bins
        for (unsigned iBin = 0; iBin < fSliceSize; ++iBin)
        {
            if (fReadState.fStatus == MonarchReadState::kReachedNextRecord)
            {
                KTDEBUG(eggreadlog, "Reading new record mid-slice");
                // try reading the next record
                if (! fStream0->ReadRecord())
                {
                    // the end of the file has been reached or there was some other error preventing the reading of the next record
                    KTWARN(eggreadlog, "End of egg file reached after reading new records (or something else went wrong)");
                    for (unsigned iChannel = 0; iChannel < nChannels; ++iChannel)
                    {
                        delete newRecords[iChannel];
                    }
                    return KTDataPtr();
                }
                ++(fReadState.fAbsoluteRecordOffset);
                fReadState.fReadPtrOffset = 0;
                ++(fReadState.fReadPtrRecordOffset);

                // check if the acquisition ID has changed on any channel
                if (fReadState.fAcquisitionID != fStream0->GetAcquisitionId())
                {
                    KTDEBUG(eggreadlog, "Acquisition ID change; resetting slice to start with this monarch record.\n"
                            << "\tNumber of unused bins: " << iBin - 1);
                    // this means that a new monarch record has come up, and it has a different acquisition id.
                    // in this situation we need to start the time series over with the new monarch record
                    // first, update the read state
                    fReadState.fAcquisitionID = fStream0->GetAcquisitionId();
                    fReadState.fReadPtrOffset = 0;
                    fReadState.fReadPtrRecordOffset = 0;
                    fReadState.fSliceStartPtrOffset = 0;
                    // reset slice data
                    sliceHeader.SetIsNewAcquisition(true);
                    sliceHeader.SetStartRecordNumber(fReadState.fAbsoluteRecordOffset);
                    sliceHeader.SetStartSampleNumber(fReadState.fReadPtrOffset);
                    for (unsigned iChannel = 0; iChannel < nChannels; ++iChannel)
                    {
                        sliceHeader.SetAcquisitionID(fStream0->GetAcquisitionId(), iChannel);
                        sliceHeader.SetRecordID(monarchRecords[iChannel]->GetRecordId(), iChannel);
                        sliceHeader.SetTimeStamp(monarchRecords[iChannel]->GetTime(), iChannel);
                    }
                    // reset bin count to 0
                    iBin = 0;
                    // change the time in run since we're going back to the beginning of the record
                    sliceHeader.SetTimeInRun(GetTimeInRun());
                    KTDEBUG(eggreadlog, "Correction to time in run: " << GetTimeInRun() << " s\n" <<
                            "\tBin width = " << fBinWidth << '\n' <<
                            "\tMonarch record size = " << fRecordSize << '\n' <<
                            "\tRecord offset = " << fReadState.fAbsoluteRecordOffset << '\n' <<
                            "\tSlice start pointer offset = " << fReadState.fSliceStartPtrOffset << '\n' <<
                            "\tRead pointer record offset = " << fReadState.fReadPtrRecordOffset << '\n' <<
                            "\tRead pointer offset = " << fReadState.fReadPtrOffset);
                    sliceHeader.SetSliceNumber(fSliceNumber);
                }

                fReadState.fStatus = MonarchReadState::kContinueReading;
            }

            // Read the data from the records
            for (unsigned iChannel = 0; iChannel < nChannels; ++iChannel)
            {
                // set the data
                (*newRecords[iChannel])(iBin)[0] = monarchRecordData[iChannel]->at(fReadState.fReadPtrOffset)[0];
                (*newRecords[iChannel])(iBin)[1] = monarchRecordData[iChannel]->at(fReadState.fReadPtrOffset)[1];
            }

            // advance the pointer for the next bin
            ++(fReadState.fReadPtrOffset);

            // check if we've reached the end of a monarch record
            if (fReadState.fReadPtrOffset >= recordSize)
            {
                KTDEBUG(eggreadlog, "End of Monarch record reached.");
                fReadState.fStatus = MonarchReadState::kReachedNextRecord;
            }
        } // end loop over bins

        sliceHeader.SetEndRecordNumber(fReadState.fAbsoluteRecordOffset);
        sliceHeader.SetEndSampleNumber(fReadState.fReadPtrOffset - 1);

        // delete the monarchRecordData objects
        while(! monarchRecordData.empty())
        {
            delete monarchRecordData.back();
            monarchRecordData.pop_back();
        }

        // finally, set the records in the new data object
        KTTimeSeriesData& tsData = newData->Of< KTTimeSeriesData >().SetNComponents(nChannels);
        for (unsigned iChannel = 0; iChannel < nChannels; ++iChannel)
        {
            tsData.SetTimeSeries(newRecords[iChannel], iChannel);
        }

        return newData;
    }

    bool KTEgg3Reader::CloseEgg()
    {
        try
        {
            fMonarch->FinishReading();
        }
        catch (M3Exception& e)
        {
            KTERROR(eggreadlog, "Something went wrong while closing the file: " << e.what());
        }
        delete fMonarch;
        fMonarch = NULL;
        return true;
    }


    void KTEgg3Reader::CopyHeaderInformation(const M3Header* monarchHeader)
    {
        const M3StreamHeader& stream0Header = monarchHeader->GetStreamHeaders()[0];
        const M3ChannelHeader& channel0Header = monarchHeader->GetChannelHeaders()[0];
        fHeader.SetFilename(monarchHeader->GetFilename());
        fHeader.SetAcquisitionMode(stream0Header.GetNChannels());
        fHeader.SetNChannels(stream0Header.GetNChannels());
        fHeader.SetRecordSize(stream0Header.GetRecordSize());
        fHeader.SetRunDuration(monarchHeader->GetRunDuration());
        fHeader.SetAcquisitionRate(stream0Header.GetAcquisitionRate() * fSampleRateUnitsInHz);
        fHeader.SetTimestamp(monarchHeader->GetTimestamp());
        fHeader.SetDescription(monarchHeader->GetDescription());
        fHeader.SetRunType(999);
        fHeader.SetRunSource(999);
        fHeader.SetFormatMode(stream0Header.GetChannelFormat());
        fHeader.SetDataTypeSize(stream0Header.GetDataTypeSize());
        fHeader.SetBitDepth(stream0Header.GetBitDepth());
        fHeader.SetVoltageMin(channel0Header.GetVoltageMin());
        fHeader.SetVoltageRange(channel0Header.GetVoltageRange());
        return;
    }

    double KTEgg3Reader::GetTimeInRunFirstCall() const
    {
        fT0Offset = fStream0->GetChannelRecord(0)->GetTime();
        KTDEBUG(eggreadlog, "Time offset of the first slice: " << fT0Offset << " ns");
        if (fT0Offset == 0)
        {
            KTDEBUG(eggreadlog, "First call to GetTimeInRun; Monarch record time is 0; switching GetTIR function to manual");
            fGetTimeInRun = &KTEgg3Reader::GetTimeInRunManually;
        }
        else
        {
            KTDEBUG(eggreadlog, "First call to GetTimeInRun; Monarch record time is not 0; switching GetTIR function to from-monarch");
            fGetTimeInRun = &KTEgg3Reader::GetTimeInRunFromMonarch;
        }
        return GetTimeInRun();
    }

} /* namespace Katydid */


