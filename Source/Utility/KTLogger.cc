/*
 * KTLogger.cxx
 *
 *  Created on: 18.11.2011 (KLogger from KATRIN's Kasper package)
 *      Author: Marco Haag <marco.haag@kit.edu>
 *
 *  Ported to Katydid on: 01.05.2012
 *      By: Noah Oblath
 */

#include "KTLogger.hh"

#if defined(LOG4CXX)

#include <cstdlib>

#include <log4cxx/logstring.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/patternlayout.h>
#include <log4cxx/consoleappender.h>
#include <log4cxx/logmanager.h>
#include <log4cxx/logger.h>

using namespace std;
using namespace log4cxx;

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
                    LayoutPtr layout(new PatternLayout(TTCC_CONVERSION_PATTERN));
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

    KTLogger::KTLogger(const std::string& name) : fPrivate(new Private)
    {
        fPrivate->fLogger = Logger::getLogger(name);
    }

    bool KTLogger::IsLevelEnabled(const string& level) const
    {
        return fPrivate->fLogger->isEnabledFor(Level::toLevel(level));
    }

    bool KTLogger::IsDebugEnabled() const
    {
        return fPrivate->fLogger->isDebugEnabled();
    }

    bool KTLogger::IsInfoEnabled() const
    {
        return fPrivate->fLogger->isInfoEnabled();
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
            void logCout(const string& level, const string& message, const Location& /*loc*/) {
                cout << __DATE__ " " __TIME__ " [" << setw(5) << level << "] " << setw(16) << fLogger << ": " << message << endl;
            }
            void logCerr(const string& level, const string& message, const Location& /*loc*/) {
                cerr << __DATE__ " " __TIME__ " [" << setw(5) << level << "] " << setw(16) << fLogger << ": " << message << endl;
            }
    };

    KTLogger::KTLogger(const char *name) : fPrivate(new Private)
    {
        const char* logName = strrchr(name, '/') ? strrchr(name, '/') + 1 : name;
        fPrivate->fLogger = logName;
    }

    bool KTLogger::IsLevelEnabled(const string& level) const
    {
#ifdef NDEBUG
        return !boost::icontains(level, "debug");
#else
        (void) level;
        return true;
#endif
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
        fPrivate->logCout("DEBUG", message, loc);
    }

    void KTLogger::logInfo (const string &message, const Location& loc)
    {
        fPrivate->logCout("INFO", message, loc);
    }

    void KTLogger::logWarn (const string &message, const Location& loc)
    {
        fPrivate->logCout("WARN", message, loc);
    }

    void KTLogger::logError (const string &message, const Location& loc)
    {
        fPrivate->logCerr("ERROR", message, loc);
    }

    void KTLogger::logFatal (const string &message, const Location& loc)
    {
        fPrivate->logCerr("FATAL", message, loc);
    }
}

#endif
