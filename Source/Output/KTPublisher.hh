/*
 * KTPublisher.hh
 *
 *  Created on: Sep 19, 2012
 *      Author: nsoblath
 */

#ifndef KTPUBLISHER_HH_
#define KTPUBLISHER_HH_

#include "KTConfigurable.hh"
#include "KTFactory.hh"
#include "KTProcessor.hh"

#include "KTWriter.hh"

namespace Katydid
{

    class KTPublisher : public KTConfigurable, public KTProcessor, public KTFactory< KTWriter >
    {
        public:
            KTPublisher();
            virtual ~KTPublisher();

        public:
            Bool_t Configure(const KTPStoreNode* node);

    };

} /* namespace Katydid */
#endif /* KTPUBLISHER_HH_ */
