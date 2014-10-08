/*
 * KTEgg.cc
 *
 *  Created on: Sep 9, 2011
 *      Author: nsoblath
 */

#include "KTEggReaderMonarch.hh"

#include "KTEggHeader.hh"
#include "KTLogger.hh"
#include "KTSliceHeader.hh"
#include "KTRawTimeSeriesData.hh"
#include "KTRawTimeSeries.hh"

#include "Monarch.hpp"
#include "MonarchException.hpp"
#include "MonarchHeader.hpp"

using monarch::Monarch;
using monarch::MonarchHeader;
using monarch::MonarchException;
using monarch::MonarchRecordBytes;
using monarch::MonarchRecordDataInterface;

using std::map;
using std::string;
using std::vector;

namespace Katydid
{
    KTLOGGER(eggreadlog, "KTEggReaderMonarch");

    unsigned KTEggReaderMonarch::GetMaxChannels()
    {
        return fMaxChannels;
    }


    KTEggReaderMonarch::KTEggReaderMonarch() :
            KTEggReader(),
            fSliceSize(1024),
            fStride(0),
            fStartTime(0.),
            fMonarch(NULL),
            fHeaderPtr(new KTData()),
            fHeader(fHeaderPtr->Of< KTEggHeader >()),
            fReadState(),
            fNumberOfChannels(),
            fGetTimeInRun(&KTEggReaderMonarch::GetTimeInRunFirstCall),
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

        fNumberOfChannels.insert(AcqModeMapValue(1, 1));
        fNumberOfChannels.insert(AcqModeMapValue(2, 2));

