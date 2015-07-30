/*
 * KTDirectory.cc
 *
 *  Created on: Dec 3, 2012
 *      Author: nsoblath
 */

#include "KTDirectory.hh"

#include "KTLogger.hh"

#include <unistd.h>

using std::string;

using boost::filesystem::path;

namespace Nymph
{
    KTLOGGER(dirlog, "KTDirectory");

    KTDirectory::KTDirectory() :
            fPath(""),
            fPathExists(false),
            fAccess(eNone)
    {
        SetPath("./");
    }

    KTDirectory::~KTDirectory()
    {
    }

    const string& KTDirectory::GetPath() const
    {
        return fPath.string();
    }

    bool KTDirectory::GetPathExists() const
    {
        return fPathExists;
    }

    KTDirectory::Access KTDirectory::GetAccess() const
    {
        return fAccess;
    }

    bool KTDirectory::SetPath(const string& strPath)
    {
        path newPath(strPath);
        if (! newPath.is_absolute())
        {
            newPath = absolute(newPath);
        }
        if (! exists(newPath))
        {
            KTERROR(dirlog, "Path does not exist: " << strPath);
            return false;
        }
        if (! is_directory(newPath))
        {
            KTERROR(dirlog, "Path specified is not a directory: " << strPath);
            return false;
        }
        fPathExists = true;
        fPath = newPath;

        // set access
        if (access(strPath.c_str(), R_OK | W_OK) != -1)
        {
            fAccess = eReadWrite;
        }
        else if (access(strPath.c_str(), R_OK))
        {
            fAccess = eRead;
        }
        else
        {
            fAccess = eNone;
        }

        return true;
    }

} /* namespace Nymph */
