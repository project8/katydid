/*
 * KTCCResults.cc
 *
 *  Created on: May 30, 2013
 *      Author: nsoblath
 */

#include "KTCCResults.hh"

namespace Katydid
{

    KTCCResults::KTCCResults() :
            KTExtensibleStruct< KTCCResults >(),
            fNEvents(0),
            fNEventsWithXCandidateMatches(),
            fNCandidates(0),
            fNCandidatesWithXEventMatches(),
            fEfficiency(0.),
            fFalseRate(0.)
    {
    }

    KTCCResults::~KTCCResults()
    {
    }

} /* namespace Katydid */
