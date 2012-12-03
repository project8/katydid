/*
 * KTDirectory.cc
 *
 *  Created on: Dec 3, 2012
 *      Author: nsoblath
 */

#include "KTDirectory.hh"

using std::string;

namespace Katydid
{

    KTDirectory::KTDirectory() :
            fPath(""),
            fAccess(eNone)
    {
    }

    KTDirectory::~KTDirectory()
    {
    }

    const string& KTDirectory::GetPath()
    {
        return fPath;
    }

    KTDirectory::Access KTDirectory::GetAccess()
    {
        return fAccess;
    }

} /* namespace Katydid */
