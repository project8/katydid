/*
 * KTLogger.cxx
 *
 *  Created on: 18.11.2011 (KLogger from KATRIN's Kasper package)
 *      Author: Marco Haag <marco.haag@kit.edu> and Sebastian Voecking
 *
 *  Ported to Katydid on: 01.05.2012
 *      By: Noah Oblath
 */

#include "KTLogger.hh"

#ifdef LOG4CXX_FOUND

#include <cstdlib>

#include <log4cxx/helpers/synchronized.h>
#include <log4cxx/logstring.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/patternlayout.h>
#include <log4cxx/consoleappender.h>
#include <log4cxx/logmanager.h>
#include <log4cxx/logger.h>

#include <boost/algorithm/string.hpp>

using namespace std;
using namespace log4cxx;

IMPLEMENT_LOG4CXX_OBJECT(KTColoredPatternLayout)

const string KTColoredPatternLayout::fNormal = COLOR_NORMAL;
const string KTColoredPatternLayout::fBright = COLOR_BRIGHT;
const string KTColoredPatternLayout::fForegroundRed = COLOR_FOREGROUND_RED;
const string KTColoredPatternLayout::fForegroundGreen = COLOR_FOREGROUND_GREEN;
const string KTColoredPatternLayout::fForegroundYellow = COLOR_FOREGROUND_YELLOW;
const string KTColoredPatternLayout::fForegroundBlue = COLOR_FOREGROUND_BLUE;
const string KTColoredPatternLayout::fForegroundCyan = COLOR_FOREGROUND_CYAN;
const string KTColoredPatternLayout::fForegroundWhite = COLOR_FOREGROUND_WHITE;
const string KTColoredPatternLayout::fPrefix = COLOR_PREFIX;
const string KTColoredPatternLayout::fSuffix = COLOR_SUFFIX;
const string KTColoredPatternLayout::fSeparator = COLOR_SEPARATOR;

const string KTColoredPatternLayout::fEndColor = KTColoredPatternLayout::fPrefix + KTColoredPatternLayout::fNormal + KTColoredPatternLayout::fSuffix;
const string KTColoredPatternLayout::fFatalColor = KTColoredPatternLayout::fPrefix + KTColoredPatternLayout::fBright + KTColoredPatternLayout::fSeparator + KTColoredPatternLayout::fForegroundRed + fSuffix;
const string KTColoredPatternLayout::fErrorColor = KTColoredPatternLayout::fPrefix + KTColoredPatternLayout::fBright + KTColoredPatternLayout::fSeparator + KTColoredPatternLayout::fForegroundRed + fSuffix;
const string KTColoredPatternLayout::fWarnColor = KTColoredPatternLayout::fPrefix + KTColoredPatternLayout::fBright + KTColoredPatternLayout::fSeparator + KTColoredPatternLayout::fForegroundYellow + fSuffix;
const string KTColoredPatternLayout::fProgColor = KTColoredPatternLayout::fPrefix + KTColoredPatternLayout::fBright + KTColoredPatternLayout::fSeparator + KTColoredPatternLayout::fForegroundBlue + fSuffix;
const string KTColoredPatternLayout::fInfoColor = KTColoredPatternLayout::fPrefix + KTColoredPatternLayout::fBright + KTColoredPatternLayout::fSeparator + KTColoredPatternLayout::fForegroundGreen + fSuffix;
const string KTColoredPatternLayout::fDebugColor = KTColoredPatternLayout::fPrefix + KTColoredPatternLayout::fBright + KTColoredPatternLayout::fSeparator + KTColoredPatternLayout::fForegroundCyan + fSuffix;
const string KTColoredPatternLayout::fOtherColor = KTColoredPatternLayout::fPrefix + KTColoredPatternLayout::fBright + KTColoredPatternLayout::fSeparator + KTColoredPatternLayout::fForegroundWhite + fSuffix;


KTColoredPatternLayout::KTColoredPatternLayout() :
        PatternLayout()
{
}

