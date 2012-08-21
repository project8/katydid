/**
 @file KTEgg.hh
 @brief Contains KTEgg
 @details Reads Egg data files: parses the header and produces events.
 @author: N. S. Oblath
 @date: Sep 9, 2011
 */

#ifndef KTEGGREADERMONARCH_HH_
#define KTEGGREADERMONARCH_HH_

#include "KTEggReader.hh"

#include "MonarchTypes.hpp"

#include <map>
#include <string>

class Monarch;
class MonarchHeader;

namespace Katydid
{
    class KTEvent;
    class KTEggHeader;

    class KTEggReaderMonarch : public KTEggReader
    {
        protected:
            typedef std::map< AcquisitionMode, int > AcquisitionModeMap;
            typedef AcquisitionModeMap::value_type AcqModeMapValue;

        public:
            KTEggReaderMonarch();
            virtual ~KTEggReaderMonarch();

            KTEggHeader* BreakEgg(const std::string& filename);
            KTEvent* HatchNextEvent(KTEggHeader* header);
            bool CloseEgg();

        private:
            /// Copy header information from the MonarchHeader object
            void CopyHeaderInformation(MonarchHeader* monarchHeader, KTEggHeader* eggHeader);

            Monarch* fMonarch;

            AcquisitionModeMap fNumberOfRecords;

    };

} /* namespace Katydid */

#endif /* KTEGGREADERMONARCH_HH_ */
