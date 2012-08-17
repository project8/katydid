/*
 * KTTestConfigurable.hh
 *
 *  Created on: Jul 25, 2012
 *      Author: nsoblath
 */

#ifndef KTTESTCONFIGURABLE_HH_
#define KTTESTCONFIGURABLE_HH_

#include "Rtypes.h"

#include <string>

namespace Katydid
{
    class KTPStoreNode;

    class KTTestConfigurable
    {
        public:
            KTTestConfigurable();
            virtual ~KTTestConfigurable();

            Bool_t Configure(const KTPStoreNode* node);

        private:
            Int_t fIntData;
            Double_t fDoubleData;
            std::string fStringData;
    };

} /* namespace Katydid */
#endif /* KTTESTCONFIGURABLE_HH_ */
