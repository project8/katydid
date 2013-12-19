/*
 * KTDigitizerTestData.hh
 *
 *  Created on: Dec 18, 2013
 *      Author: nsoblath
 */

#ifndef KTDIGITIZERTESTDATA_HH_
#define KTDIGITIZERTESTDATA_HH_

#include "KTData.hh"

namespace Katydid
{

    class KTDigitizerTestData : public KTExtensibleData< KTDigitizerTestData >
    {
        public:
            KTDigitizerTestData();
            virtual ~KTDigitizerTestData();

            unsigned GetNComponents() const;

            KTDigitizerTestData& SetNComponents(unsigned num);

        //private:

    };

} /* namespace Katydid */
#endif /* KTDIGITIZERTESTDATA_HH_ */
