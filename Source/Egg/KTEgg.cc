/*
 * KTEgg.cc
 *
 *  Created on: Sep 9, 2011
 *      Author: nsoblath
 */

#include "KTEgg.hh"

#include "KTEggHeader.hh"
#include "KTEvent.hh"
#include "KTLogger.hh"

#include "Monarch.hpp"

using std::map;
using std::string;

namespace Katydid
{
    KTLOGGER(egglog, "katydid.egg");

    KTEgg::KTEgg() :
            fMonarch(NULL),
            fHeader(new KTEggHeader()),
            fNumberOfRecords()
    {
        fNumberOfRecords.insert(AcqModeMapValue(OneChannel, 1));
        fNumberOfRecords.insert(AcqModeMapValue(TwoChannel, 2));
    }

    KTEgg::~KTEgg()
    {
        if (fMonarch != NULL)
        {
            fMonarch->Close();
            delete fMonarch;
        }
        delete fHeader;
    }

    bool KTEgg::BreakEgg(const string& filename)
    {
        if (fMonarch != NULL)
        {
            fMonarch->Close();
            delete fMonarch;
        }

        // open the file
        KTDEBUG(egglog, "Attempting to open file <" << filename << ">");
        fMonarch = Monarch::Open(filename, ReadMode);
        if (fMonarch == NULL)
        {
            KTERROR(egglog, "Unable to break egg (no cake for you!)");
            return false;
        }

        KTDEBUG(egglog, "File open; reading header");
        fHeader->TakeInformation(fMonarch->GetHeader());

        return true;
    }

    KTEvent* KTEgg::HatchNextEvent()
    {
        if (fMonarch == NULL) return NULL;

        MonarchRecord* eventRecord = NULL;
        KTEvent* event = new KTEvent();

        //
        event->SetSampleRate(fHeader->GetAcquisitionRate());
        event->SetRecordSize(fHeader->GetRecordSize());
        event->CalculateBinWidthAndRecordLength();
        //event->SetBinWidth(1. / event->GetSampleRate());
        //event->SetRecordLength((double)(event->GetRecordSize()) * event->GetBinWidth());

        for (int iRecord=0; iRecord<=fNumberOfRecords[fHeader->GetAcquisitionMode()]; iRecord++)
        {

            try
            {
                eventRecord = fMonarch->GetNextEvent();
            }
            catch(MonarchExceptions::EndOfFile& e)
            {
                delete event;
                KTINFO(egglog, "End of file reached before complete event was read: <" << e.what() << ">");
                return NULL;
            }

            event->SetChannelID(eventRecord->fCId, iRecord);
            event->SetAcquisitionID(eventRecord->fAId, iRecord);
            event->SetRecordID(eventRecord->fRId, iRecord);
            event->SetTimeStamp(eventRecord->fTick, iRecord);

            event->SetRecord(std::vector< DataType >(eventRecord->fDataPtr, eventRecord->fDataPtr+fHeader->GetRecordSize()), iRecord);

        }

        return event;
    }

    bool KTEgg::CloseEgg()
    {
        bool fileExistedAndWasClosed = fMonarch->Close();
        delete fMonarch;
        fMonarch = NULL;
        return fileExistedAndWasClosed;
    }

} /* namespace Katydid */


