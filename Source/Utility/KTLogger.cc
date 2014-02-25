/*
 * KTLogger.cc
 *
 *  Created on: Jan 21, 2014
 *      Author: nsoblath
 */

/*
 * KTLogger.cc
 * based on KLogger.cxx from KATRIN's Kasper
 *
 *  Created on: 18.11.2011
 *      Author: Marco Haag <marco.haag@kit.edu>
 */

#include "KTLogger.hh"



#include <cstdlib>
#include <cstring>

using namespace std;

static const string skKTEndColor =   KTCOLOR_PREFIX KTCOLOR_NORMAL KTCOLOR_SUFFIX;
static const string skKTFatalColor = KTCOLOR_PREFIX KTCOLOR_BRIGHT KTCOLOR_SEPARATOR KTCOLOR_FOREGROUND_RED    KTCOLOR_SUFFIX;
static const string skKTErrorColor = KTCOLOR_PREFIX KTCOLOR_BRIGHT KTCOLOR_SEPARATOR KTCOLOR_FOREGROUND_RED    KTCOLOR_SUFFIX;
static const string skKTWarnColor =  KTCOLOR_PREFIX KTCOLOR_BRIGHT KTCOLOR_SEPARATOR KTCOLOR_FOREGROUND_YELLOW KTCOLOR_SUFFIX;
static const string skKTProgColor =  KTCOLOR_PREFIX KTCOLOR_BRIGHT KTCOLOR_SEPARATOR KTCOLOR_FOREGROUND_BLUE   KTCOLOR_SUFFIX;
static const string skKTInfoColor =  KTCOLOR_PREFIX KTCOLOR_BRIGHT KTCOLOR_SEPARATOR KTCOLOR_FOREGROUND_GREEN  KTCOLOR_SUFFIX;
static const string skKTDebugColor = KTCOLOR_PREFIX KTCOLOR_BRIGHT KTCOLOR_SEPARATOR KTCOLOR_FOREGROUND_CYAN   KTCOLOR_SUFFIX;
static const string skKTOtherColor = KTCOLOR_PREFIX KTCOLOR_BRIGHT KTCOLOR_SEPARATOR KTCOLOR_FOREGROUND_WHITE  KTCOLOR_SUFFIX;


#if defined(LOG4CXX_FOUND)
#include "log4cxx/KTLevel.hh"

/*
 * Default implementation for systems with the 'log4cxx' library installed.
 */

#include <log4cxx/logstring.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/patternlayout.h>
#include <log4cxx/level.h>
#include <log4cxx/consoleappender.h>
#include <log4cxx/logmanager.h>
#include <log4cxx/logger.h>

using namespace log4cxx;

#ifndef KT_LOG4CXX_COLORED_PATTERN_LAYOUT_H
#define KT_LOG4CXX_COLORED_PATTERN_LAYOUT_H

namespace log4cxx {

    class LOG4CXX_EXPORT KTColoredPatternLayout : public PatternLayout
    {
        public:
            DECLARE_LOG4CXX_OBJECT(KTColoredPatternLayout)
            BEGIN_LOG4CXX_CAST_MAP()
            LOG4CXX_CAST_ENTRY(KTColoredPatternLayout)
            LOG4CXX_CAST_ENTRY_CHAIN(Layout)
            END_LOG4CXX_CAST_MAP()

        KTColoredPatternLayout() : PatternLayout() {}
            KTColoredPatternLayout(const LogString& pattern) : PatternLayout(pattern) {};
            virtual ~KTColoredPatternLayout() {}

        protected:
            virtual void format(LogString& output, const spi::LoggingEventPtr& event, helpers::Pool& pool) const;
            virtual std::string getColor(const LevelPtr& level) const;

    };

    LOG4CXX_PTR_DEF(KTColoredPatternLayout);

}

#endif /* KT_LOG4CXX_COLORED_PATTERN_LAYOUT_H */

