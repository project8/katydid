/*
 * KTConfigurable.hh
 *
 *  Created on: Jul 24, 2012
 *      Author: nsoblath
 */

#ifndef KTCONFIGURABLE_HH_
#define KTCONFIGURABLE_HH_

namespace Katydid
{
    class KTParameterStore;
    class KTPStoreNode;

    class KTConfigurable
    {
        public:
            KTConfigurable();
            virtual ~KTConfigurable();

            virtual void Configure(KTPStoreNode* node) = 0;

        protected:
            KTParameterStore* fParameterStore;

    };

} /* namespace Katydid */
#endif /* KTCONFIGURABLE_HH_ */
