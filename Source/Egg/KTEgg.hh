/**
 @file KTEgg.hh
 @brief Contains KTEgg
 @details Reads Egg data files: parses the header and produces events.
 @author: N. S. Oblath
 @date: Sep 9, 2011
 */

#ifndef KTEGG_HH_
#define KTEGG_HH_

#include "MonarchTypes.hpp"

#include <map>
#include <string>

class Monarch;

namespace Katydid
{
    class KTEvent;
    class KTEggHeader;

    class KTEgg
    {
        protected:
            typedef std::map< AcquisitionMode, int > AcquisitionModeMap;
            typedef AcquisitionModeMap::value_type AcqModeMapValue;

        public:
            KTEgg();
            virtual ~KTEgg();

            bool BreakEgg(const std::string& filename);
            KTEvent* HatchNextEvent();

            void SetHeader(KTEggHeader* header);
            KTEggHeader* GetHeader() const;

        private:
            Monarch* fMonarch;

            KTEggHeader* fHeader;

            AcquisitionModeMap fNumberOfRecords;

    };

    inline void KTEgg::SetHeader(KTEggHeader* header)
    {
        fHeader = header;
        return;
    }

    inline KTEggHeader* KTEgg::GetHeader() const
    {
        return fHeader;
    }

} /* namespace Katydid */

#endif /* KTEGG_HH_ */