KTColoredPatternLayout::KTColoredPatternLayout(const LogString& pattern) :
        PatternLayout(pattern)
{
}

KTColoredPatternLayout::~KTColoredPatternLayout()
{
}

void KTColoredPatternLayout::format(LogString& output, const spi::LoggingEventPtr& event, helpers::Pool& pool) const
{
    PatternLayout::format(output, event, pool);
    output = getColor(event->getLevel()) + output + fEndColor;
    return;
}

string KTColoredPatternLayout::getColor(const LevelPtr& level) const
{
    switch(level->toInt())
    {
        case Level::FATAL_INT:
            return fFatalColor;
            break;
        case Level::ERROR_INT:
            return fErrorColor;
            break;
        case Level::WARN_INT:
            return fWarnColor;
            break;
        case KTLevel::PROG_INT:
            return fProgColor;
            break;
        case Level::INFO_INT:
            return fInfoColor;
            break;
        case Level::DEBUG_INT:
            return fDebugColor;
            break;
        default:
            return fOtherColor;
    }
}


namespace {

    struct StaticInitializer {
            StaticInitializer() {

                if (LogManager::getLoggerRepository()->isConfigured())
                    return;
                //        AppenderList appenders = Logger::getRootLogger()->getAllAppenders();

                char* envLoggerConfig;
                envLoggerConfig = getenv("LOGGER_CONFIGURATION");
                if (envLoggerConfig != NULL) {
                    PropertyConfigurator::configure(envLoggerConfig);
                }
                else {
#ifdef LOGGER_CONFIGURATION
                    PropertyConfigurator::configure(LOGGER_CONFIGURATION);
#else
                    LogManager::getLoggerRepository()->setConfigured(true);
                    LoggerPtr root = Logger::getRootLogger();
#ifdef NDEBUG
                    Logger::getRootLogger()->setLevel(Level::getInfo());
#endif
                    static const LogString TTCC_CONVERSION_PATTERN(LOG4CXX_STR("%r [%-5p] %16c: %m%n"));
                    LayoutPtr layout(new KTColoredPatternLayout(TTCC_CONVERSION_PATTERN));
                    AppenderPtr appender(new ConsoleAppender(layout));
                    root->addAppender(appender);
#endif
                }

            }
    } static sLoggerInitializer;

}

namespace Katydid
{
    struct KTLogger::Private
    {
            LoggerPtr fLogger;
            void log(const LevelPtr& level, const string& message, const Location& loc) {
                fLogger->forcedLog(level, message, ::log4cxx::spi::LocationInfo(loc.fFileName, loc.fFunctionName, loc.fLineNumber));
            }
    };

    KTLogger::KTLogger(const char *name) : fPrivate(new Private)
    {
        //    const char* logName = strrchr(name, '/') ? strrchr(name, '/') + 1 : name;
        fPrivate->fLogger = Logger::getLogger(name);
    }

    KTLogger::KTLogger(const string& name) : fPrivate(new Private)
    {
        fPrivate->fLogger = Logger::getLogger(name);
    }

    bool KTLogger::IsLevelEnabled(const string& level) const
    {
#ifdef STANDARD
        if (boost::icontains(level, "debug"))
            return false;
        else
            return fPrivate->fLogger->isEnabledFor(Level::toLevel(level));
#else // STANDARD
#ifdef NDEBUG
        if (boost::icontains(level, "debug") || boost::icontains(level, "info"))
            return false;
        else
            return fPrivate->fLogger->isEnabledFor(Level::toLevel(level));
#else // NDEBUG
        return fPrivate->fLogger->isEnabledFor(Level::toLevel(level));
#endif // NDEBUG
#endif // STANDARD
    }

    bool KTLogger::IsDebugEnabled() const
    {
#ifdef NDEBUG
        return false;
#else
        return fPrivate->fLogger->isDebugEnabled();
#endif
    }

