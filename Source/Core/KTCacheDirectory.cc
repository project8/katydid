/*
 * KTCacheDirectory.cc
 *
 *  Created on: Dec 3, 2012
 *      Author: nsoblath
 */

#include "KTCacheDirectory.hh"

#include "KTLogger.hh"
#include "KTParam.hh"

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

    bool KTCacheDirectory::Configure(const KTParamNode* node)
    {
        if (node == NULL) return false;

        return SetPath(node->GetValue< string >("path", fPath.string()));
    }

} /* namespace Katydid */
