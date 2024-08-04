/*
 * KTEgg3Reader.cc
 *
 *  Created on: Jan 7, 2015
 *      Author: nsoblath
 */

#include "KTEgg3Reader.hh"

#include "KTArbitraryMetadata.hh"
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

#include "param.hh"
#include "param_codec.hh"
#include "scarab_version.hh"

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
            fStartRecord(0),
            fRequireMetadata(false),
            //fHatchNextSlicePtr(NULL),
            fFilenames(),
            fCurrentFileIt(),
            fMonarch(nullptr),
            fM3Stream(nullptr),
            fM3StreamHeader(nullptr),
            fHeaderPtr(new Nymph::KTData()),
            fHeader(fHeaderPtr->Of< KTEggHeader >()),
            fMasterSliceHeader(),
            fReadState(),
            fSampleRateUnitsInHz(1.e6),
            fRecordSize(0),
            fBinWidth(0.),
            fGetTimeInRun(&KTEgg3Reader::GetTimeInRunFromMonarch),
            fT0Offset(0),
            fAcqTimeInRun(0),
            fSliceNumber(0),
            fRecordsProcessed(0)
    {
        fReadState.fStatus = MonarchReadState::kInvalid;
        fReadState.fStartOfLastSliceRecord = 0;
        fReadState.fStartOfLastSliceReadPtr = 0;
        fReadState.fStartOfSliceAcquisitionId = 0;
        fReadState.fCurrentRecord = 0;
        fReadState.fFileNumber = 0;
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
        SetStartRecord(eggProc.GetStartRecord());
        SetRequireMetadata(eggProc.GetRequireMetadata());
        return true;
    }

    Nymph::KTDataPtr KTEgg3Reader::BreakEgg(const path_vec& filenames)
    {
        if (fStride == 0) fStride = fSliceSize;

        if (fMonarch != NULL)
        {
            delete fMonarch;
        }

        // copy the vector of filenames
        fFilenames = filenames;
        fCurrentFileIt = fFilenames.begin();

        // open the file
        //KTINFO(eggreadlog, "All egg files <" << fFilenames << ">");
        KTINFO(eggreadlog, "Opening egg file <" << fFilenames[0].first << ">");
        KTINFO(eggreadlog, "Variable type <" << typeid(fFilenames[0].first.string()).name() << ">");

        try
        {
            fMonarch = Monarch3::OpenForReading(fFilenames[0].first.native());
        }
        catch (M3Exception& e)
        {
            KTERROR(eggreadlog, "Unable to break egg: " << e.what());
            return Nymph::KTDataPtr();
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
            return Nymph::KTDataPtr();
        }

        // Check that this is an egg version 3 file
        scarab::version_semantic eggVersion(fMonarch->GetHeader()->GetEggVersion());
        if (eggVersion.major_version() != 3)
        {
            KTERROR(eggreadlog, "Cannot read egg version " << eggVersion.version_str());
            delete fMonarch;
            fMonarch = NULL;
            return Nymph::KTDataPtr();
        }

        // Set the appropriate time-in-run calculation function based on the egg version
        if (eggVersion < scarab::version_semantic("3.2.0"))
        {
            KTDEBUG(eggreadlog, "Egg version is " << eggVersion.version_str() << "; switching GetTIR function to manual");
            fGetTimeInRun = &KTEgg3Reader::GetTimeInRunManually;
        }
        else
        {
            KTDEBUG(eggreadlog, "Egg version is " << eggVersion.version_str() << "; switching GetTIR function to from-monarch");
            fGetTimeInRun = &KTEgg3Reader::GetTimeInRunFromMonarch;
        }

        // Temporary assumption: using channel 0 + any other channels in the same stream
        unsigned streamNum = fMonarch->GetHeader()->GetChannelStreams()[0];
        // set the stream pointer to stream 0
        fM3Stream = fMonarch->GetStream(streamNum);
        fM3StreamHeader = &(fMonarch->GetHeader()->GetStreamHeaders()[streamNum]);

        CopyHeader(fMonarch->GetHeader());
        fHeader.SetMetadataFilename(fFilenames[0].second.native());
        AddMetadata();
        if (fRequireMetadata && ! MetadataIsPresent())
        {
            KTERROR(eggreadlog, "Metadata is required but not present");
            delete fMonarch;
            fMonarch = nullptr;
            return Nymph::KTDataPtr();
        }

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
            return Nymph::KTDataPtr();
        }
        */

        fRecordSize = fHeader.GetChannelHeader(0)->GetRecordSize();
        fBinWidth = 1. / fHeader.GetAcquisitionRate();

        // by default, start the read state at the beginning of the run
        fReadState.fStatus = MonarchReadState::kAtStartOfRun;
        fReadState.fStartOfLastSliceRecord = 0;
        fReadState.fStartOfLastSliceReadPtr = 0;
        fReadState.fStartOfSliceAcquisitionId = 0;
        fReadState.fCurrentRecord = 0;


        KTDEBUG(eggreadlog, "Filenumber is :\n" << fReadState.fFileNumber);


        // skip forward in the run if fStartTime is non-zero
        if (fStartRecord == 0 && fStartTime > 0.)
        {
            double recordLength = fRecordSize * fBinWidth; // seconds
            unsigned recordSkips = (unsigned)(fStartTime / recordLength);
            fReadState.fStartOfLastSliceRecord = recordSkips;
            fReadState.fStartOfLastSliceReadPtr = (unsigned)((fStartTime - (double)recordSkips * recordLength) / fBinWidth);;
        }
        else if (fStartRecord != 0)
        {
            fReadState.fStartOfLastSliceRecord = fStartRecord;
            // fReadState.fStartOfLastSliceReadPtr stays 0
        }

        fSliceNumber = 0;

        fHeader.SetFileNumber(fReadState.fFileNumber+1); //Setting FileNumber in KTHeader 
        fHeader.SetFilename(fFilenames[0].first.string());

        // set a few values in the master slice header that don't change with each slice
        fMasterSliceHeader.SetSampleRate(fHeader.GetAcquisitionRate());
        fMasterSliceHeader.SetRawSliceSize(fSliceSize);
        fMasterSliceHeader.SetSliceSize(fSliceSize);
        fMasterSliceHeader.CalculateBinWidthAndSliceLength();
        fMasterSliceHeader.SetNonOverlapFrac((double)fStride / (double)fSliceSize);
        fMasterSliceHeader.SetRecordSize(fHeader.GetChannelHeader(0)->GetRecordSize());
        fMasterSliceHeader.SetFileNumber(fReadState.fFileNumber);
        fMasterSliceHeader.SetFilename(fFilenames[0].first.string());

        //KTDEBUG(eggreadlog, "Slice Header now: " << fMasterSliceHeader);
        //KTDEBUG(eggreadlog, "Slice Header filename now: " << fMasterSliceHeader.GetFilename());

        
        //KTDEBUG(eggreadlog, "Updated Filename: " << fHeader.GetFilename());
        //KTDEBUG(eggreadlog, "Updated MasterSliceHeader :\n" << fMasterSliceHeader);
        //if (LoadNextFile() == false)
        //{
        //    KTDEBUG(eggreadlog, "Filenumber is :\n" << fReadState.fFileNumber);
        //}


        return fHeaderPtr;
    }


    inline Nymph::KTDataPtr KTEgg3Reader::HatchNextSlice()
    {
        if (fMonarch == NULL)
        {
            KTERROR(eggreadlog, "Monarch file has not been opened");
            return Nymph::KTDataPtr();
        }
        if (fReadState.fStatus == MonarchReadState::kInvalid)
        {
            KTERROR(eggreadlog, "Read state status is <invalid>. Did you hatch the egg first?");
            return Nymph::KTDataPtr();
        }

        // Get the number of channels, record size and the number of bytes per sample from the stream
        unsigned nChannels = fM3Stream->GetNChannels();
        unsigned recordSize = fM3Stream->GetChannelRecordSize();
        unsigned sampleSize = fM3Stream->GetSampleSize();
        unsigned nBytesInSample = fM3Stream->GetDataTypeSize() * sampleSize;

        // the read position in the current record (initialize to 0 for now; will be set correctly below)
        unsigned readPos = 0;

        // flag to use later, which indicates if this new slice is in a new acquisition
        bool isNewAcquisition = false;

        try
        {
            if (fReadState.fStatus == MonarchReadState::kAtStartOfRun)
            {
                // Assumed values in fReadState:
                // - fStartOfLastSliceRecord and fStartOfLastSliceReadPtr are at the intended starting offset into the run.
                //   They may be (0,0), or they may specify some offset into the run.
                // - fCurrentRecord is 0

                // get the record start offset into the run
                int startingRecordShift = fReadState.fStartOfLastSliceRecord;
                // and set the read position
                readPos = fReadState.fStartOfLastSliceReadPtr;
                KTINFO(eggreadlog, "Starting offset into the run: record shift = " << startingRecordShift << "; read position = " << readPos);

                KTDEBUG(eggreadlog, "Reading first record");
                isNewAcquisition = true;

                // if we're at the beginning of the run, load the first record
                // second argument specifies that monarch should not go to the first record if it's a new acquisition
                if (! fM3Stream->ReadRecord(startingRecordShift, false))
                {
                    KTERROR(eggreadlog, "There's nothing in the file or the requested start is beyond the end of the (first) file");
                    return Nymph::KTDataPtr();
                }
                ++fRecordsProcessed;

                // set the time offset for the run based on the first channel
                fT0Offset = fM3Stream->GetAcqFirstRecordTime();
                KTDEBUG(eggreadlog, "Time offset of the first slice: " << fT0Offset << " ns");

                // set fStartOFLastSliceRecord properly, considering that the first record in the file might not be record 0
                // this has to be done after the first record is read, because Monarch only knows what the first record number is after accessing the records
                fReadState.fStartOfLastSliceRecord += fM3Stream->GetFirstRecordInFile();
                KTDEBUG(eggreadlog, "File starts with record " << fReadState.fStartOfLastSliceRecord);

                // the current record is the one we just loaded
                fReadState.fCurrentRecord = fReadState.fStartOfLastSliceRecord;

                // at this point, fReadState.fStartOfLastSliceRecord and fReadState.fStartOfLastSliceReadPtr are where they need to be
                // for the slice that will follow this one.
                // but we need to set fReadState.fStartOfSliceAcquisitionId
                fReadState.fStartOfSliceAcquisitionId = fM3Stream->GetAcquisitionId();

                fAcqTimeInRun = GetTimeInRun();

                fSliceNumber = 0;
            }
            else
            {
                // Assumed values in fReadState:
                // - fStartOfLastSliceRecord and fStartOfLastSliceReadPtr were left at the beginning of the last slice
                // - fCurrentRecord is the record # (from the start of the run) currently loaded in fStream0

                // determine the initial shift in records required to get to the start of the last slice from the current record
                int recordShift = (int)fReadState.fStartOfLastSliceRecord - (int)fReadState.fCurrentRecord;

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
                    bool inNewFile = false; // use this in addition to checking for an acquisition ID change below since the ID won't change if the entire file just finished has one acquisition
                    if (! fM3Stream->ReadRecord(recordShift - 1))  // 1 is subtracted since ReadRecord(0) goes to the next record
                    {
                        // we've reached the end of the file
                        if (! LoadNextFile())
                        {
                            KTINFO(eggreadlog, "End of egg file reached after reading new records");
                            return Nymph::KTDataPtr();
                        }
                        if (! fM3Stream->ReadRecord())
                        {
                            KTERROR(eggreadlog, "There's nothing in the file or the requested start is beyond the end of the file");
                            return Nymph::KTDataPtr();
                        }
                        inNewFile = true;
                    }
                    ++fRecordsProcessed;

                    // set the current record according to what's now loaded
                    fReadState.fCurrentRecord = fM3Stream->GetAcqFirstRecordId() + fM3Stream->GetRecordCountInAcq();
                    // check if we're in a new acquisition
                    if (fReadState.fStartOfSliceAcquisitionId != fM3Stream->GetAcquisitionId() || inNewFile)
                    {
                        KTDEBUG(eggreadlog, "Starting slice in a new acquisition: " << fM3Stream->GetAcquisitionId() << "; is a new file? " << inNewFile << "; Starting at record << " << fReadState.fCurrentRecord);
                        isNewAcquisition = true;
                        // then we need to start reading at the start of this record
                        readPos = 0;
                        // update these now (even though they're done just below) so we can accurately get the time in run
                        fReadState.fStartOfLastSliceRecord = fReadState.fCurrentRecord;
                        fReadState.fStartOfLastSliceReadPtr = readPos;
                        fAcqTimeInRun = GetTimeInRun();
                    }
                }

                // now set fStartOfLastSliceRecord and fStartOfLastSliceReadPtr for this slice (they'll be used next time around)
                fReadState.fStartOfLastSliceRecord = fReadState.fCurrentRecord;
                fReadState.fStartOfLastSliceReadPtr = readPos;
                // also set fStartOfSliceAcquisitionId
                fReadState.fStartOfSliceAcquisitionId = fM3Stream->GetAcquisitionId();

                ++fSliceNumber;
            }

            // at this point, the stream is ready for data to be copied from it, and fReadState is up-to-date

            // create the new data object
            Nymph::KTDataPtr newData(new Nymph::KTData());

            // add a slice header, and fill out the slice header information
            KTSliceHeader& sliceHeader = newData->Of< KTSliceHeader >();
            sliceHeader = fMasterSliceHeader;
            sliceHeader.SetIsNewAcquisition(isNewAcquisition);
            sliceHeader.SetTimeInRun(GetTimeInRun());
            sliceHeader.SetTimeInAcq(sliceHeader.GetTimeInRun() - fAcqTimeInRun);
            sliceHeader.SetSliceNumber(fSliceNumber);
            sliceHeader.SetStartRecordNumber(fReadState.fCurrentRecord);
            sliceHeader.SetStartSampleNumber(readPos);

            // create the raw time series objects that will contain the new copies of slices
            // and set some channel-specific slice header info
            vector< KTRawTimeSeries* > newSlices(nChannels);
            for (unsigned iChan = 0; iChan < nChannels; ++iChan)
            {
                // nBins = fSliceSize * sampleSize to allow for real and complex samples
                newSlices[iChan] = new KTRawTimeSeries(fM3Stream->GetDataTypeSize(),
                        ConvertMonarch3DataFormat(fM3StreamHeader->GetDataFormat()),
                        fSliceSize * sampleSize, 0., double(fSliceSize) * sliceHeader.GetBinWidth());
                newSlices[iChan]->SetSampleSize(sampleSize);

                sliceHeader.SetAcquisitionID(fM3Stream->GetAcquisitionId(), iChan);
                sliceHeader.SetRecordID(fM3Stream->GetChannelRecord( iChan )->GetRecordId(), iChan);
                sliceHeader.SetTimeStamp(fM3Stream->GetChannelRecord( iChan )->GetTime(), iChan);
                sliceHeader.SetRawDataFormatType(fHeader.GetChannelHeader( iChan )->GetDataFormat(), iChan);
            }

            KTDEBUG(eggreadlog, sliceHeader << "\nNote: some fields may not be filled in correctly yet");

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
                            fM3Stream->GetChannelRecord( iChan )->GetData() + readPosBytes,
                            bytesToCopy );
                }

                //*** DEBUG ***//
                /*
                std::stringstream readstream, writestream;
                M3DataReader< int64_t > readIfc(fM3Stream->GetChannelRecord( 0 )->GetData(), fHeader.GetChannelHeader(0)->GetDataTypeSize(), fHeader.GetChannelHeader(0)->GetDataFormat());
                KTVarTypePhysicalArray< int64_t > writeIfc = newSlices[0]->CreateInterface< int64_t >();
                for (unsigned iBin = 0; iBin < 30; ++iBin)
                {
                    std::cout << "read at " << readPos + iBin << " = " << readPos << " + " << iBin << ";    write at " << writePos + iBin << " = " << writePos << " + " << iBin << std::endl;
                    readstream << readIfc.at( readPos + iBin ) << "  ";
                    writestream << writeIfc( writePos + iBin ) << "  ";
                }
                KTWARN(eggreadlog, "Reading:  " << readstream.str());
                KTWARN(eggreadlog, "Writing:  " << writestream.str());
                */
                //*** DEBUG ***//

                // update samplesRemainingToCopy
                samplesRemainingToCopy -= samplesToCopyFromThisRecord;

                // check if there's still more to do
                if( samplesRemainingToCopy > 0 )
                {
                    bool inNewFile = false; // use this in addition to checking for an acquisition ID change below since the ID won't change if the entire file just finished has one acquisition
                    // move to the next record
                    if (! fM3Stream->ReadRecord())
                    {
                        if (! LoadNextFile())
                        {
                            KTINFO(eggreadlog, "End of file reached in the middle of reading out a slice");
                            return Nymph::KTDataPtr();
                        }
                        if (! fM3Stream->ReadRecord())
                        {
                            KTERROR(eggreadlog, "There's nothing in the file or the requested start is beyond the end of the file");
                            return Nymph::KTDataPtr();
                        }
                        inNewFile = true;
                    }
                    ++fRecordsProcessed;
                    fReadState.fCurrentRecord = fM3Stream->GetAcqFirstRecordId() + fM3Stream->GetRecordCountInAcq();

                    readPos = 0; // reset the read position, which is now at the beginning of the new record

                    // check if we've moved to a new acquisition
                    if (fReadState.fStartOfSliceAcquisitionId != fM3Stream->GetAcquisitionId() || inNewFile)
                    {
                        KTDEBUG(eggreadlog, "New acquisition reached; starting slice again\n" <<
                                "\tUnused samples: " << writePos + samplesToCopyFromThisRecord);
                        // now we need to start the slice over with the now-current record
                        writePos = 0;
                        samplesRemainingToCopy = fSliceSize;

                        // reset fStartOfLastSliceRecord and fStartOfLastSliceReadPtr for this slice (they'll be used next time around)
                        fReadState.fStartOfLastSliceRecord = fReadState.fCurrentRecord;
                        fReadState.fStartOfLastSliceReadPtr = readPos;
                        // also reset fStartOfSliceAcquisitionId
                        fReadState.fStartOfSliceAcquisitionId = fM3Stream->GetAcquisitionId();

                        // reset slice data
                        sliceHeader.SetIsNewAcquisition(true);
                        sliceHeader.SetStartRecordNumber(fReadState.fCurrentRecord);
                        sliceHeader.SetStartSampleNumber(readPos);
                        for (unsigned iChan = 0; iChan < nChannels; ++iChan)
                        {
                            sliceHeader.SetAcquisitionID(fM3Stream->GetAcquisitionId(), iChan);
                            sliceHeader.SetRecordID(fM3Stream->GetChannelRecord( iChan )->GetRecordId(), iChan);
                            sliceHeader.SetTimeStamp(fM3Stream->GetChannelRecord( iChan )->GetTime(), iChan);
                        }
                        sliceHeader.SetTimeInRun(GetTimeInRun());
                        fAcqTimeInRun = sliceHeader.GetTimeInRun();
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
        catch (M3Exception& e)
        {
            KTERROR(eggreadlog, "Error while hatching a slice: " << e.what());
            return Nymph::KTDataPtr();
        }
    }

    bool KTEgg3Reader::LoadNextFile()
    {
        KTDEBUG(eggreadlog, "Attempting to load next file");

        // advance the iterator and check if we're done with the list of files
        ++fCurrentFileIt;
        //++fReadState.fFileNumber;

        if (fCurrentFileIt == fFilenames.end())
        {
            KTINFO(eggreadlog, "There are no more files to open");
            return false;
        }

        // close the last file
        if (! CloseEgg())
        {
            return false;
        }

        // open the next file
        KTINFO(eggreadlog, "Opening next egg file <" << fCurrentFileIt->first << ">");
        try
        {
            fMonarch = Monarch3::OpenForReading(fCurrentFileIt->first.native());
        }
        catch (M3Exception& e)
        {
            KTERROR(eggreadlog, "Unable to open the file: " << e.what());
            return false;
        }

        KTDEBUG(eggreadlog, "File open; reading header");
        try
        {
            fMonarch->ReadHeader();
        }
        catch (M3Exception& e)
        {
            KTERROR(eggreadlog, "Header was not read correctly: " << e.what() << '\n' <<
                    "file-opening aborted.");
            delete fMonarch;
            fMonarch = nullptr;
            return false;
        }

        // Temporary assumption: using channel 0 + any other channels in the same stream
        unsigned streamNum = fMonarch->GetHeader()->GetChannelStreams()[0];
        // set the stream pointer to stream 0
        fM3Stream = fMonarch->GetStream(streamNum);
        fM3StreamHeader = &(fMonarch->GetHeader()->GetStreamHeaders()[streamNum]);

        // by default, start the read state at the beginning of the file
        fReadState.fStatus = MonarchReadState::kAtStartOfRun;
        fReadState.fStartOfLastSliceRecord = 0;
        fReadState.fStartOfLastSliceReadPtr = 0;
        fReadState.fStartOfSliceAcquisitionId = 0;
        fReadState.fCurrentRecord = 0; 

        return true;
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


    void KTEgg3Reader::CopyHeader(const M3Header* monarchHeader)
    {
        fHeader.SetFilename(monarchHeader->GetFilename());
        fHeader.SetAcquisitionMode(fM3StreamHeader->GetNChannels());
        fHeader.SetRunDuration(monarchHeader->GetRunDuration()); // in ms
        fHeader.SetAcquisitionRate(fM3StreamHeader->GetAcquisitionRate() * fSampleRateUnitsInHz);
        fHeader.SetTimestamp(monarchHeader->GetTimestamp());
        fHeader.SetDescription(monarchHeader->GetDescription());

        // NOTE: the egg header has fields for the min, max, and center frequencies
        //       these don't really make sense for the file-wide header
        //       the values are pulled from the first channel found
        bool haveSetHeaderFreqs = false;

        fHeader.SetNChannels(fM3StreamHeader->GetNChannels());
        unsigned streamNum = fM3StreamHeader->GetNumber();
        unsigned iChanInKatydid = 0;
        // loop over all of the channels in the file, and use the map of channel # to stream # to find the channels in this stream
        for (unsigned iChanInFile = 0; iChanInFile < monarchHeader->GetNChannels(); ++iChanInFile)
        {
            if (monarchHeader->GetChannelStreams()[iChanInFile] == streamNum)
            {
                KTDEBUG(eggreadlog, "Adding channel " << iChanInFile << " in the egg file");
                const M3ChannelHeader& channelHeader = monarchHeader->GetChannelHeaders()[iChanInFile];
                KTChannelHeader* newChanHeader = new KTChannelHeader();
                if (! haveSetHeaderFreqs)
                {
                    // NOTE: here we assume that channel's center frequency and frequency width are valid for the whole egg file
                    fHeader.SetMinimumFrequency(channelHeader.GetFrequencyMin());
                    fHeader.SetMaximumFrequency(channelHeader.GetFrequencyMin() + channelHeader.GetFrequencyRange());
                    fHeader.SetCenterFrequency(0.5 * channelHeader.GetFrequencyRange() + fHeader.GetMinimumFrequency());
                    KTDEBUG(eggreadlog, "Extracted frequencies from channel (in file) " << iChanInFile <<
                            ";  min freq: " << fHeader.GetMinimumFrequency() <<
                            ";  center freq: " << fHeader.GetCenterFrequency() <<
                            ";  max freq: " << fHeader.GetMaximumFrequency() );
                    haveSetHeaderFreqs = true;
                }
                newChanHeader->SetNumber(iChanInKatydid);
                newChanHeader->SetSource(channelHeader.GetSource());
                newChanHeader->SetRawSliceSize(fSliceSize);
                newChanHeader->SetSliceSize(fSliceSize);
                newChanHeader->SetSliceStride(fStride);
                newChanHeader->SetRecordSize(fM3StreamHeader->GetRecordSize());
                unsigned sampleSize = channelHeader.GetSampleSize();
                newChanHeader->SetSampleSize(sampleSize);
                newChanHeader->SetDataTypeSize(channelHeader.GetDataTypeSize());
                newChanHeader->SetDataFormat(ConvertMonarch3DataFormat(channelHeader.GetDataFormat()));
                newChanHeader->SetBitDepth(channelHeader.GetBitDepth());
                newChanHeader->SetBitAlignment(channelHeader.GetBitAlignment());
                newChanHeader->SetVoltageOffset(channelHeader.GetVoltageOffset());
                newChanHeader->SetVoltageRange(channelHeader.GetVoltageRange());
                newChanHeader->SetDACGain(channelHeader.GetDACGain());
                if (sampleSize == 1) newChanHeader->SetTSDataType(KTChannelHeader::kReal);
                else if (sampleSize == 2) newChanHeader->SetTSDataType(KTChannelHeader::kIQ);
                else
                {
                    KTWARN(eggreadlog, "Sample size <" << sampleSize << "> on channel " << iChanInFile << " may cause problems with downstream processing");
                }
                fHeader.SetChannelHeader(newChanHeader, iChanInKatydid);
                ++iChanInKatydid;
            }
        }

        // set the TS data type size based on channel 0 (by Katydid's channel counting)
        return;
    }

    void KTEgg3Reader::AddMetadata()
    {
        if (fHeader.GetMetadataFilename().empty())
        {
            fHeader.Of< KTArbitraryMetadata >().SetMetadata(nullptr);
            return;
        }

        scarab::param_translator translator;
        scarab::param* metadata = translator.read_file(fHeader.GetMetadataFilename());
        if (metadata == nullptr)
        {
            KTWARN(eggreadlog, "Metadata file was not present or could not be converted to a param object");
        }
#ifndef NDEBUG
        if (metadata != nullptr)
        {
            KTDEBUG(eggreadlog, "Adding metadata from <" << fHeader.GetMetadataFilename() << ">");
            KTDEBUG(eggreadlog, *metadata);
        }
#endif
        fHeader.Of< KTArbitraryMetadata >().SetMetadata(metadata);

        return;
    }

    bool KTEgg3Reader::MetadataIsPresent() const
    {
        if (! fHeader.Has< KTArbitraryMetadata >() ) return false;
        return fHeader.Of< KTArbitraryMetadata >().GetMetadata() != nullptr;
    }


    uint32_t ConvertMonarch3DataFormat( uint32_t m3DataFormat )
    {
        switch( m3DataFormat )
        {
            case monarch3::sDigitizedUS:
                return sDigitizedUS;
                break;
            case monarch3::sDigitizedS:
                return sDigitizedS;
                break;
            case monarch3::sAnalog:
                return sAnalog;
                break;
            default:
                return sInvalidFormat;
        }
    }


} /* namespace Katydid */


