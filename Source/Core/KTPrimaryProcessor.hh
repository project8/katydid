/*
 * KTPrimaryProcessor.hh
 *
 *  Created on: Oct 10, 2012
 *      Author: nsoblath
 */

#ifndef KTPRIMARYPROCESSOR_HH_
#define KTPRIMARYPROCESSOR_HH_

#include "KTProcessor.hh"

namespace Katydid
{

    class KTPrimaryProcessor : public KTProcessor
    {
        public:
            KTPrimaryProcessor();
            virtual ~KTPrimaryProcessor();

        public:
            virtual Bool_t Run() = 0;

    };

} /* namespace Katydid */
#endif /* KTPRIMARYPROCESSOR_HH_ */
