/*
 * KTAnalysisCandidates.cc
 *
 *  Created on: Apr 11, 2013
 *      Author: nsoblath
 */

#include "KTAnalysisCandidates.hh"

namespace Katydid
{

    KTAnalysisCandidates::KTAnalysisCandidates() :
                KTExtensibleData< KTAnalysisCandidates >(),
                fCandidates(),
                fRecordSize(1)
    {
    }

    KTAnalysisCandidates::~KTAnalysisCandidates()
    {
    }

} /* namespace Katydid */
