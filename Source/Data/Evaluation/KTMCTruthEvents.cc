/*
 * KTMCTruthEvents.cc
 *
 *  Created on: Apr 11, 2013
 *      Author: nsoblath
 */

#include "KTMCTruthEvents.hh"

namespace Katydid
{

    KTMCTruthEvents::KTMCTruthEvents() :
        KTExtensibleData< KTMCTruthEvents >(),
        fEventLength(0.),
        fdfdt(0.),
        fSignalPower(0.),
        fEvents(),
        fRecordSize(1),
        fNRecords(0)
    {
    }

    KTMCTruthEvents::~KTMCTruthEvents()
    {
    }

} /* namespace Katydid */
