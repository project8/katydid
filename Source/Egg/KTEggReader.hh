/*
 * KTEggReader.hh
 *
 *  Created on: Aug 20, 2012
 *      Author: nsoblath
 */

#ifndef KTEGGREADER_HH_
#define KTEGGREADER_HH_

#include "KTConfigurable.hh"

#include <string>

namespace Katydid
{
    class KTEggHeader;
    class KTTimeSeriesData;

    class KTEggReader
    {
        public:
            KTEggReader();
            virtual ~KTEggReader();

        public:
            Bool_t Configure(const KTPStoreNode* node);

        public:
            virtual KTEggHeader* BreakEgg(const std::string&) = 0;
            virtual KTTimeSeriesData* HatchNextEvent(KTEggHeader*) = 0;
            virtual Bool_t CloseEgg() = 0;

    };

} /* namespace Katydid */
#endif /* KTEGGREADER_HH_ */
