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

using std::string;

namespace Katydid
{
    KTLOGGER(egglog, "katydid.egg");

    KTEgg::KTEgg() :
            fReader(NULL),
            fHeader(NULL)
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
        return true;
    }

    KTEvent* KTEgg::HatchNextEvent()
    {
        if (fReader == NULL || fHeader == NULL)
        {
            KTWARN(egglog, "Not prepared to hatch an event");
            return NULL;
        }
        return fReader->HatchNextEvent(fHeader);
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


