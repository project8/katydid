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
            Double_t fEventLength; // sec
            Double_t fdfdt; // Hz/sec ?
            Double_t fSignalPower; // Watts
            KTLocustMCFilename(const std::string& filename);
    };


} /* namespace Katydid */

#endif /* KTFILENAMEPARSERS_HH_*/
