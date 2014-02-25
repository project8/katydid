/*
 * TestCacheDirectory.cc
 *
 *  Created on: Dec 3, 2012
 *      Author: nsoblath
 *
 *      Usage:
 *      > TestCacheDirectory
 *
 *      Will first test if the current directory can be used as the cache (the default condition if no other directory is specified.
 *      Then tests if ./TestCacheDirectory can be used as the cache. This will pass if the directory exists and is read/writable.
 */

#include "KTCacheDirectory.hh"

#include "KTLogger.hh"

using namespace Katydid;

KTLOGGER(testlog, "TestCacheDirectory");

int main()
{

    KTCacheDirectory* cacheDir = KTCacheDirectory::GetInstance();

    KTINFO(testlog, "Testing ./ (should pass if the current directory is read/writeable)");
    KTINFO(testlog, "Current cache directory is: " << cacheDir->GetPath());
    KTWARN(testlog, "Is the cache directory OK? " << cacheDir->IsReady());

    KTINFO(testlog, "Testing ./TestCacheDirectory (should pass if ./TestCacheDirectory exists and is read/writeable)");
    if (! cacheDir->SetPath("./TestCacheDirectory"))
    {
        KTERROR(testlog, "Error setting the path");
    }
    KTINFO(testlog, "Current cache directory is: " << cacheDir->GetPath());
    KTWARN(testlog, "Is the cache directory OK? " << cacheDir->IsReady());

    KTINFO(testlog, "Testing /usr/local (should fail, assuming you're not running as root)");
    if (! cacheDir->SetPath("/usr/local"))
    {
        KTERROR(testlog, "Error setting the path");
    }
    KTINFO(testlog, "Current cache directory is: " << cacheDir->GetPath());
    KTWARN(testlog, "Is the cache directory OK? " << cacheDir->IsReady());

    return 0;
}
