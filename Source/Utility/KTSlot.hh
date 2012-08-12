/*
 * KTSlot.hh
 *
 *  Created on: Feb 10, 2012
 *      Author: nsoblath
 */

#ifndef KTSLOT_HH_
#define KTSLOT_HH_

namespace Katydid
{
    template< typename XSignature >
    class KTSlot
    {
        public:
            KTSlot() : fSlot(NULL)
            {}
            virtual ~KTSlot()
            {
                delete fSlot;
            }

            XSignature GetSlot()
            {
                return fSlot;
            }

        protected:
            XSignature fSlot;

    };

} /* namespace Katydid */
#endif /* KTSLOT_HH_ */
