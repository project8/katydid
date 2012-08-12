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

            KTEggHeader* GetHeader();

            double GetHertzPerSampleRateUnit() const;
            double GetSecondsPerRunLengthUnit() const;

            void SetHeader(KTEggHeader* header);

            void SetHertzPerSampleRateUnit(double hpsru);
            void SetSecondsPerRunLengthUnit(double sprlu);

        private:
            Monarch* fMonarch;

            KTEggHeader* fHeader;

            AcquisitionModeMap fNumberOfRecords;

    };

} /* namespace Katydid */

#endif /* KTEGG_HH_ */
