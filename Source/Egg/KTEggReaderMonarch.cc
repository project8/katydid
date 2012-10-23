/*
 * KTEgg.cc
 *
 *  Created on: Sep 9, 2011
 *      Author: nsoblath
 */

#include "KTEggReaderMonarch.hh"

#include "KTEgg.hh"
#include "KTLogger.hh"
#include "KTTimeSeriesDataFFTW.hh"
#include "KTTimeSeriesDataReal.hh"
#include "KTTimeSeriesFFTW.hh"
#include "KTTimeSeriesReal.hh"

#include "MonarchPP.hh"
#include "MonarchHeader.hpp"

using std::map;
using std::string;
using std::vector;

namespace Katydid
{
    KTLOGGER(eggreadlog, "katydid.egg");

    KTEggReaderMonarch::KTEggReaderMonarch() :
            KTEggReader(),
            fTimeSeriesType(kRealTimeSeries),
            fTimeSeriesSize(0),
            fMonarch(NULL),
            fHeader(),
            fReadState(),
            fNumberOfChannels(),
            fSampleRateUnitsInHz(1.e6),
            fFullVoltageScale(0.5),
            fNADCLevels(256)
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
        fHeader.SetRecordSize(fTimeSeriesSize);
        CopyHeaderInformation(fMonarch->GetHeader());

        fReadState.fStatus = MonarchReadState::kAtStartOfRun;
        fReadState.fAcquisitionID = 0;
        fReadState.fDataPtrOffset = 0;

        return new KTEggHeader(fHeader);
    }

    KTTimeSeriesData* KTEggReaderMonarch::HatchNextEvent()
    {
        if (fMonarch == NULL)
        {
            KTERROR(eggreadlog, "Monarch file has not been opened");
            return NULL;
        }
        if (fReadState.fStatus == MonarchReadState::kInvalid)
        {
            KTERROR(eggreadlog, "Read state status is <invalid>. Did you hatch the egg first?");
            return NULL;
        }

        // if we're at the beginning of the run, load the first records
        if (fReadState.fStatus == MonarchReadState::kAtStartOfRun)
        {
            if (! fMonarch->ReadRecord())
            {
                KTERROR(eggreadlog, "File appears to contain no events.");
                return NULL;
            }
            fReadState.fStatus = MonarchReadState::kContinueReading;
        }

        KTTimeSeriesData* eventData;
        if (fTimeSeriesType == kRealTimeSeries)
        {
            eventData = new KTTimeSeriesDataReal(fHeader.GetNChannels());
        }
        else
        {
            eventData = new KTTimeSeriesDataFFTW(fHeader.GetNChannels());
        }

        // Fill out event information
        eventData->SetSampleRate(fHeader.GetAcquisitionRate());
        eventData->SetRecordSize(fHeader.GetRecordSize());
        eventData->CalculateBinWidthAndRecordLength();

        // Normalization of the record values
        Double_t normalization = fFullVoltageScale / (Double_t)fNADCLevels;

        // Setup pointers to monarch and new katydid records
        vector< const MonarchRecord* > monarchRecords(fHeader.GetNChannels());
        vector< KTTimeSeries* > newRecords(fHeader.GetNChannels());
        for (UInt_t iChannel = 0; iChannel < fHeader.GetNChannels(); iChannel++)
        {
            monarchRecords[iChannel] = fMonarch->GetRecord(iChannel);
            eventData->SetChannelID(monarchRecords[iChannel]->fCId, iChannel);
            eventData->SetAcquisitionID(monarchRecords[iChannel]->fAId, iChannel);
            eventData->SetRecordID(monarchRecords[iChannel]->fRId, iChannel);
            eventData->SetTimeStamp(monarchRecords[iChannel]->fTick, iChannel);

            //eventData->SetRecord(new vector< DataType >(monarchRecord->fDataPtr, monarchRecord->fDataPtr+header->GetRecordSize()), iChannel);
            KTTimeSeries* newRecord;
            if (fTimeSeriesType == kRealTimeSeries)
            {
                newRecord = new KTTimeSeriesReal(fTimeSeriesSize, 0., Double_t(fTimeSeriesSize) * eventData->GetBinWidth());
            }
            else
            {
                newRecord = new KTTimeSeriesFFTW(fTimeSeriesSize, 0., Double_t(fTimeSeriesSize) * eventData->GetBinWidth());
            }
            newRecords[iChannel] = newRecord;
        }

        // Loop over bins
        for (UInt_t iBin = 0; iBin < fHeader.GetRecordSize(); iBin++)
        {
            if (fReadState.fStatus == MonarchReadState::kAtEndOfFile)
            {
                KTINFO(eggreadlog, "End of file reached.\n"
                        << "\tNumber of unused bins: " << iBin - 1);
                delete eventData;
                for (UInt_t iChannel = 0; iChannel < fHeader.GetNChannels(); iChannel++)
                {
                    delete newRecords[iChannel];
                }
                return NULL;
            }
            else if (fReadState.fStatus == MonarchReadState::kAcquisitionIDHasChanged)
            {
                // this means that a new monarch record has come up, and it has a different acquisition id.
                // in this situation we need to start the time series over with the new monarch record
                KTDEBUG(eggreadlog, "Acquisition ID change; resetting event to start with this monarch record.\n"
                        << "\tNumber of unused bins: " << iBin - 1);
                // reset event data
                for (UInt_t iChannel = 0; iChannel < fHeader.GetNChannels(); iChannel++)
                {
                    eventData->SetChannelID(monarchRecords[iChannel]->fCId, iChannel);
                    eventData->SetAcquisitionID(monarchRecords[iChannel]->fAId, iChannel);
                    eventData->SetRecordID(monarchRecords[iChannel]->fRId, iChannel);
                    eventData->SetTimeStamp(monarchRecords[iChannel]->fTick, iChannel);
                }
                // reset bin count to 0
                iBin = 0;
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
            if (fReadState.fDataPtrOffset > fHeader.GetMonarchRecordSize())
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
                }
                fReadState.fDataPtrOffset = 0;
            }
        } // end loop over bins

        // finally, set the records in the new data object
        for (UInt_t iChannel = 0; iChannel < fHeader.GetNChannels(); iChannel++)
        {
            eventData->SetRecord(newRecords[iChannel], iChannel);
        }

        return eventData;
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
        fHeader.SetMonarchRecordSize(monarchHeader->GetRecordSize());
        fHeader.SetAcquisitionTime(monarchHeader->GetAcqTime());
        fHeader.SetAcquisitionRate(monarchHeader->GetAcqRate() * fSampleRateUnitsInHz);
        return;
    }

} /* namespace Katydid */


