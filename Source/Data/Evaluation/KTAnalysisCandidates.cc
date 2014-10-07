/*
 * KTAnalysisCandidates.cc
 *
 *  Created on: Apr 11, 2013
 *      Author: nsoblath
 */

#include "KTAnalysisCandidates.hh"

namespace Katydid
{
    const std::string KTAnalysisCandidates::sName("analysis-candidates");

    KTAnalysisCandidates::KTAnalysisCandidates() :
                KTExtensibleData< KTAnalysisCandidates >(),
                fCandidates(),
                fRecordSize(1),
                fNRecords(0)
    {
    }

    KTAnalysisCandidates::~KTAnalysisCandidates()
    {
    }

} /* namespace Katydid */
