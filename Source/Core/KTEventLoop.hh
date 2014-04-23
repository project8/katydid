/*
 * KTEventLoop.hh
 *
 *  Created on: Apr 22, 2014
 *      Author: nsoblath
 */

#ifndef KTEVENTLOOP_HH_
#define KTEVENTLOOP_HH_

namespace Katydid
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

} /* namespace Katydid */

#endif /* KTEVENTLOOP_HH_ */
