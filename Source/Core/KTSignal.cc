/*
 * KTSignal.cc
 *
 *  Created on: Feb 25, 2013
 *      Author: nsoblath
 */

#include "KTSignal.hh"

namespace Katydid
{

    KTSignalOneArg< void >::KTSignalOneArg(const std::string& name, KTProcessor* proc) :
            fSignal()
    {
        proc->RegisterSignal(name, &fSignal, "");
    }

    KTSignalOneArg< void >::KTSignalOneArg() :
            fSignal()
    {}

    KTSignalOneArg< void >::KTSignalOneArg(const KTSignalOneArg& rhs) :
            fSignal()
    {}

    KTSignalOneArg< void >::~KTSignalOneArg()
    {
    }



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
