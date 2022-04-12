/*
 * KTChirpSpaceVarianceData.cc
 *
 *  Created on: Feb 23, 2018
 *      Author: nsoblath
 */

#include "KTChirpSpaceVarianceData.hh"

namespace Katydid
{
    KTChirpSpaceVarianceDataCore::KTChirpSpaceVarianceDataCore() :
            KTChirpDomainArrayData()
    {
    }

    KTChirpSpaceVarianceDataCore::~KTChirpSpaceVarianceDataCore()
    {
        while (! fSpectra.empty())
        {
            delete fSpectra.back();
            fSpectra.pop_back();
        }
    }

} /* namespace Katydid */