    bool KTLogger::IsInfoEnabled() const
    {
#ifdef STANDARD
        return fPrivate->fLogger->isInfoEnabled();
#else // STANDARD
#ifdef NDEBUG
        return false;
#else // NDEBUG
        return fPrivate->fLogger->isInfoEnabled();
#endif // NDEBUG
#endif // STANDARD
    }

    bool KTLogger::IsProgEnabled() const
    {
        /*
        if(fPrivate->fLogger->repository == 0 || fPrivate->fLogger->repository->isDisabled(KTLevel::PROG_INT))
        {
            return false;
        }
        */
        return fPrivate->fLogger->getEffectiveLevel()->toInt() <= KTLevel::PROG_INT;
    }

    bool KTLogger::IsWarnEnabled() const
    {
        return fPrivate->fLogger->isWarnEnabled();
    }

    bool KTLogger::IsErrorEnabled() const
    {
        return fPrivate->fLogger->isErrorEnabled();
    }

    bool KTLogger::IsFatalEnabled() const
    {
        return fPrivate->fLogger->isFatalEnabled();
    }

    void KTLogger::logLevel(const string& level, const string& message, const Location& loc)
    {
        fPrivate->log(Level::toLevel(level), message, loc);
    }

    void KTLogger::logDebug (const string &message, const Location& loc)
    {
        fPrivate->log(Level::getDebug(), message, loc);
    }

    void KTLogger::logInfo (const string &message, const Location& loc)
    {
        fPrivate->log(Level::getInfo(), message, loc);
    }

    void KTLogger::logProg (const string &message, const Location& loc)
    {
        fPrivate->log(KTLevel::getProg(), message, loc);
    }

    void KTLogger::logWarn (const string &message, const Location& loc)
    {
        fPrivate->log(Level::getWarn(), message, loc);
    }

    void KTLogger::logError (const string &message, const Location& loc)
    {
        fPrivate->log(Level::getError(), message, loc);
    }

    void KTLogger::logFatal (const string &message, const Location& loc)
    {
        fPrivate->log(Level::getFatal(), message, loc);
    }
}


#else

#include <boost/algorithm/string.hpp>
#include <iomanip>

using namespace std;

namespace Katydid
{
    struct KTLogger::Private
    {
            const char* fLogger;
            void logCout(const string& level, const string& message, const Location& /*loc*/, const string& color=fOtherColor) {
                cout << color << __DATE__ " " __TIME__ " [" << setw(5) << level << "] " << setw(16) << fLogger << ": " << message << fEndColor << endl;
            }
            void logCerr(const string& level, const string& message, const Location& /*loc*/, const string& color=fOtherColor) {
                cerr << color << __DATE__ " " __TIME__ " [" << setw(5) << level << "] " << setw(16) << fLogger << ": " << message << fEndColor << endl;
            }
            const static string fNormal;
            const static string fBright;
            const static string fForegroundRed;
            const static string fForegroundGreen;
            const static string fForegroundYellow;
            const static string fForegroundBlue;
            const static string fForegroundCyan;
            const static string fForegroundWhite;

            const static string fPrefix;
            const static string fSuffix;
            const static string fSeparator;

            const static string fEndColor;
            const static string fFatalColor;
            const static string fErrorColor;
            const static string fWarnColor;
            const static string fProgColor;
            const static string fInfoColor;
            const static string fDebugColor;
            const static string fOtherColor;

    };

    const string KTLogger::Private::fNormal = COLOR_NORMAL;
    const string KTLogger::Private::fBright = COLOR_BRIGHT;
    const string KTLogger::Private::fForegroundRed = COLOR_FOREGROUND_RED;
    const string KTLogger::Private::fForegroundGreen = COLOR_FOREGROUND_GREEN;
    const string KTLogger::Private::fForegroundYellow = COLOR_FOREGROUND_YELLOW;
    const string KTLogger::Private::fForegroundBlue = COLOR_FOREGROUND_BLUE;
    const string KTLogger::Private::fForegroundCyan = COLOR_FOREGROUND_CYAN;
    const string KTLogger::Private::fForegroundWhite = COLOR_FOREGROUND_WHITE;
    const string KTLogger::Private::fPrefix = COLOR_PREFIX;
    const string KTLogger::Private::fSuffix = COLOR_SUFFIX;
    const string KTLogger::Private::fSeparator = COLOR_SEPARATOR;

