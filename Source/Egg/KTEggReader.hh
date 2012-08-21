/*
 * KTEggReader.hh
 *
 *  Created on: Aug 20, 2012
 *      Author: nsoblath
 */

#ifndef KTEGGREADER_HH_
#define KTEGGREADER_HH_

#include<string>

namespace Katydid
{
    class KTEggHeader;
    class KTEvent;

    class KTEggReader
    {
        public:
            KTEggReader();
            virtual ~KTEggReader();

        public:
            virtual KTEggHeader* BreakEgg(const std::string&) = 0;
            virtual KTEvent* HatchNextEvent(KTEggHeader*) = 0;
            virtual bool CloseEgg() = 0;

    };

} /* namespace Katydid */
#endif /* KTEGGREADER_HH_ */
