/**
 @file KTProcessor.hh
 @brief Contains KTProcessor
 @details KTProcessor is the processor base class
 @author: N. S. Oblath
 @date: Jan 5, 2012
 */

#ifndef KTPROCESSOR_HH_
#define KTPROCESSOR_HH_

#include "KTConfigurable.hh"
#include "KTSetting.hh"

#include "Rtypes.h"

namespace Katydid
{

    class KTProcessor
    {
        public:
            KTProcessor();
            virtual ~KTProcessor();

            //virtual void RunProcess() = 0;

            virtual Bool_t ApplySetting(const KTSetting* setting) = 0;
            //virtual Bool_t ApplySetting(const KTSetting* setting) = 0;
    };
    /*
    inline Bool_t KTProcessor::ApplySetting(KTSetting setting)
    {
        return ApplySetting(&setting);
    }
    */
} /* namespace Katydid */
#endif /* KTPROCESSOR_HH_ */
