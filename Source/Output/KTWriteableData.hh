/*
 * KTWriteableData.hh
 *
 *  Created on: Aug 25, 2012
 *      Author: nsoblath
 */

#ifndef KTWRITEABLEDATA_HH_
#define KTWRITEABLEDATA_HH_

#include "KTData.hh"

#include "Rtypes.h"

#include <iostream>

namespace Katydid
{
    class KTWriter;

    class KTWriteableData : public KTData
    {
        public:
            KTWriteableData();
            virtual ~KTWriteableData();

            virtual void Accept(KTWriter* writer) const = 0;
    };

} /* namespace Katydid */
#endif /* KTWRITEABLEDATA_HH_ */
