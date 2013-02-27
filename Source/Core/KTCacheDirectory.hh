/*
 * KTCacheDirectory.hh
 *
 *  Created on: Dec 3, 2012
 *      Author: nsoblath
 */

#ifndef KTCACHEDIRECTORY_HH_
#define KTCACHEDIRECTORY_HH_

#include "KTConfigurable.hh"
#include "KTDirectory.hh"
#include "KTSingleton.hh"

namespace Katydid
{
    class KTPStoreNode;

    class KTCacheDirectory : public KTDirectory, public KTSingleton< KTCacheDirectory >, public KTConfigurable
    {
        protected:
            friend class KTSingleton< KTCacheDirectory >;
            friend class KTDestroyer< KTCacheDirectory >;
            KTCacheDirectory(const std::string& name = "cache-directory");
            virtual ~KTCacheDirectory();

        public:
            Bool_t Configure(const KTPStoreNode* node);
            Bool_t PrepareForUse();

            Bool_t IsOkay() const;

            Bool_t GetPreparedForUse() const;

        protected:
            Bool_t fPreparedForUse;


    };

    inline Bool_t KTCacheDirectory::IsOkay() const
    {
        return fPathExists && fAccess == eReadWrite;
    }

    inline Bool_t KTCacheDirectory::GetPreparedForUse() const
    {
        return fPreparedForUse;
    }

} /* namespace Katydid */
#endif /* KTCACHEDIRECTORY_HH_ */
