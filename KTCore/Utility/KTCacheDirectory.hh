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
    class KTParamNode;

    class KTCacheDirectory : public KTDirectory, public KTSingleton< KTCacheDirectory >, public KTSelfConfigurable
    {
        protected:
            friend class KTSingleton< KTCacheDirectory >;
            friend class KTDestroyer< KTCacheDirectory >;
            KTCacheDirectory(const std::string& name = "cache-directory");
            virtual ~KTCacheDirectory();

        public:
            using KTSelfConfigurable::Configure;

            bool Configure(const KTParamNode* node);

            bool IsReady() const;

    };

    inline bool KTCacheDirectory::IsReady() const
    {
        return fPathExists && fAccess == eReadWrite;
    }

} /* namespace Katydid */
#endif /* KTCACHEDIRECTORY_HH_ */
