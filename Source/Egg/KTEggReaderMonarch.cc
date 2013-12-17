/*
 * KTEgg.cc
 *
 *  Created on: Sep 9, 2011
 *      Author: nsoblath
 */

#include "KTEggReaderMonarch.hh"

#include "KTLogger.hh"
#include "KTSliceHeader.hh"
#include "KTTimeSeriesData.hh"
#include "KTTimeSeriesFFTW.hh"
#include "KTTimeSeriesReal.hh"

#include "Monarch.hpp"
#include "MonarchException.hpp"
#include "MonarchHeader.hpp"



using std::map;
using std::string;
using std::vector;

namespace Katydid
{
    KTLOGGER(eggreadlog, "katydid.egg");

    unsigned KTEggReaderMonarch::GetMaxChannels()
    {
        return fMaxChannels;
    }


    KTEggReaderMonarch::KTEggReaderMonarch() :
            KTEggReader(),
            fTimeSeriesType(kRealTimeSeries),
            fSliceSize(1024),
            fStride(0),
            fMonarch(NULL),
            fHeader(),
            fReadState(),
            fNumberOfChannels(),
            fGetTimeInRun(&KTEggReaderMonarch::GetTimeInRunFirstCall),
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

    KTEggHeader* KTEggReaderMonarch::BreakEgg(const string& filename)
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
            return NULL;

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
            return NULL;
        }
        CopyHeaderInformation(fMonarch->GetHeader());
        fHeader.SetSliceSize(fSliceSize);

        KTDEBUG(eggreadlog, "Parsed header:\n"
             << "\tFilename: " << fHeader.GetFilename() << '\n'
             << "\tAcquisition Mode: " << fHeader.GetAcquisitionMode() << '\n'
             << "\tNumber of Channels: " << fHeader.GetNChannels() << '\n'
             << "\tSlice Size: " << fHeader.GetSliceSize() << '\n'
             << "\tRecord Size: " << fHeader.GetRecordSize() << '\n'
             << "\tRun Duration: " << fHeader.GetRunDuration() << " ms" << '\n'
             << "\tAcquisition Rate: " << fHeader.GetAcquisitionRate() << " Hz \n"
             << "\tTimestamp: " << fHeader.GetTimestamp() << '\n'
             << "\tDescription: " << fHeader.GetDescription() << '\n'
             << "\tRun Type: " << fHeader.GetRunType() << '\n'
             << "\tRun Source: " << fHeader.GetRunSource() << '\n'
             << "\tFormat Mode: " << fHeader.GetFormatMode());


        fReadState.fStatus = MonarchReadState::kAtStartOfRun;
        fReadState.fAcquisitionID = 0;
        fReadState.fReadPtrOffset = 0;
        fReadState.fAbsoluteRecordOffset = 0;

        fRecordSize = fHeader.GetRecordSize();
        fBinWidth = 1. / fHeader.GetAcquisitionRate();

        // force monarch to use Separate interface
        fMonarch->SetInterface(sInterfaceSeparate);

        fSliceNumber = 0;

        return new KTEggHeader(fHeader);
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
            if (! fMonarch->ReadRecord())
            {
                KTERROR(eggreadlog, "File appears to contain no slices.");
                return KTDataPtr();
            }
            fReadState.fReadPtrOffset = 0;
            fReadState.fReadPtrRecordOffset = 0;
            fReadState.fSliceStartPtrOffset = 0;
            fReadState.fAbsoluteRecordOffset = 0;
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
            // If it's > 0, then it needs to be reduced by 1 because of how the offset number in Monarch::ReadRecord is used (offset=0 will advance to the next record)
            int readPtrRecordOffsetShift = int(sliceStartRecordOffset) - int(fReadState.fReadPtrRecordOffset);
            if (readPtrRecordOffsetShift != 0)
            {
                // change the absolute record offset first because it should be done before the adjustment to Monarch::ReadRecord offset counting is made
                fReadState.fAbsoluteRecordOffset += readPtrRecordOffsetShift;
                if (readPtrRecordOffsetShift != 0)
                {
                    --readPtrRecordOffsetShift;
                    KTDEBUG(eggreadlog, "Reading new record with offset " << readPtrRecordOffsetShift);
                    // move the read pointer to the slice start pointer (first move monarch to the correct record)
                    if (! fMonarch->ReadRecord(readPtrRecordOffsetShift))
                    {
                        KTWARN(eggreadlog, "End of egg file reached after reading new records (or something else went wrong)");
                        return KTDataPtr();
                    }
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
        vector< const MonarchRecord* > monarchRecords(nChannels);
        vector< KTTimeSeries* > newRecords(nChannels);
        for (unsigned iChannel = 0; iChannel < nChannels; ++iChannel)
        {
            monarchRecords[iChannel] = (fMonarch->*fMonarchGetRecord[iChannel])();
            sliceHeader.SetAcquisitionID(monarchRecords[iChannel]->fAcquisitionId, iChannel);
            sliceHeader.SetRecordID(monarchRecords[iChannel]->fRecordId, iChannel);
            sliceHeader.SetTimeStamp(monarchRecords[iChannel]->fTime, iChannel);

            //tsData->SetTimeSeries(new vector< DataType >(monarchRecord->fDataPtr, monarchRecord->fDataPtr+header->GetSliceSize()), iChannel);
            KTTimeSeries* newRecord;
            if (fTimeSeriesType == kRealTimeSeries)
            {
                newRecord = new KTTimeSeriesReal(fSliceSize, 0., double(fSliceSize) * sliceHeader.GetBinWidth());
            }
            else
            {
                newRecord = new KTTimeSeriesFFTW(fSliceSize, 0., double(fSliceSize) * sliceHeader.GetBinWidth());
            }
            newRecords[iChannel] = newRecord;
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
                newRecords[iChannel]->SetValue(iBin, double(monarchRecords[iChannel]->fData[fReadState.fReadPtrOffset]));
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


        // finally, set the records in the new data object
        KTTimeSeriesData& tsData = newData->Of< KTTimeSeriesData >().SetNComponents(nChannels);
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
        return;
    }

    double KTEggReaderMonarch::GetTimeInRunFirstCall() const
    {
        if ((fMonarch->*fMonarchGetRecord[0])()->fTime == 0)
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


