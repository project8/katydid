/*
 * KTEgg.cc
 *
 *  Created on: Sep 9, 2011
 *      Author: nsoblath
 */

#include "KTEggReaderMonarch.hh"

#include "KTEgg.hh"
#include "KTEggHeader.hh"
#include "KTLogger.hh"
#include "KTTimeSeriesDataFFTW.hh"
#include "KTTimeSeriesDataReal.hh"
#include "KTTimeSeriesFFTW.hh"
#include "KTTimeSeriesReal.hh"

#include "MonarchPP.hh"
#include "MonarchHeader.hpp"
#include "MonarchTypes.hpp"

using std::map;
using std::string;
using std::vector;

namespace Katydid
{
    KTLOGGER(eggreadlog, "katydid.egg");

    KTEggReaderMonarch::KTEggReaderMonarch() :
            KTEggReader(),
            fTimeSeriesType(kRealTimeSeries),
            fMonarch(NULL),
            fNumberOfRecords(),
            fSampleRateUnitsInHz(1.e6),
            fFullVoltageScale(0.5),
            fNADCLevels(256)
    {
        fNumberOfRecords.insert(AcqModeMapValue(sOneChannel, 1));
        fNumberOfRecords.insert(AcqModeMapValue(sTwoChannel, 2));
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
        KTEggHeader* header = new KTEggHeader();
        CopyHeaderInformation(fMonarch->GetHeader(), header);

        return header;
    }

    KTTimeSeriesData* KTEggReaderMonarch::HatchNextEvent(KTEggHeader* header)
    {
        if (fMonarch == NULL)
        {
            KTERROR(eggreadlog, "Monarch file has not been opened");
            return NULL;
        }

        if (! fMonarch->ReadRecord())
        {
            KTINFO(eggreadlog, "End of file reached before complete event was read.");
            return NULL;
        }

        UInt_t numberOfRecords = fNumberOfRecords[header->GetAcquisitionMode()];
        KTTimeSeriesData* eventData;
        if (fTimeSeriesType == kRealTimeSeries)
        {
            eventData = new KTTimeSeriesDataReal(numberOfRecords);
        }
        else
        {
            eventData = new KTTimeSeriesDataFFTW(numberOfRecords);
        }

        // Fill out event information
        eventData->SetSampleRate(header->GetAcquisitionRate());
        eventData->SetRecordSize(header->GetRecordSize());
        eventData->CalculateBinWidthAndRecordLength();
        //eventData->SetBinWidth(1. / eventData->GetSampleRate());
        //eventData->SetRecordLength((double)(eventData->GetRecordSize()) * eventData->GetBinWidth());

        // Normalization of the record values
        Double_t normalization = fFullVoltageScale / (Double_t)fNADCLevels;

        for (UInt_t iRecord=0; iRecord < numberOfRecords; iRecord++)
        {
            const MonarchRecord* monarchRecord = monarchRecord = fMonarch->GetRecord(iRecord);
            if (monarchRecord == NULL)
            {
                KTWARN(eggreadlog, "Record " << iRecord << " is incomplete; event read aborted.");
                delete eventData;
                return NULL;
            }

            eventData->SetChannelID(monarchRecord->fCId, iRecord);
            eventData->SetAcquisitionID(monarchRecord->fAId, iRecord);
            eventData->SetRecordID(monarchRecord->fRId, iRecord);
            eventData->SetTimeStamp(monarchRecord->fTick, iRecord);

            //eventData->SetRecord(new vector< DataType >(monarchRecord->fDataPtr, monarchRecord->fDataPtr+header->GetRecordSize()), iRecord);
            KTTimeSeries* newRecord;
            if (fTimeSeriesType == kRealTimeSeries)
            {
                newRecord = new KTTimeSeriesReal(header->GetRecordSize(), 0., Double_t(header->GetRecordSize()) * eventData->GetBinWidth());
            }
            else
            {
                newRecord = new KTTimeSeriesFFTW(header->GetRecordSize(), 0., Double_t(header->GetRecordSize()) * eventData->GetBinWidth());
            }

            for (UInt_t iBin=0; iBin<header->GetRecordSize(); iBin++)
            {
                //(*newRecord)(iBin) = Double_t(monarchRecord->fDataPtr[iBin]) * normalization;
                newRecord->SetValue(iBin, Double_t(monarchRecord->fDataPtr[iBin]) * normalization);
            }
            eventData->SetRecord(newRecord, iRecord);
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


    void KTEggReaderMonarch::CopyHeaderInformation(const MonarchHeader* monarchHeader, KTEggHeader* eggHeader)
    {
        eggHeader->SetFilename(monarchHeader->GetFilename());
        eggHeader->SetAcquisitionMode(monarchHeader->GetAcqMode());
        eggHeader->SetRecordSize(monarchHeader->GetRecordSize());
        eggHeader->SetAcquisitionTime(monarchHeader->GetAcqTime());
        eggHeader->SetAcquisitionRate(monarchHeader->GetAcqRate() * fSampleRateUnitsInHz);
        return;
    }



} /* namespace Katydid */


