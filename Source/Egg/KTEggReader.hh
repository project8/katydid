/*
 * KTEggReader.hh
 *
 *  Created on: Aug 20, 2012
 *      Author: nsoblath
 */

#ifndef KTEGGREADER_HH_
#define KTEGGREADER_HH_

#include "KTData.hh"

#include <string>

namespace Katydid
{
    class KTEggReader
    {
        public:
            KTEggReader();
            virtual ~KTEggReader();

        public:
            virtual KTDataPtr BreakEgg(const std::string&) = 0;
            virtual KTDataPtr HatchNextSlice() = 0;
            virtual bool CloseEgg() = 0;

            virtual unsigned GetNSlicesProcessed() const = 0;
            virtual unsigned GetNRecordsProcessed() const = 0;
            virtual double GetIntegratedTime() const = 0;

    };

} /* namespace Katydid */
#endif /* KTEGGREADER_HH_ */
