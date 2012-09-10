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

#include "Rtypes.h"

#include <map>
#include <string>

class MonarchPP;
class MonarchHeader;

namespace Katydid
{
    class KTEvent;
    class KTEggHeader;

    class KTEggReaderMonarch : public KTEggReader
    {
        protected:
            typedef std::map< UInt_t, Int_t > AcquisitionModeMap;
            typedef AcquisitionModeMap::value_type AcqModeMapValue;

        public:
            KTEggReaderMonarch();
            virtual ~KTEggReaderMonarch();

            KTEggHeader* BreakEgg(const std::string& filename);
            KTTimeSeriesData* HatchNextEvent(KTEggHeader* header);
            Bool_t CloseEgg();

        private:
            /// Copy header information from the MonarchHeader object
            void CopyHeaderInformation(const MonarchHeader* monarchHeader, KTEggHeader* eggHeader);

            const MonarchPP* fMonarch;

            AcquisitionModeMap fNumberOfRecords;

    };

} /* namespace Katydid */

#endif /* KTEGGREADERMONARCH_HH_ */
