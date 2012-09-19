/*
 * KTPublishingProcessor.hh
 *
 *  Created on: Sep 19, 2012
 *      Author: nsoblath
 */

#ifndef KTPUBLISHINGPROCESSOR_HH_
#define KTPUBLISHINGPROCESSOR_HH_

#include "KTConfigurable.hh"
#include "KTFactory.hh"
#include "KTProcessor.hh"

#include "KTWriter.hh"

namespace Katydid
{

    class KTPublishingProcessor : public KTConfigurable, public KTProcessor, public KTFactory< KTWriter >
    {
        public:
            KTPublishingProcessor();
            virtual ~KTPublishingProcessor();

        public:
            Bool_t Configure(const KTPStoreNode* node);

    };

} /* namespace Katydid */
#endif /* KTPUBLISHINGPROCESSOR_HH_ */
