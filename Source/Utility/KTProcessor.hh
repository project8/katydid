/**
 @file KTProcessor.hh
 @brief Contains KTProcessor
 @details KTProcessor is the processor base class
 @author: N. S. Oblath
 @date: Jan 5, 2012
 */

#ifndef KTPROCESSOR_HH_
#define KTPROCESSOR_HH_

#include "KTSetting.hh"

#include "Rtypes.h"

namespace Katydid
{

    class KTProcessor
    {
        public:
            KTProcessor();
            virtual ~KTProcessor();

    };

} /* namespace Katydid */
#endif /* KTPROCESSOR_HH_ */
