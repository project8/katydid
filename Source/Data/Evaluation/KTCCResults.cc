/*
 * KTCCResults.cc
 *
 *  Created on: May 30, 2013
 *      Author: nsoblath
 */

#include "KTCCResults.hh"

namespace Katydid
{
    const std::string KTCCResults::sName("cc-results");

    KTCCResults::KTCCResults() :
            KTExtensibleData< KTCCResults >(),
            fEventLength(0.),
            fdfdt(0.),
            fSignalPower(0.),
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
