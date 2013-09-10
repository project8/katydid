/*
 * KTLevel.cc
 *
 *  Created on: Apr 8, 2013
 *      Author: nsoblath
 */

#include "KTLevel.hh"

#include <log4cxx/helpers/stringhelper.h>

using namespace log4cxx;

IMPLEMENT_LOG4CXX_LEVEL( KTLevel );

KTLevel::KTLevel(int level, const LogString& name, int syslogEquivalent) :
        Level(level, name, syslogEquivalent)
{
}


LevelPtr KTLevel::getProg()
{
    static const LevelPtr level(new KTLevel(KTLevel::PROG_INT, LOG4CXX_STR("PROG"), 4));
    return level;
}

LevelPtr KTLevel::toLevel(int val)
{
    switch(val)
    {
        case PROG_INT: return getProg();
        default: return Level::toLevel(val);
    }
}

LevelPtr KTLevel::toLevel(int val, const LevelPtr& defaultLevel)
{
    switch(val)
    {
        case PROG_INT: return getProg();
        default: return Level::toLevel(val, defaultLevel);
    }
}

LevelPtr KTLevel::toLevelLS(const LogString& sArg)
{
    const size_t len = sArg.length();

    if (len == 4)
    {
        if (helpers::StringHelper::equalsIgnoreCase(sArg, LOG4CXX_STR("PROG"), LOG4CXX_STR("prog")))
        {
            return getProg();
        }
    }
    return Level::toLevelLS(sArg);
}

LevelPtr KTLevel::toLevelLS(const LogString& sArg, const LevelPtr& defaultLevel)
{
    const size_t len = sArg.length();

    if (len == 4)
    {
        if (helpers::StringHelper::equalsIgnoreCase(sArg, LOG4CXX_STR("PROG"), LOG4CXX_STR("prog")))
        {
            return getProg();
        }
    }
    return Level::toLevelLS(sArg, defaultLevel);
}
