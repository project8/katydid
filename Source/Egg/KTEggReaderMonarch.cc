/*
 * KTEgg.cc
 *
 *  Created on: Sep 9, 2011
 *      Author: nsoblath
 */

#include "KTEggReaderMonarch.hh"

#include "KTEgg.hh"
#include "KTLogger.hh"
#include "KTSliceHeader.hh"
#include "KTTimeSeriesData.hh"
#include "KTTimeSeriesFFTW.hh"
#include "KTTimeSeriesReal.hh"

#include "MonarchPP.hh"
#include "MonarchHeader.hpp"

using boost::shared_ptr;

using std::map;
using std::string;
using std::vector;

namespace Katydid
{
    KTLOGGER(eggreadlog, "katydid.egg");

    KTEggReaderMonarch::KTEggReaderMonarch() :
            KTEggReader(),
            fTimeSeriesType(kRealTimeSeries),
            fTimeSeriesSizeRequest(0),
            fMonarch(NULL),
            fHeader(),
            fReadState(),
            fNumberOfChannels(),
            fSampleRateUnitsInHz(1.e6),
            fFullVoltageScale(0.5),
            fNADCLevels(256),
            fRecordsRead(0),
            fRecordSize(0),
            fBinWidth(0.),
            fSliceNumber(0)
    {
        fReadState.fStatus = MonarchReadState::kInvalid;
        fReadState.fAcquisitionID = 0;
        fReadState.fDataPtrOffset = 0;

        fNumberOfChannels.insert(AcqModeMapValue(sOneChannel, 1));
        fNumberOfChannels.insert(AcqModeMapValue(sTwoChannel, 2));
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
        if (fMonarch != NULL)
        {
            fMonarch->Close();
            delete fMonarch;
        }

        // open the file
        KTDEBUG(eggreadlog, "Attempting to open file <" << filename << ">");
        fMonarch = MonarchPP::OpenForReading(filename);
        if (fMonarch == NULL)
        {
            KTERROR(eggreadlog, "Unable to break egg");
            return NULL;
        }

        KTDEBUG(eggreadlog, "File open; reading header");
        if (! fMonarch->ReadHeader())
        {
            KTERROR(eggreadlog, "Header was not read correctly; egg breaking aborted.");
            fMonarch->Close();
            delete fMonarch;
            fMonarch = NULL;
            return NULL;
        }
        CopyHeaderInformation(fMonarch->GetHeader());
        if (fTimeSeriesSizeRequest == 0)
        {
            fHeader.SetSliceSize(fHeader.GetRecordSize());
        }
        else
        {
            fHeader.SetSliceSize(fTimeSeriesSizeRequest);
        }

        KTDEBUG("Parsed header:\n"
             << "\tFilename: " << fHeader.GetFilename() << '\n'
             << "\tAcuisition Mode: " << fHeader.GetAcquisitionMode() << '\n'
             << "\tNumber of Channels: " << fHeader.GetNChannels() << '\n'
             << "\tRecord Size: " << fHeader.GetSliceSize() << '\n'
             << "\tRecord Size: " << fHeader.GetRecordSize() << '\n'
             << "\tAcquisition Time: " << fHeader.GetAcquisitionTime() << " s" << '\n'
             << "\tAcquisition Rate: " << fHeader.GetAcquisitionRate() << " Hz ");


        fReadState.fStatus = MonarchReadState::kAtStartOfRun;
        fReadState.fAcquisitionID = 0;
        fReadState.fDataPtrOffset = 0;

        fRecordsRead = 0;
        fRecordSize = fHeader.GetRecordSize();
        fBinWidth = 1. / fHeader.GetAcquisitionRate();

        fSliceNumber = 0;

        return new KTEggHeader(fHeader);
    }

