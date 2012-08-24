/*
 * KTEgg.cc
 *
 *  Created on: Sep 9, 2011
 *      Author: nsoblath
 */

#include "KTEggReaderMonarch.hh"

#include "KTEgg.hh"
#include "KTEggHeader.hh"
#include "KTTimeSeriesData.hh"
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

    KTTimeSeriesData* KTEggReaderMonarch::HatchNextEvent(KTEggHeader* header)
    {
        if (fMonarch == NULL) return NULL;

        MonarchRecord* monarchRecord = NULL;
        KTTimeSeriesData* eventData = new KTTimeSeriesData(fNumberOfRecords[header->GetAcquisitionMode()]);

        //
        eventData->SetSampleRate(header->GetAcquisitionRate());
        eventData->SetRecordSize(header->GetRecordSize());
        eventData->CalculateBinWidthAndRecordLength();
        //eventData->SetBinWidth(1. / eventData->GetSampleRate());
        //eventData->SetRecordLength((double)(eventData->GetRecordSize()) * eventData->GetBinWidth());

        for (int iRecord=0; iRecord<=fNumberOfRecords[header->GetAcquisitionMode()]; iRecord++)
        {

            try
            {
                monarchRecord = fMonarch->GetNextEvent();
            }
            catch(MonarchExceptions::EndOfFile& e)
            {
                delete eventData;
                KTINFO(eggreadlog, "End of file reached before complete event was read: <" << e.what() << ">");
                return NULL;
            }
            catch(std::exception& e)
            {
                delete eventData;
                KTERROR(eggreadlog, "Something went wrong while reading out an event: " << e.what());
                return NULL;
            }

            eventData->SetChannelID(monarchRecord->fCId, iRecord);
            eventData->SetAcquisitionID(monarchRecord->fAId, iRecord);
            eventData->SetRecordID(monarchRecord->fRId, iRecord);
            eventData->SetTimeStamp(monarchRecord->fTick, iRecord);

            eventData->SetRecord(new vector< DataType >(monarchRecord->fDataPtr, monarchRecord->fDataPtr+header->GetRecordSize()), iRecord);
        }

        return eventData;
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