    const string KTLogger::Private::fEndColor = fPrefix + fNormal + fSuffix;
    const string KTLogger::Private::fFatalColor = fPrefix + fBright + fSeparator + fForegroundRed + fSuffix;
    const string KTLogger::Private::fErrorColor = fPrefix + fBright + fSeparator + fForegroundRed + fSuffix;
    const string KTLogger::Private::fWarnColor = fPrefix + fBright + fSeparator + fForegroundYellow + fSuffix;
    const string KTLogger::Private::fProgColor = fPrefix + fBright + fSeparator + fForegroundBlue + fSuffix;
    const string KTLogger::Private::fInfoColor = fPrefix + fBright + fSeparator + fForegroundGreen + fSuffix;
    const string KTLogger::Private::fDebugColor = fPrefix + fBright + fSeparator + fForegroundCyan + fSuffix;
    const string KTLogger::Private::fOtherColor = fPrefix + fBright + fSeparator + fForegroundWhite + fSuffix;


    KTLogger::KTLogger(const char *name) : fPrivate(new Private)
    {
        const char* logName = strrchr(name, '/') ? strrchr(name, '/') + 1 : name;
        fPrivate->fLogger = logName;
    }

    bool KTLogger::IsLevelEnabled(const string& level) const
    {
#ifdef STANDARD
        return !boost::icontains(level, "debug");
#else // STANDARD
#ifdef NDEBUG
        return !boost::icontains(level, "debug") && !boost::icontains(level, "info");
#else // NDEBUG
        (void) level;
        return true;
#endif // NDEBUG
#endif // STANDARD
    }

    bool KTLogger::IsDebugEnabled() const
    {
#ifdef NDEBUG
        return false;
#else
        return true;
#endif
    }

    bool KTLogger::IsInfoEnabled() const
    {
#ifdef STANDARD
        return true;
#else // STANDARD
#ifdef NDEBUG
        return false;
#else // NDEBUG
        return true;
#endif // NDEBUG
#endif // STANDARD
    }

    bool KTLogger::IsProgEnabled() const
    {
        return true;
    }

    bool KTLogger::IsWarnEnabled() const
    {
        return true;
    }

    bool KTLogger::IsErrorEnabled() const
    {
        return true;
    }

    bool KTLogger::IsFatalEnabled() const
    {
        return true;
    }

    void KTLogger::logLevel(const string& level, const string& message, const Location& loc)
    {
        if (boost::icontains(level, "error") || boost::icontains(level, "fatal")) {
            fPrivate->logCerr(level, message, loc);
        }
        else {
            fPrivate->logCout(level, message, loc);
        }
    }

    void KTLogger::logDebug (const string &message, const Location& loc)
    {
        fPrivate->logCout("DEBUG", message, loc, KTLogger::Private::fDebugColor);
    }

    void KTLogger::logInfo (const string &message, const Location& loc)
    {
        fPrivate->logCout("INFO", message, loc, KTLogger::Private::fInfoColor);
    }

    void KTLogger::logProg (const string &message, const Location& loc)
    {
        fPrivate->logCout("PROG", message, loc, KTLogger::Private::fProgColor);
    }

    void KTLogger::logWarn (const string &message, const Location& loc)
    {
        fPrivate->logCout("WARN", message, loc, KTLogger::Private::fWarnColor);
    }

    void KTLogger::logError (const string &message, const Location& loc)
    {
        fPrivate->logCerr("ERROR", message, loc, KTLogger::Private::fErrorColor);
    }

    void KTLogger::logFatal (const string &message, const Location& loc)
    {
        fPrivate->logCerr("FATAL", message, loc, KTLogger::Private::fFatalColor);
    }
}

#endif
