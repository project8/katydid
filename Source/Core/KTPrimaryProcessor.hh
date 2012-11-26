/*
 * KTPrimaryProcessor.hh
 *
 *  Created on: Oct 10, 2012
 *      Author: nsoblath
 */

#ifndef KTPRIMARYPROCESSOR_HH_
#define KTPRIMARYPROCESSOR_HH_

#include "KTProcessor.hh"

#include "KTLogger.hh"

namespace Katydid
{

    class KTPrimaryProcessor : public KTProcessor
    {
        public:
            KTPrimaryProcessor();
            virtual ~KTPrimaryProcessor();

        public:
            /// Callable function used by boost::thread
            virtual void operator()();

        public:
            /// Starts the  main action of the processor
            virtual Bool_t Run() = 0;

    };

} /* namespace Katydid */
#endif /* KTPRIMARYPROCESSOR_HH_ */
