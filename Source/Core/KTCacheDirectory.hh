/*
 * KTCacheDirectory.hh
 *
 *  Created on: Dec 3, 2012
 *      Author: nsoblath
 */

#ifndef KTCACHEDIRECTORY_HH_
#define KTCACHEDIRECTORY_HH_

#include "KTDirectory.hh"
#include "KTSingleton.hh"

#include "Rtypes.h"

namespace Katydid
{

    class KTCacheDirectory : public KTDirectory, public KTSingleton< KTCacheDirectory >
    {

        protected:
            friend class KTSingleton< KTCacheDirectory >;
            friend class KTDestroyer< KTCacheDirectory >;
            KTCacheDirectory();
            virtual ~KTCacheDirectory();

        public:
            Bool_t IsOkay() const;


    };

    Bool_t KTCacheDirectory::IsOkay() const
    {
        return /*Exists() && */fAccess == eReadWrite;
    }

} /* namespace Katydid */
#endif /* KTCACHEDIRECTORY_HH_ */
