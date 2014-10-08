/*
 * KTFilenameParsers.hh
 *
 *  Created on: May 31, 2013
 *      Author: nsoblath
 */

#ifndef KTFILENAMEPARSERS_HH_
#define KTFILENAMEPARSERS_HH_

#include <string>

namespace Katydid
{
    struct KTLocustMCFilename
    {
            unsigned fNEvents;
            double fEventLength; // sec
            double fdfdt; // Hz/sec ?
            double fSignalPower; // Watts
            KTLocustMCFilename(const std::string& filename);
    };


} /* namespace Katydid */

#endif /* KTFILENAMEPARSERS_HH_*/