IMPLEMENT_LOG4CXX_OBJECT(KTColoredPatternLayout)

void KTColoredPatternLayout::format(LogString& output, const spi::LoggingEventPtr& event, helpers::Pool& pool) const
{
    PatternLayout::format(output, event, pool);
    output = getColor(event->getLevel()) + output + skKTEndColor;
    return;
}

string KTColoredPatternLayout::getColor(const LevelPtr& level) const
{
    switch(level->toInt())
    {
        case Level::FATAL_INT:
            return skKTFatalColor;
            break;
        case Level::ERROR_INT:
            return skKTErrorColor;
            break;
        case Level::WARN_INT:
            return skKTWarnColor;
            break;
        case KTLevel::PROG_INT:
            return skKTProgColor;
            break;
        case Level::INFO_INT:
            return skKTInfoColor;
            break;
        case Level::DEBUG_INT:
            return skKTDebugColor;
            break;
        case Level::TRACE_INT:
            return skKTDebugColor;
            break;
        default:
            return skKTOtherColor;
    }
}

namespace Katydid
{

    struct KTStaticInitializer
    {
            KTStaticInitializer()
            {
                // the check of whether the logger repository is configured was removed to allow nested use of the log4cxx-based logger.
                // otherwise the logger from the parent package isn't able to override the settings of the child.
                ///if (LogManager::getLoggerRepository()->isConfigured())
                ///    return;
                //        AppenderList appenders = Logger::getRootLogger()->getAllAppenders();

                char* envLoggerConfig;
                envLoggerConfig = getenv("LOGGER_CONFIGURATION");
                if (envLoggerConfig != 0)
                {
                    PropertyConfigurator::configure(envLoggerConfig);
                }
                else
                {
#ifdef LOGGER_CONFIGURATION
                    PropertyConfigurator::configure(LOGGER_CONFIGURATION);
#else
                    LogManager::getLoggerRepository()->setConfigured(true);
                    LoggerPtr root = Logger::getRootLogger();
#ifdef NDEBUG
                    Logger::getRootLogger()->setLevel(Level::getInfo());
#endif
                    static const LogString TTCC_CONVERSION_PATTERN(LOG4CXX_STR("%r [%-5p] %16c: %m%n"));
                    //LayoutPtr layout(new PatternLayout(TTCC_CONVERSION_PATTERN));
                    LayoutPtr layout(new KTColoredPatternLayout(TTCC_CONVERSION_PATTERN));
                    AppenderPtr appender(new ConsoleAppender(layout));
                    root->addAppender(appender);
#endif
                }

            }
    } static sKTLoggerInitializer;

}

namespace Katydid
{
    struct KTLogger::Private
    {
            void log(const LevelPtr& level, const string& message, const Location& loc)
            {
                fLogger->forcedLog(level, message, ::log4cxx::spi::LocationInfo(loc.fFileName, loc.fFunctionName, loc.fLineNumber));
            }

            static LevelPtr level2Ptr(ELevel level)
            {
                switch(level)
                {
                    case eTrace : return KTLevel::getTrace();
                    case eDebug : return KTLevel::getDebug();
                    case eInfo  : return KTLevel::getInfo();
                    case eProg  : return KTLevel::getProg();
                    case eWarn  : return KTLevel::getWarn();
                    case eError : return KTLevel::getError();
                    case eFatal : return KTLevel::getFatal();
                    default     : return KTLevel::getOff();
                }
            }

            LoggerPtr fLogger;
    };

    KTLogger::KTLogger(const char* name) : fPrivate(new Private())
    {
        fPrivate->fLogger = (name == 0) ? Logger::getRootLogger() : Logger::getLogger(name);
    }

    KTLogger::KTLogger(const std::string& name) : fPrivate(new Private())
    {
        fPrivate->fLogger = Logger::getLogger(name);
    }

    KTLogger::~KTLogger()
    {
        delete fPrivate;
    }

