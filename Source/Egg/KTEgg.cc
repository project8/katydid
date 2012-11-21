/*
 * KTEgg.cc
 *
 *  Created on: Sep 9, 2011
 *      Author: nsoblath
 */

#include "KTEgg.hh"

#include "KTEggHeader.hh"
#include "KTEggReader.hh"
#include "KTEvent.hh"
#include "KTLogger.hh"
#include "KTTimeSeriesData.hh"

using std::string;
using boost::shared_ptr;

namespace Katydid
{
    KTLOGGER(egglog, "katydid.egg");

    KTEgg::KTEgg() :
            fReader(NULL),
            fHeader(NULL),
            fEventCounter(-1)
    {
    }

    KTEgg::~KTEgg()
    {
        delete fReader;
        delete fHeader;
    }

    bool KTEgg::BreakEgg(const string& filename)
    {
        if (fReader == NULL)
        {
            KTWARN(egglog, "Egg reader has not been set");
            return false;
        }
        fHeader = fReader->BreakEgg(filename);
        if (fHeader == NULL)
        {
            KTWARN(egglog, "No header was received");
            return false;
        }
        fEventCounter = -1;
        return true;
    }

    shared_ptr<KTEvent> KTEgg::HatchNextEvent()
    {
        if (fReader == NULL || fHeader == NULL)
        {
            KTWARN(egglog, "Not prepared to hatch an event");
            return shared_ptr<KTEvent>();
        }

        KTTimeSeriesData* data = fReader->HatchNextEvent(fHeader);
        if (data == NULL)
        {
            return shared_ptr<KTEvent>();
        }
        fEventCounter++;

        shared_ptr<KTEvent> newEvent(new KTEvent());
        newEvent->SetEventNumber(unsigned(fEventCounter));
        newEvent->AddData(data);

        return newEvent;
    }

    bool KTEgg::CloseEgg()
    {
        if (fReader == NULL)
        {
            KTWARN(egglog, "Egg reader has not been set");
            return false;
        }
        return fReader->CloseEgg();
    }

    void KTEgg::SetReader(KTEggReader* reader)
    {
        delete fReader;
        fReader = reader;
        return;
    }

    void KTEgg::SetHeader(KTEggHeader* header)
    {
        delete fHeader;
        fHeader = header;
        return;
    }



} /* namespace Katydid */


