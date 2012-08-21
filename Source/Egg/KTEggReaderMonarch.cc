/*
 * KTEgg.cc
 *
 *  Created on: Sep 9, 2011
 *      Author: nsoblath
 */

#include "KTEggReaderMonarch.hh"

#include "KTEgg.hh"
#include "KTEggHeader.hh"
#include "KTEvent.hh"
#include "KTLogger.hh"

#include "Monarch.hpp"
#include "MonarchHeader.hpp"

using std::map;
using std::string;
using std::vector;

namespace Katydid
{
    KTLOGGER(eggreadlog, "katydid.egg");

    KTEggReaderMonarch::KTEggReaderMonarch() :
            KTEggReader(),
            fMonarch(NULL),
            fNumberOfRecords()
    {
        fNumberOfRecords.insert(AcqModeMapValue(OneChannel, 1));
        fNumberOfRecords.insert(AcqModeMapValue(TwoChannel, 2));
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
        fMonarch = Monarch::Open(filename, ReadMode);
        if (fMonarch == NULL)
        {
            KTERROR(eggreadlog, "Unable to break egg (no cake for you!)");
            return NULL;
        }

        KTDEBUG(eggreadlog, "File open; reading header");
        KTEggHeader* header = new KTEggHeader();
        CopyHeaderInformation(fMonarch->GetHeader(), header);

        return header;
    }

    KTEvent* KTEggReaderMonarch::HatchNextEvent(KTEggHeader* header)
    {
        if (fMonarch == NULL) return NULL;

        MonarchRecord* eventRecord = NULL;
        KTEvent* event = new KTEvent(fNumberOfRecords[header->GetAcquisitionMode()]);

        //
        event->SetSampleRate(header->GetAcquisitionRate());
        event->SetRecordSize(header->GetRecordSize());
        event->CalculateBinWidthAndRecordLength();
        //event->SetBinWidth(1. / event->GetSampleRate());
        //event->SetRecordLength((double)(event->GetRecordSize()) * event->GetBinWidth());

        for (int iRecord=0; iRecord<=fNumberOfRecords[header->GetAcquisitionMode()]; iRecord++)
        {

            try
            {
                eventRecord = fMonarch->GetNextEvent();
            }
            catch(MonarchExceptions::EndOfFile& e)
            {
                delete event;
                KTINFO(eggreadlog, "End of file reached before complete event was read: <" << e.what() << ">");
                return NULL;
            }

            event->SetChannelID(eventRecord->fCId, iRecord);
            event->SetAcquisitionID(eventRecord->fAId, iRecord);
            event->SetRecordID(eventRecord->fRId, iRecord);
            event->SetTimeStamp(eventRecord->fTick, iRecord);

            event->SetRecord(new vector< DataType >(eventRecord->fDataPtr, eventRecord->fDataPtr+header->GetRecordSize()), iRecord);
        }

        return event;
    }

    bool KTEggReaderMonarch::CloseEgg()
    {
        bool fileExistedAndWasClosed = fMonarch->Close();
        delete fMonarch;
        fMonarch = NULL;
        return fileExistedAndWasClosed;
    }


    void KTEggReaderMonarch::CopyHeaderInformation(MonarchHeader* monarchHeader, KTEggHeader* eggHeader)
    {
        eggHeader->SetFilename(monarchHeader->GetFilename());
        eggHeader->SetAcquisitionMode(monarchHeader->GetAcqMode());
        eggHeader->SetRecordSize(monarchHeader->GetRecordSize());
        eggHeader->SetAcquisitionTime(monarchHeader->GetAcqTime());
        eggHeader->SetAcquisitionRate(monarchHeader->GetAcqRate());
        return;
    }



} /* namespace Katydid */


