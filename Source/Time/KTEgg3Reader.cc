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
            //fHatchNextSlicePtr(NULL),
            fMonarch(NULL),
            fStream0(NULL),
            fStreamHeader0(NULL),
            fHeaderPtr(new KTData()),
            fHeader(fHeaderPtr->Of< KTEggHeader >()),
            fMasterSliceHeader(),
            fReadState(),
            fGetTimeInRun(&KTEgg3Reader::GetTimeInRunFirstCall),
            fT0Offset(0),
            fSampleRateUnitsInHz(1.e6),
            fRecordSize(0),
            fBinWidth(0.),
            fSliceNumber(0)
    {
        fReadState.fStatus = MonarchReadState::kInvalid;
        fReadState.fStartOfLastSliceRecord = 0;
        fReadState.fStartOfLastSliceReadPtr = 0;
        fReadState.fStartOfSliceAcquisitionId = 0;
        fReadState.fCurrentRecord = 0;
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

        fStreamHeader0 = &(fMonarch->GetHeader()->GetStreamHeaders()[0]);

        KTDEBUG(eggreadlog, "Parsed header:\n" << fHeader);

        /*
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
        */

        fRecordSize = fHeader.GetRecordSize();
        fBinWidth = 1. / fHeader.GetAcquisitionRate();

        fHeader.SetMinimumFrequency(0.0);
        fHeader.SetMaximumFrequency(fHeader.GetAcquisitionRate()/2.0);
        fHeader.SetCenterFrequency((fHeader.GetMaximumFrequency() - fHeader.GetMinimumFrequency())/2.0);

        // by default, start the read state at the beginning of the run
        fReadState.fStatus = MonarchReadState::kAtStartOfRun;
        fReadState.fStartOfLastSliceRecord = 0;
        fReadState.fStartOfLastSliceReadPtr = 0;
        fReadState.fStartOfSliceAcquisitionId = 0;
        fReadState.fCurrentRecord = 0;

        // skip forward in the run if fStartTime is non-zero
        if (fStartTime > 0.)
        {
            double recordLength = fRecordSize * fBinWidth; // seconds
            unsigned recordSkips = (unsigned)(fStartTime / recordLength);
            fReadState.fStartOfLastSliceRecord = recordSkips;
            fReadState.fStartOfLastSliceReadPtr = (unsigned)((fStartTime - (double)recordSkips * recordLength) / fBinWidth);;
        }

        fSliceNumber = 0;

        // set a few values in the master slice header that don't change with each slice
        fMasterSliceHeader.SetSampleRate(fHeader.GetAcquisitionRate());
        fMasterSliceHeader.SetRawSliceSize(fSliceSize);
        fMasterSliceHeader.SetSliceSize(fSliceSize);
        fMasterSliceHeader.CalculateBinWidthAndSliceLength();
        fMasterSliceHeader.SetNonOverlapFrac((double)fStride / (double)fSliceSize);
        fMasterSliceHeader.SetRecordSize(fHeader.GetRecordSize());

        // set the stream pointer to stream 0
        fStream0 = fMonarch->GetStream(0);

        return fHeaderPtr;
    }


    inline KTDataPtr KTEgg3Reader::HatchNextSlice()
    {
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

        // Get the number of channels, record size and the number of bytes per sample from the stream
        unsigned nChannels = fStream0->GetNChannels();
        unsigned recordSize = fStream0->GetChannelRecordSize();
        unsigned nBytesInSample = fStream0->GetDataTypeSize() * fStream0->GetSampleSize();

        // the read position in the current record (initialize to 0 for now; will be set correctly below)
        unsigned readPos = 0;

        // flag to use later, which indicates if this new slice is in a new acquisition
        bool isNewAcquisition = false;

        if (fReadState.fStatus == MonarchReadState::kAtStartOfRun)
        {
            // Assumed values in fReadState:
            // - fStartOfLastSliceRecord and fStartOfLastSliceReadPtr are at the intended starting position in the run.
            //   They may be (0,0), or they may specify some offset into the run.
            // - fCurrentRecord is 0

            KTDEBUG(eggreadlog, "Reading first record");
            isNewAcquisition = true;

            // if we're at the beginning of the run, load the first record
            if (! fStream0->ReadRecord(fReadState.fStartOfLastSliceRecord))
            {
                KTERROR(eggreadlog, "There's nothing in the file or the requested start is beyond the end of the file");
                return KTDataPtr();
            }
            // and set the read position
            readPos = fReadState.fStartOfLastSliceReadPtr;

            // the current record is the one we just loaded
            fReadState.fCurrentRecord = fReadState.fStartOfLastSliceRecord;

            // at this point, fReadState.fStartOfLastSliceRecord and fReadState.fStartOfLastSliceReadPtr are where they need to be
            // for the slice that will follow this one.
            // but we need to set fReadState.fStartOfSliceAcquisitionId
            fReadState.fStartOfSliceAcquisitionId = fStream0->GetAcquisitionId();

            fSliceNumber = 0;
        }
        else
        {
            // Assumed values in fReadState:
            // - fStartOfLastSliceRecord and fStartOfLastSliceReadPtr were left at the beginning of the last slice
            // - fCurrentRecord is the record # (from the start of the run) currently loaded in fStream0

            // determine the initial shift in records required to get to the start of the last slice from the current record
            int recordShift = (int)fReadState.fCurrentRecord - (int)fReadState.fStartOfLastSliceRecord;

            // advance the read position from the last slice by the stride
            readPos = fReadState.fStartOfLastSliceReadPtr + fStride;

            // check whether we have to advance to a new record
            while (readPos >= recordSize)
            {
                readPos -= recordSize;
                ++recordShift;
            }
            KTDEBUG(eggreadlog, "Preparing to read next slice; Record shift: " << recordShift << "; Read position (in record): " << readPos);
            // readPos is now the read position in the new record that will be loaded (or the current record, if we stay on this record)

            // if necessary, move to a new record
            if( recordShift != 0 )
            {
                if (! fStream0->ReadRecord(recordShift - 1))  // 1 is subtracted since ReadRecord(0) goes to the next record
                {
                    KTWARN(eggreadlog, "End of egg file reached after reading new records (or something else went wrong)");
                    return KTDataPtr();
                }
                // set the current record according to what's now loaded
                fReadState.fCurrentRecord = fReadState.fCurrentRecord + recordShift;
                // check if we're in a new acquisition
                if (fReadState.fStartOfSliceAcquisitionId != fStream0->GetAcquisitionId())
                {
                    KTDEBUG(eggreadlog, "Starting slice in a new acquisition: " << fStream0->GetAcquisitionId());
                    isNewAcquisition = true;
                    // then we need to start reading at the start of this record
                    readPos = 0;
                }
            }

            // now set fStartOfLastSliceRecord and fStartOfLastSliceReadPtr for this slice (they'll be used next time around)
            fReadState.fStartOfLastSliceRecord = fReadState.fCurrentRecord;
            fReadState.fStartOfLastSliceReadPtr = readPos;
            // also set fStartOfSliceAcquisitionId
            fReadState.fStartOfSliceAcquisitionId = fStream0->GetAcquisitionId();

            ++fSliceNumber;
        }

        // at this point, the stream is ready for data to be copied from it, and fReadState is up-to-date

        // create the new data object
        KTDataPtr newData(new KTData());

        // add a slice header, and fill out the slice header information
        KTSliceHeader& sliceHeader = newData->Of< KTSliceHeader >();
        sliceHeader = fMasterSliceHeader;
        sliceHeader.SetIsNewAcquisition(isNewAcquisition);
        sliceHeader.SetTimeInRun(GetTimeInRun());
        sliceHeader.SetSliceNumber(fSliceNumber);
        sliceHeader.SetStartRecordNumber(fReadState.fCurrentRecord);
        sliceHeader.SetStartSampleNumber(readPos);
        KTDEBUG(eggreadlog, sliceHeader << "\nNote: some fields may not be filled in correctly yet");

        // create the raw time series objects that will contain the new copies of slices
        // and set some channel-specific slice header info
        vector< KTRawTimeSeries* > newSlices(nChannels);
        for (unsigned iChan = 0; iChan < nChannels; ++iChan)
        {
            newSlices[iChan] = new KTRawTimeSeries(fStream0->GetDataTypeSize(),
                    ConvertMonarch3DataFormat(fStreamHeader0->GetDataFormat()),
                    fSliceSize, 0., double(fSliceSize) * sliceHeader.GetBinWidth());

            sliceHeader.SetAcquisitionID(fStream0->GetAcquisitionId(), iChan);
            sliceHeader.SetRecordID(fStream0->GetChannelRecord( iChan )->GetRecordId(), iChan);
            sliceHeader.SetTimeStamp(fStream0->GetChannelRecord( iChan )->GetTime(), iChan);
        }

        // the write position on the new slice
        unsigned writePos = 0;

        // the number of samples still to copy to the new slice
        unsigned samplesRemainingToCopy = fSliceSize;

        // the last sample that will be copied in a record, for each copy
        // the motivation for calculating this (at least initially) is to know the last (record, sample) pair for the slice
        unsigned lastSampleCopied = 0;

        fReadState.fStatus = MonarchReadState::kContinueReading;

        //****************************************
        // loop until all samples have been copied
        //****************************************
        while( samplesRemainingToCopy > 0 )
        {
            // calculate the end-of-slice position
            unsigned endOfSlicePos = readPos + samplesRemainingToCopy; // this is 1 past the last sample in the slice

            // calculate the number of samples to copy from this record
            unsigned samplesToCopyFromThisRecord = samplesRemainingToCopy;
            // check if the slice goes beyond the end of the record, and if so, reduce the number of samples to copy from this record
            if( endOfSlicePos > recordSize )
            {
                // samplesBeyondRecord = endOfSlicePos - recordSize;
                samplesToCopyFromThisRecord -= endOfSlicePos - recordSize;
            }
            lastSampleCopied = readPos + samplesToCopyFromThisRecord - 1;

            // number of bytes to copy from this record
            unsigned bytesToCopy = samplesToCopyFromThisRecord * nBytesInSample;

            // write position in bytes
            unsigned writePosBytes = writePos * nBytesInSample;
            unsigned readPosBytes = readPos * nBytesInSample;

            // copy the samples from this record for each channel
            KTDEBUG(eggreadlog, "Copying data to slice; " << bytesToCopy << " bytes in " << samplesToCopyFromThisRecord << " samples");
            for( unsigned iChan = 0; iChan < nChannels; ++iChan )
            {
                memcpy( newSlices[iChan]->GetStorage() + writePosBytes,
                        fStream0->GetChannelRecord( iChan )->GetData() + readPosBytes,
                        bytesToCopy );
            }

            //*** DEBUG ***//
            //std::stringstream readstream, writestream;
            //M3DataReader< int64_t > readIfc(fStream0->GetChannelRecord( 0 )->GetData(), fHeader.GetDataTypeSize(), fHeader.GetFormatMode());
            //KTVarTypePhysicalArray< int64_t > writeIfc = newSlices[0]->CreateInterface< int64_t >();
            //for (unsigned iBin = 0; iBin < 10; ++iBin)
            //{
            //    std::cout << "read at " << readPos + iBin << " = " << readPos << " + " << iBin << ";    write at " << writePos + iBin << " = " << writePos << " + " << iBin << std::endl;
            //    readstream << readIfc.at( readPos + iBin ) << "  ";
            //    writestream << writeIfc( writePos + iBin ) << "  ";
            //}
            //KTWARN(eggreadlog, "Reading:  " << readstream.str());
            //KTWARN(eggreadlog, "Writing:  " << writestream.str());
            //*** DEBUG ***//

            // update samplesRemainingToCopy
            samplesRemainingToCopy -= samplesToCopyFromThisRecord;

            // check if there's still more to do
            if( samplesRemainingToCopy > 0 )
            {
                // move to the next record
                if (! fStream0->ReadRecord())
                {
                    KTWARN(eggreadlog, "End of file reached before slice was completely read (or something else went wrong)");
                    return KTDataPtr();
                }
                fReadState.fCurrentRecord = fReadState.fCurrentRecord + 1;

                readPos = 0; // reset the read position, which is now at the beginning of the new record

                // check if we've moved to a new acquisition
                if (fReadState.fStartOfSliceAcquisitionId != fStream0->GetAcquisitionId())
                {
                    KTDEBUG(eggreadlog, "New acquisition reached; starting slice again\n" <<
                            "\tUnused samples: " << writePos + samplesToCopyFromThisRecord)
                    // now we need to start the slice over with the now-current record
                    writePos = 0;
                    samplesRemainingToCopy = fSliceSize;

                    // reset fStartOfLastSliceRecord and fStartOfLastSliceReadPtr for this slice (they'll be used next time around)
                    fReadState.fStartOfLastSliceRecord = fReadState.fCurrentRecord;
                    fReadState.fStartOfLastSliceReadPtr = 0;
                    // also reset fStartOfSliceAcquisitionId
                    fReadState.fStartOfSliceAcquisitionId = fStream0->GetAcquisitionId();

                    // reset slice data
                    sliceHeader.SetIsNewAcquisition(true);
                    sliceHeader.SetStartRecordNumber(fReadState.fCurrentRecord);
                    sliceHeader.SetStartSampleNumber(readPos);
                    for (unsigned iChan = 0; iChan < nChannels; ++iChan)
                    {
                        sliceHeader.SetAcquisitionID(fStream0->GetAcquisitionId(), iChan);
                        sliceHeader.SetRecordID(fStream0->GetChannelRecord( iChan )->GetRecordId(), iChan);
                        sliceHeader.SetTimeStamp(fStream0->GetChannelRecord( iChan )->GetTime(), iChan);
                    }
                    sliceHeader.SetTimeInRun(GetTimeInRun());
                    KTDEBUG(eggreadlog, "Correction to time in run: " << GetTimeInRun() << " s\n" <<
                            "\tCurent record = " << fReadState.fCurrentRecord << '\n' <<
                            "\tSlice start sample in record = " << readPos);
                } // end if-block dealing with new acquisitions

                // and now we're ready to loop back to copy samples from the new record
            } // end if-block used when there are still samples left to copy for this slice

        } // end while-loop responsible for copying samples to the slice

        // at this point, all of the samples have been copied into the new slice

        // the slice header gets information about where the slice ended
        sliceHeader.SetEndRecordNumber(fReadState.fCurrentRecord);
        sliceHeader.SetEndSampleNumber(lastSampleCopied);

        // finally, set the records in the new data object
        KTRawTimeSeriesData& tsData = newData->Of< KTRawTimeSeriesData >().SetNComponents(nChannels);
        for (unsigned iChannel = 0; iChannel < nChannels; ++iChannel)
        {
            tsData.SetTimeSeries(newSlices[iChannel], iChannel);
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


