/*
 * KTSignal.cc
 *
 *  Created on: Feb 25, 2013
 *      Author: nsoblath
 */

#include "KTSignal.hh"

namespace Nymph
{

    KTSignalOneArg< void >::KTSignalOneArg(const std::string& name, KTProcessor* proc) :
            fSignal()
    {
        proc->RegisterSignal(name, &fSignal);
    }

    KTSignalOneArg< void >::KTSignalOneArg() :
            fSignal()
    {}

    KTSignalOneArg< void >::KTSignalOneArg(const KTSignalOneArg&) :
            fSignal()
    {}

    KTSignalOneArg< void >::~KTSignalOneArg()
    {
    }



    KTSignalData::KTSignalData(const std::string& name, KTProcessor* proc) :
            KTSignalOneArg(name, proc),
            fRefSignal()
    {
        proc->RegisterSignal("ref-"+name, &fRefSignal);
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