    bool KTLogger::IsLevelEnabled(ELevel level) const
    {
        return fPrivate->fLogger->isEnabledFor( Private::level2Ptr(level) );
    }

    void KTLogger::SetLevel(ELevel level) const
    {
        fPrivate->fLogger->setLevel( Private::level2Ptr(level) );
    }

    void KTLogger::Log(ELevel level, const string& message, const Location& loc)
    {
        fPrivate->log(Private::level2Ptr(level), message, loc);
    }
}


#else

/**
 * Fallback solution for systems without log4cxx.
 */

#include <iomanip>

namespace Katydid
{
    struct KTLogger::Private
    {
            const char* fLogger;
            bool fColored;

            static const char* level2Str(ELevel level)
            {
                switch(level)
                {
                    case eTrace : return "TRACE"; break;
                    case eDebug : return "DEBUG"; break;
                    case eInfo  : return "INFO"; break;
                    case eProg  : return "PROG"; break;
                    case eWarn  : return "WARN"; break;
                    case eError : return "ERROR"; break;
                    case eFatal : return "FATAL"; break;
                    default     : return "XXX";
                }
            }

            static string level2Color(ELevel level)
            {
                switch(level)
                {
                    case eTrace : return skKTDebugColor; break;
                    case eDebug : return skKTDebugColor; break;
                    case eInfo  : return skKTInfoColor; break;
                    case eProg  : return skKTProgColor; break;
                    case eWarn  : return skKTWarnColor; break;
                    case eError : return skKTErrorColor; break;
                    case eFatal : return skKTErrorColor; break;
                    default     : return skKTOtherColor;
                }
            }


            void logCout(const char* level, const string& message, const Location& /*loc*/, const string& color = skKTOtherColor)
            {
                if (fColored)
                    cout << color << __DATE__ " " __TIME__ " [" << setw(5) << level << "] " << setw(16) << fLogger << ": " << message << skKTEndColor << endl;
                else
                    cout << __DATE__ " " __TIME__ " [" << setw(5) << level << "] " << setw(16) << fLogger << ": " << message << endl;
            }

            void logCerr(const char* level, const string& message, const Location& /*loc*/, const string& color = skKTOtherColor)
            {
                if (fColored)
                    cerr << color << __DATE__ " " __TIME__ " [" << setw(5) << level << "] " << setw(16) << fLogger << ": " << message << skKTEndColor << endl;
                else
                    cerr << __DATE__ " " __TIME__ " [" << setw(5) << level << "] " << setw(16) << fLogger << ": " << message << endl;
            }
    };

    KTLogger::KTLogger(const char* name) : fPrivate(new Private())
    {
        if (name == 0)
        {
            fPrivate->fLogger = "root";
        }
        else
        {
            const char* logName = strrchr(name, '/') ? strrchr(name, '/') + 1 : name;
            fPrivate->fLogger = logName;
        }
        fPrivate->fColored = true;
    }

    KTLogger::KTLogger(const std::string& name) : fPrivate(new Private())
    {
        fPrivate->fLogger = name.c_str();
        fPrivate->fColored = true;
    }

    KTLogger::~KTLogger()
    {
        delete fPrivate;
    }

    bool KTLogger::IsLevelEnabled(ELevel level) const
    {
#ifdef NDEBUG
        return level > eDebug;
#else
        (void) level;
        return true;
#endif
    }

    void KTLogger::SetLevel(ELevel /*level*/) const
    {
        // TODO: implement levels for fallback KTLogger
    }

    void KTLogger::Log(ELevel level, const string& message, const Location& loc)
    {
        const char* levelStr = Private::level2Str(level);

        if (level >= eFatal)
        {
            fPrivate->logCerr(levelStr, message, loc, Private::level2Color(level));
        }
        else
        {
            fPrivate->logCout(levelStr, message, loc, Private::level2Color(level));
        }
    }
}

#endif
