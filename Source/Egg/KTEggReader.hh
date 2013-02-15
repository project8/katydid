/*
 * KTEggReader.hh
 *
 *  Created on: Aug 20, 2012
 *      Author: nsoblath
 */

#ifndef KTEGGREADER_HH_
#define KTEGGREADER_HH_

#include "Rtypes.h"

#include <boost/shared_ptr.hpp>

#include <string>

namespace Katydid
{
    class KTEggHeader;
    class KTData;

    class KTEggReader
    {
        public:
            KTEggReader();
            virtual ~KTEggReader();

        public:
            virtual KTEggHeader* BreakEgg(const std::string&) = 0;
            virtual boost::shared_ptr< KTData > HatchNextSlice() = 0;
            virtual Bool_t CloseEgg() = 0;

    };

} /* namespace Katydid */
#endif /* KTEGGREADER_HH_ */
