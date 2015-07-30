/*
 * KTEventLoop.hh
 *
 *  Created on: Apr 22, 2014
 *      Author: nsoblath
 */

#ifndef KTEVENTLOOP_HH_
#define KTEVENTLOOP_HH_

namespace Nymph
{

    /*!
     @class KTEventLoop
     @author N. S. Oblath

     @brief Abstract base class for event loops

     @details
     */
    class KTEventLoop
    {
        public:
            KTEventLoop();
            virtual ~KTEventLoop();

            virtual void Go() = 0;

            virtual void Stop() = 0;

    };

} /* namespace Nymph */

#endif /* KTEVENTLOOP_HH_ */
