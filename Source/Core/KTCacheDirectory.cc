/*
 * KTCacheDirectory.cc
 *
 *  Created on: Dec 3, 2012
 *      Author: nsoblath
 */

#include "KTCacheDirectory.hh"

#include "KTLogger.hh"
#include "KTParameterStore.hh"
#include "KTPStoreNode.hh"

using std::string;

namespace Katydid
{
    KTLOGGER(dirlog, "KTCacheDirectory");

    KTCacheDirectory::KTCacheDirectory(const std::string& name) :
            KTDirectory(),
            KTSelfConfigurable(name)
    {
    }

    KTCacheDirectory::~KTCacheDirectory()
    {
    }

    bool KTCacheDirectory::Configure(const KTPStoreNode* node)
    {
        if (node == NULL) return false;

        return SetPath(node->GetData<string>("path", fPath.string()));
    }

} /* namespace Katydid */
