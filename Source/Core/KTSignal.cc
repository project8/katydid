/*
 * KTSignal.cc
 *
 *  Created on: Feb 25, 2013
 *      Author: nsoblath
 */

#include "KTSignal.hh"

namespace Katydid
{

    KTSignalData::KTSignalData(const std::string& name, KTProcessor* proc) :
            KTSignalOneArg(name, proc)
    {
    }

    KTSignalData::~KTSignalData()
    {
    }

    KTSignalData::KTSignalData()
    {
    }

    KTSignalData::KTSignalData(const KTSignalData&)
    {
    }

}
