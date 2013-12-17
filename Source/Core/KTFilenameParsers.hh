/*
 * KTFilenameParsers.hh
 *
 *  Created on: May 31, 2013
 *      Author: nsoblath
 */

#ifndef KTFILENAMEPARSERS_HH_
#define KTFILENAMEPARSES_HH_

#include "Rtypes.h"

#include <string>

namespace Katydid
{
    struct KTLocustMCFilename
    {
            UInt_t fNEvents;
            double fEventLength; // sec
            double fdfdt; // Hz/sec ?
            double fSignalPower; // Watts
            KTLocustMCFilename(const std::string& filename);
    };


} /* namespace Katydid */

#endif /* KTFILENAMEPARSERS_HH_*/