        fMonarchGetRecord[0] = &Monarch::GetRecordSeparateOne;
        fMonarchGetRecord[1] = &Monarch::GetRecordSeparateTwo;
    }

    KTEggReaderMonarch::~KTEggReaderMonarch()
    {
        if (fMonarch != NULL)
        {
            fMonarch->Close();
            delete fMonarch;
        }
    }

    KTDataPtr KTEggReaderMonarch::BreakEgg(const string& filename)
    {
        if (fStride == 0) fStride = fSliceSize;

        if (fMonarch != NULL)
        {
            fMonarch->Close();
            delete fMonarch;
        }

        // open the file
        KTINFO(eggreadlog, "Opening egg file <" << filename << ">");
        try
        {
            fMonarch = Monarch::OpenForReading(filename);
        }
        catch (MonarchException& e)
        {
            KTERROR(eggreadlog, "Unable to break egg: " << e.what());
            return KTDataPtr();

        }

        KTDEBUG(eggreadlog, "File open; reading header");
        try
        {
            fMonarch->ReadHeader();
        }
        catch (MonarchException& e)
        {
            KTERROR(eggreadlog, "Header was not read correctly: " << e.what() << '\n' <<
                    "Egg breaking aborted.");
            fMonarch->Close();
            delete fMonarch;
            fMonarch = NULL;
            return KTDataPtr();
        }
        CopyHeaderInformation(fMonarch->GetHeader());
        fHeader.SetRawSliceSize(fSliceSize);
        fHeader.SetSliceSize(fSliceSize);
        fHeader.SetSliceStride(fStride);

        KTDEBUG(eggreadlog, "Parsed header:\n" << fHeader);

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

        // force monarch to use Separate interface
        fMonarch->SetInterface(monarch::sInterfaceSeparate);

        fSliceNumber = 0;

        return fHeaderPtr;
    }

    KTDataPtr KTEggReaderMonarch::HatchNextSlice()
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
            if (! fMonarch->ReadRecord(fReadState.fAbsoluteRecordOffset))
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
                if (! fMonarch->ReadRecord())
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
                if (! fMonarch->ReadRecord(readPtrRecordOffsetShift))
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
        vector< const MonarchRecordBytes* > monarchRecords(nChannels);
        // the elements of monarchRecordData will need to be deleted
        vector< const MonarchRecordDataInterface< uint64_t >* > monarchRecordData(nChannels);
        vector< KTRawTimeSeries* > newRecords(nChannels);
        for (unsigned iChannel = 0; iChannel < nChannels; ++iChannel)
        {
            monarchRecords[iChannel] = (fMonarch->*fMonarchGetRecord[iChannel])();
            sliceHeader.SetAcquisitionID(monarchRecords[iChannel]->fAcquisitionId, iChannel);
            sliceHeader.SetRecordID(monarchRecords[iChannel]->fRecordId, iChannel);
            sliceHeader.SetTimeStamp(monarchRecords[iChannel]->fTime, iChannel);
            monarchRecordData[iChannel] = new MonarchRecordDataInterface< uint64_t >(monarchRecords[iChannel]->fData, fHeader.GetDataTypeSize());

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
                if (! fMonarch->ReadRecord())
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
                if (fReadState.fAcquisitionID != monarchRecords[0]->fAcquisitionId)
                {
                    KTDEBUG(eggreadlog, "Acquisition ID change; resetting slice to start with this monarch record.\n"
                            << "\tNumber of unused bins: " << iBin - 1);
                    // this means that a new monarch record has come up, and it has a different acquisition id.
                    // in this situation we need to start the time series over with the new monarch record
                    // first, update the read state
                    fReadState.fAcquisitionID = monarchRecords[0]->fAcquisitionId;
                    fReadState.fReadPtrOffset = 0;
                    fReadState.fReadPtrRecordOffset = 0;
                    fReadState.fSliceStartPtrOffset = 0;
                    // reset slice data
                    sliceHeader.SetIsNewAcquisition(true);
                    sliceHeader.SetStartRecordNumber(fReadState.fAbsoluteRecordOffset);
                    sliceHeader.SetStartSampleNumber(fReadState.fReadPtrOffset);
                    for (unsigned iChannel = 0; iChannel < nChannels; ++iChannel)
                    {
                        sliceHeader.SetAcquisitionID(monarchRecords[iChannel]->fAcquisitionId, iChannel);
                        sliceHeader.SetRecordID(monarchRecords[iChannel]->fRecordId, iChannel);
                        sliceHeader.SetTimeStamp(monarchRecords[iChannel]->fTime, iChannel);
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

    bool KTEggReaderMonarch::CloseEgg()
    {
        try
        {
            fMonarch->Close();
        }
        catch (MonarchException& e)
        {
            KTERROR(eggreadlog, "Something went wrong while closing the file: " << e.what());
        }
        delete fMonarch;
        fMonarch = NULL;
        return true;
    }


    void KTEggReaderMonarch::CopyHeaderInformation(const MonarchHeader* monarchHeader)
    {
        fHeader.SetFilename(monarchHeader->GetFilename());
        fHeader.SetAcquisitionMode(monarchHeader->GetAcquisitionMode());
        fHeader.SetNChannels(fNumberOfChannels[fHeader.GetAcquisitionMode()]);
        fHeader.SetRecordSize(monarchHeader->GetRecordSize());
        fHeader.SetRunDuration(monarchHeader->GetRunDuration());
        fHeader.SetAcquisitionRate(monarchHeader->GetAcquisitionRate() * fSampleRateUnitsInHz);
        fHeader.SetTimestamp(monarchHeader->GetTimestamp());
        fHeader.SetDescription(monarchHeader->GetDescription());
        fHeader.SetRunType(monarchHeader->GetRunType());
        fHeader.SetRunSource(monarchHeader->GetRunSource());
        fHeader.SetFormatMode(monarchHeader->GetFormatMode());
        fHeader.SetDataTypeSize(monarchHeader->GetDataTypeSize());
        fHeader.SetBitDepth(monarchHeader->GetBitDepth());
        fHeader.SetVoltageMin(monarchHeader->GetVoltageMin());
        fHeader.SetVoltageRange(monarchHeader->GetVoltageRange());
        return;
    }

    double KTEggReaderMonarch::GetTimeInRunFirstCall() const
    {
        fT0Offset = (fMonarch->*fMonarchGetRecord[0])()->fTime;
        KTDEBUG(eggreadlog, "Time offset of the first slice: " << fT0Offset << " ns");
        if (fT0Offset == 0)
        {
            KTDEBUG(eggreadlog, "First call to GetTimeInRun; Monarch record time is 0; switching GetTIR function to manual");
            fGetTimeInRun = &KTEggReaderMonarch::GetTimeInRunManually;
        }
        else
        {
            KTDEBUG(eggreadlog, "First call to GetTimeInRun; Monarch record time is not 0; switching GetTIR function to from-monarch");
            fGetTimeInRun = &KTEggReaderMonarch::GetTimeInRunFromMonarch;
        }
        return GetTimeInRun();
    }

} /* namespace Katydid */


