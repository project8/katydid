/*
 * KTLevel.hh
 *
 *  Created on: Apr 8, 2013
 *      Author: nsoblath
 */

#ifndef KATYDID_LEVELS_H_
#define KATYDID_LEVELS_H_

#include <log4cxx/level.h>
#include <log4cxx/logger.h>

namespace log4cxx
{
    class KTLevel
    {
        DECLARE_LOG4CXX_LEVEL( KTLevel );

        virtual ~KTLevel() {}

         enum
         {
             PROG_INT = 25000 /* And so forth for the other levels */
         };

         static LevelPtr getProg(); /* and so forth */

         // You must provide this, see the one in Logger.cpp
         // If it doesn't match one of your levels you should probably
         // call Logger::toLevel
         static LevelPtr toLevel(int val);

         // You must provide this, see the one in Logger.cpp
         // If it doesn't match one of your levels you should probably
         // call Logger::toLevelLS
         static LevelPtr toLevelLS(const LogString& sArg);
    };
} /* namespace log4cxx */

#endif
