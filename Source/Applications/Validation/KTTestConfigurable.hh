/*
 * KTTestConfigurable.hh
 *
 *  Created on: Jul 25, 2012
 *      Author: nsoblath
 */

#ifndef KTTESTCONFIGURABLE_HH_
#define KTTESTCONFIGURABLE_HH_

#include "KTConfigurable.hh"

#include <string>

namespace Katydid
{

    class KTTestConfigurable : public KTConfigurable
    {
        public:
            KTTestConfigurable();
            virtual ~KTTestConfigurable();

            virtual Bool_t Configure(const KTPStoreNode* node);

        private:
            Int_t fIntData;
            Double_t fDoubleData;
            std::string fStringData;
    };

} /* namespace Katydid */
#endif /* KTTESTCONFIGURABLE_HH_ */
