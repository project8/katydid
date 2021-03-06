/*
 * KTEgg.cc
 *
 *  Created on: Sep 9, 2011
 *      Author: nsoblath
 */

#include "KTEgg.hh"

#include "KTData.hh"
#include "KTEggHeader.hh"
#include "KTEggReader.hh"
#include "KTLogger.hh"

using std::string;
using boost::shared_ptr;

namespace Katydid
{
    KTLOGGER(egglog, "katydid.egg");

    KTEgg::KTEgg() :
            fReader(NULL),
            fHeader(NULL),
            fSliceCounter(-1)
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
        fSliceCounter = -1;
        return true;
    }

    shared_ptr<KTData> KTEgg::HatchNextSlice()
    {
        if (fReader == NULL || fHeader == NULL)
        {
            KTWARN(egglog, "Not prepared to hatch an slice");
            return shared_ptr<KTData>();
        }

        shared_ptr<KTData> data = fReader->HatchNextSlice();
        fSliceCounter++;

        if (data)
        {
            data->fCounter = (unsigned)fSliceCounter;
        }

        return data;
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


