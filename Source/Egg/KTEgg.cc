/*
 * KTEgg.cc
 *
 *  Created on: Sep 9, 2011
 *      Author: nsoblath
 */

#include "KTEgg.hh"

#include "KTEggHeader.hh"
#include "KTEvent.hh"

#include "Monarch.hpp"
#include "MonarchIO.hpp"

#include <string>

using std::string;

namespace Katydid
{

    KTEgg::KTEgg() :
            fMonarch(NULL),
            fHeader(new KTEggHeader())
    {
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
        fMonarch = Monarch::Open(filename, MonarchIO::ReadMode);

        fHeader->TakeInformation(fMonarch->GetHeader());

        return true;
    }

    KTEvent* KTEgg::HatchNextEvent()
    {
        if (fMonarch == NULL) return NULL;

        KTEvent* event = new KTEvent();

        // TODO: Figure out how to fill all of these things out

        event->SetTimeStamp(newTimeStamp);
        event->SetFrameID(newFrameID);
        event->SetRecord(newRecord);


        //
        event->SetSampleRate(this->GetSampleRate());
        event->SetBinWidth(1. / this->GetSampleRate());
        event->SetRecordLength((double)(this->GetRecordSize()) * event->GetBinWidth());

        return event;
    }

} /* namespace Katydid */


