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

        protected:
            /// Copy header information from the MonarchHeader object
            void CopyHeaderInformation(const MonarchHeader* monarchHeader, KTEggHeader* eggHeader);

            const MonarchPP* fMonarch;

            AcquisitionModeMap fNumberOfRecords;

        public:
            Double_t GetSampleRateUnitsInHz();

            Double_t GetFullVoltageScale();
            UInt_t GetNADCLevels();

        protected:
            Double_t fSampleRateUnitsInHz;

            Double_t fFullVoltageScale;
            UInt_t fNADCLevels;

    };

} /* namespace Katydid */

#endif /* KTEGGREADERMONARCH_HH_ */
