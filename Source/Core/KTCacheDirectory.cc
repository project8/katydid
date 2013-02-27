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
    KTLOGGER(dirlog, "katydid.core");

    KTCacheDirectory::KTCacheDirectory(const std::string& name) :
            KTDirectory(),
            KTConfigurable(name),
            fPreparedForUse(false)
    {
    }

    KTCacheDirectory::~KTCacheDirectory()
    {
    }

    Bool_t KTCacheDirectory::Configure(const KTPStoreNode* node)
    {
        if (node == NULL) return false;

        return SetPath(node->GetData<string>("path", fPath.string()));
    }

    Bool_t KTCacheDirectory::PrepareForUse()
    {
        if (fPreparedForUse) return true;

        KTPStoreNode* node = KTParameterStore::GetInstance()->GetNode(fConfigName);
        if (node != NULL)
        {
            if (! Configure(node))
            {
                KTERROR(dirlog, "An error occurred while configuring the cache directory");
                return false;
            }
            fPreparedForUse = IsOkay();
        }
        return fPreparedForUse;
    }


} /* namespace Katydid */