    boost::shared_ptr< KTData > KTEggReaderMonarch::HatchNextSlice()
    {
        if (fMonarch == NULL)
        {
            KTERROR(eggreadlog, "Monarch file has not been opened");
            return shared_ptr< KTData >();
        }
        if (fReadState.fStatus == MonarchReadState::kInvalid)
        {
            KTERROR(eggreadlog, "Read state status is <invalid>. Did you hatch the egg first?");
            return shared_ptr< KTData >();
        }

        // if we're at the beginning of the run, load the first records
        if (fReadState.fStatus == MonarchReadState::kAtStartOfRun)
        {
            if (! fMonarch->ReadRecord())
            {
                KTERROR(eggreadlog, "File appears to contain no slices.");
                return shared_ptr< KTData >();
            }
            fRecordsRead = 0;
            fSliceNumber = 0;
            fReadState.fStatus = MonarchReadState::kContinueReading;
        }

        boost::shared_ptr< KTData > newData(new KTData());

        // Fill out slice header information
        KTSliceHeader& sliceHeader = newData->Of< KTSliceHeader >().SetNComponents(fHeader.GetNChannels());
        sliceHeader.SetSampleRate(fHeader.GetAcquisitionRate());
        sliceHeader.SetSliceSize(fHeader.GetSliceSize());
        sliceHeader.CalculateBinWidthAndSliceLength();
        sliceHeader.SetTimeInRun(GetTimeInRun());
        sliceHeader.SetSliceNumber(fSliceNumber);

        // Normalization of the record values
        Double_t normalization = fFullVoltageScale / (Double_t)fNADCLevels;

        // Setup pointers to monarch and new katydid records
        vector< const MonarchRecord* > monarchRecords(fHeader.GetNChannels());
        vector< KTTimeSeries* > newRecords(fHeader.GetNChannels());
        for (UInt_t iChannel = 0; iChannel < fHeader.GetNChannels(); iChannel++)
        {
            monarchRecords[iChannel] = fMonarch->GetRecord(iChannel);
            sliceHeader.SetAcquisitionID(monarchRecords[iChannel]->fAId, iChannel);
            sliceHeader.SetRecordID(monarchRecords[iChannel]->fRId, iChannel);
            sliceHeader.SetTimeStamp(monarchRecords[iChannel]->fTick, iChannel);

            //tsData->SetTimeSeries(new vector< DataType >(monarchRecord->fDataPtr, monarchRecord->fDataPtr+header->GetSliceSize()), iChannel);
            KTTimeSeries* newRecord;
            if (fTimeSeriesType == kRealTimeSeries)
            {
                newRecord = new KTTimeSeriesReal(fHeader.GetSliceSize(), 0., Double_t(fHeader.GetSliceSize()) * sliceHeader.GetBinWidth());
            }
            else
            {
                newRecord = new KTTimeSeriesFFTW(fHeader.GetSliceSize(), 0., Double_t(fHeader.GetSliceSize()) * sliceHeader.GetBinWidth());
            }
            newRecords[iChannel] = newRecord;
        }

        KTDEBUG(eggreadlog, "Time in run: " << GetTimeInRun() << " s\n" <<
                "\tBin width = " << fBinWidth << '\n' <<
                "\tMonarch records read = " << fRecordsRead << '\n' <<
                "\tMonarch record size = " << fRecordSize << '\n' <<
                "\tPointer offset = " << fReadState.fDataPtrOffset);

        // Loop over bins
        for (UInt_t iBin = 0; iBin < fHeader.GetSliceSize(); iBin++)
        {
            if (fReadState.fStatus == MonarchReadState::kAtEndOfFile)
            {
                KTINFO(eggreadlog, "End of file reached.\n"
                        << "\tNumber of unused bins: " << iBin - 1);
                for (UInt_t iChannel = 0; iChannel < fHeader.GetNChannels(); iChannel++)
                {
                    delete newRecords[iChannel];
                }
                return shared_ptr< KTData >();
            }
            else if (fReadState.fStatus == MonarchReadState::kAcquisitionIDHasChanged)
            {
                // this means that a new monarch record has come up, and it has a different acquisition id.
                // in this situation we need to start the time series over with the new monarch record
                KTDEBUG(eggreadlog, "Acquisition ID change; resetting slice to start with this monarch record.\n"
                        << "\tNumber of unused bins: " << iBin - 1);
                // reset slice data
                for (UInt_t iChannel = 0; iChannel < fHeader.GetNChannels(); iChannel++)
                {
                    sliceHeader.SetAcquisitionID(monarchRecords[iChannel]->fAId, iChannel);
                    sliceHeader.SetRecordID(monarchRecords[iChannel]->fRId, iChannel);
                    sliceHeader.SetTimeStamp(monarchRecords[iChannel]->fTick, iChannel);
                }
                // reset bin count to 0
                iBin = 0;
                // change the time in run since we're going back to the beginning of the record
                sliceHeader.SetTimeInRun(GetTimeInRun());
                KTDEBUG(eggreadlog, "Correction to time in run: " << GetTimeInRun() << " s\n" <<
                        "\tBin width = " << fBinWidth << '\n' <<
                        "\tMonarch records read = " << fRecordsRead << '\n' <<
                        "\tMonarch record size = " << fRecordSize << '\n' <<
                        "\tPointer offset = " << fReadState.fDataPtrOffset);
                sliceHeader.SetSliceNumber(fSliceNumber);
                // change status
                fReadState.fStatus = MonarchReadState::kContinueReading;
            }

            // Read the data from the records
            for (UInt_t iChannel = 0; iChannel < fHeader.GetNChannels(); iChannel++)
            {
                // set the data
                newRecords[iChannel]->SetValue(iBin, Double_t(monarchRecords[iChannel]->fDataPtr[fReadState.fDataPtrOffset]) * normalization);
            }

            // advance the pointer
            fReadState.fDataPtrOffset++;
            // check if we've reached the end of a monarch record
            if (fReadState.fDataPtrOffset >= fHeader.GetRecordSize())
            {
                KTDEBUG(eggreadlog, "End of Monarch record reached; attempting to read a new one.");
                // try reading the next record
                if (! fMonarch->ReadRecord())
                {
                    // the end of the file has been reached
                    fReadState.fStatus = MonarchReadState::kAtEndOfFile;
                    KTDEBUG(eggreadlog, "End of egg file reached after reading new records.");
                }
                else
                {
                    // check if the acquisition ID has changed on any channel
                    if (fReadState.fAcquisitionID != monarchRecords[0]->fAId)
                    {
                        fReadState.fAcquisitionID = monarchRecords[0]->fAId;
                        fReadState.fStatus = MonarchReadState::kAcquisitionIDHasChanged;
                        KTDEBUG(eggreadlog, "New acquisition ID found: " << fReadState.fAcquisitionID);
                    }
                    fRecordsRead++;
                }
                fReadState.fDataPtrOffset = 0;
            }
        } // end loop over bins

        // finally, set the records in the new data object
        KTTimeSeriesData& tsData = newData->Of< KTTimeSeriesData >().SetNComponents(sliceHeader.GetNComponents());
        for (UInt_t iChannel = 0; iChannel < fHeader.GetNChannels(); iChannel++)
        {
            tsData.SetTimeSeries(newRecords[iChannel], iChannel);
        }

        fSliceNumber++;

        return newData;
    }

    Bool_t KTEggReaderMonarch::CloseEgg()
    {
        Bool_t fileExistedAndWasClosed = fMonarch->Close();
        delete fMonarch;
        fMonarch = NULL;
        return fileExistedAndWasClosed;
    }


    void KTEggReaderMonarch::CopyHeaderInformation(const MonarchHeader* monarchHeader)
    {
        fHeader.SetFilename(monarchHeader->GetFilename());
        fHeader.SetAcquisitionMode(monarchHeader->GetAcqMode());
        fHeader.SetNChannels(fNumberOfChannels[fHeader.GetAcquisitionMode()]);
        fHeader.SetRecordSize(monarchHeader->GetRecordSize());
        fHeader.SetAcquisitionTime(monarchHeader->GetAcqTime());
        fHeader.SetAcquisitionRate(monarchHeader->GetAcqRate() * fSampleRateUnitsInHz);
        return;
    }

} /* namespace Katydid */


