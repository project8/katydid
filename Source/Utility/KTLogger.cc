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

const string& EndColor() {static string* color = new string(KTCOLOR_PREFIX KTCOLOR_NORMAL KTCOLOR_SUFFIX); return *color;}
const string& FatalColor() {static string* color = new string(KTCOLOR_PREFIX KTCOLOR_BRIGHT KTCOLOR_SEPARATOR KTCOLOR_FOREGROUND_RED    KTCOLOR_SUFFIX); return *color;}
const string& ErrorColor() {static string* color = new string(KTCOLOR_PREFIX KTCOLOR_BRIGHT KTCOLOR_SEPARATOR KTCOLOR_FOREGROUND_RED    KTCOLOR_SUFFIX); return *color;}
const string& WarnColor() {static string* color = new string(KTCOLOR_PREFIX KTCOLOR_BRIGHT KTCOLOR_SEPARATOR KTCOLOR_FOREGROUND_YELLOW KTCOLOR_SUFFIX); return *color;}
const string& ProgColor() {static string* color = new string(KTCOLOR_PREFIX KTCOLOR_BRIGHT KTCOLOR_SEPARATOR KTCOLOR_FOREGROUND_BLUE   KTCOLOR_SUFFIX); return *color;}
const string& InfoColor() {static string* color = new string(KTCOLOR_PREFIX KTCOLOR_BRIGHT KTCOLOR_SEPARATOR KTCOLOR_FOREGROUND_GREEN  KTCOLOR_SUFFIX); return *color;}
const string& DebugColor() {static string* color = new string(KTCOLOR_PREFIX KTCOLOR_BRIGHT KTCOLOR_SEPARATOR KTCOLOR_FOREGROUND_CYAN   KTCOLOR_SUFFIX); return *color;}
const string& OtherColor() {static string* color = new string(KTCOLOR_PREFIX KTCOLOR_BRIGHT KTCOLOR_SEPARATOR KTCOLOR_FOREGROUND_WHITE  KTCOLOR_SUFFIX); return *color;}

#if 0
//#if defined(BOOST_LOG_FOUND)

#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/format.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sources/severity_feature.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/manipulators/to_log.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/formatter_parser.hpp>
#include <boost/move/utility.hpp>

//#include <vector>


/*
namespace Katydid
{
    struct SeverityLevels
    {
        struct LevelAttributes
        {
                std::string fTag;
                std::string fColor;
                //LevelAttributes(const char* tag, const string& color) :
                //    fTag(tag),
                //    fColor(color)
                //{}
        };
        std::vector< LevelAttributes > fLevelAttributes;
        SeverityLevels() :
            fLevelAttributes(7)
        {
            fLevelAttributes[0].fTag = string("TRACE"); fLevelAttributes[0].fColor = string(DebugColor());
            fLevelAttributes[1].fTag = string("DEBUG"); fLevelAttributes[1].fColor = DebugColor();
            fLevelAttributes[2].fTag = string("INFO"); fLevelAttributes[2].fColor = InfoColor();
            fLevelAttributes[3].fTag = string("PROG"); fLevelAttributes[3].fColor = ProgColor();
            fLevelAttributes[4].fTag = string("WARN"); fLevelAttributes[4].fColor = WarnColor();
            fLevelAttributes[5].fTag = string("ERROR"); fLevelAttributes[5].fColor = ErrorColor();
            fLevelAttributes[6].fTag = string("FATAL"); fLevelAttributes[6].fColor = FatalColor();
            //fLevelAttributes.push_back(LevelAttributes("TRACE", skKTDebugColor));
            //fLevelAttributes.push_back(LevelAttributes("DEBUG", skKTDebugColor));
            //fLevelAttributes.push_back(LevelAttributes("INFO", skKTInfoColor));
            //fLevelAttributes.push_back(LevelAttributes("PROG", skKTProgColor));
            //fLevelAttributes.push_back(LevelAttributes("WARN", skKTWarnColor));
            //fLevelAttributes.push_back(LevelAttributes("ERROR", skKTErrorColor));
            //fLevelAttributes.push_back(LevelAttributes("FATAL", skKTFatalColor));
            std::cout << "attributes for trace: " << fLevelAttributes.begin()->fTag << "  " << fLevelAttributes.begin()->fColor << std::endl;
        }

    };
    //std::vector< SeverityLevels::LevelAttributes > SeverityLevels::fLevelAttributes;
    SeverityLevels& GetLevels()
    {
        static SeverityLevels* levels = new SeverityLevels();
        return *levels;
    }
}
*/

struct severity_tag;
boost::log::formatting_ostream& operator<<(
        boost::log::formatting_ostream& strm,
        boost::log::to_log_manip< Katydid::KTLogger::ELevel, severity_tag > const& manip)
{
    static const char* strings[] =
    {
            "TRACE",
            "DEBUG",
            "INFO",
            "PROD",
            "WARN",
            "ERROR",
            "FATAL"
    };
    static std::size_t levels = 7;
    Katydid::KTLogger::ELevel level = manip.get();
    if (static_cast< std::size_t >(level) < levels)
        strm << strings[level];
    else
        strm << "XXXX";

    return strm;
}

struct severity_color;
boost::log::formatting_ostream& operator<<(
        boost::log::formatting_ostream& strm,
        boost::log::to_log_manip< Katydid::KTLogger::ELevel, severity_color > const& manip)
{
    static const char* strings[] =
    {
            KTCOLOR_PREFIX KTCOLOR_BRIGHT KTCOLOR_SEPARATOR KTCOLOR_FOREGROUND_CYAN   KTCOLOR_SUFFIX,
            KTCOLOR_PREFIX KTCOLOR_BRIGHT KTCOLOR_SEPARATOR KTCOLOR_FOREGROUND_CYAN   KTCOLOR_SUFFIX,
            KTCOLOR_PREFIX KTCOLOR_BRIGHT KTCOLOR_SEPARATOR KTCOLOR_FOREGROUND_GREEN  KTCOLOR_SUFFIX,
            KTCOLOR_PREFIX KTCOLOR_BRIGHT KTCOLOR_SEPARATOR KTCOLOR_FOREGROUND_BLUE   KTCOLOR_SUFFIX,
            KTCOLOR_PREFIX KTCOLOR_BRIGHT KTCOLOR_SEPARATOR KTCOLOR_FOREGROUND_YELLOW KTCOLOR_SUFFIX,
            KTCOLOR_PREFIX KTCOLOR_BRIGHT KTCOLOR_SEPARATOR KTCOLOR_FOREGROUND_RED    KTCOLOR_SUFFIX,
            KTCOLOR_PREFIX KTCOLOR_BRIGHT KTCOLOR_SEPARATOR KTCOLOR_FOREGROUND_RED    KTCOLOR_SUFFIX
    };
    /*static const char* strings[] =
    {
            KTCOLOR_PREFIX KTCOLOR_BRIGHT KTCOLOR_SEPARATOR KTCOLOR_FOREGROUND_CYAN   KTCOLOR_SUFFIX,
            KTCOLOR_PREFIX KTCOLOR_BRIGHT KTCOLOR_SEPARATOR KTCOLOR_FOREGROUND_CYAN   KTCOLOR_SUFFIX,
            KTCOLOR_PREFIX KTCOLOR_BRIGHT KTCOLOR_SEPARATOR KTCOLOR_FOREGROUND_GREEN  KTCOLOR_SUFFIX,
            KTCOLOR_PREFIX KTCOLOR_BRIGHT KTCOLOR_SEPARATOR KTCOLOR_FOREGROUND_BLUE   KTCOLOR_SUFFIX,
            KTCOLOR_PREFIX KTCOLOR_BRIGHT KTCOLOR_SEPARATOR KTCOLOR_FOREGROUND_YELLOW KTCOLOR_SUFFIX,
            KTCOLOR_PREFIX KTCOLOR_BRIGHT KTCOLOR_SEPARATOR KTCOLOR_FOREGROUND_RED    KTCOLOR_SUFFIX,
            KTCOLOR_PREFIX KTCOLOR_BRIGHT KTCOLOR_SEPARATOR KTCOLOR_FOREGROUND_RED    KTCOLOR_SUFFIX
    };*/
    static std::size_t levels = 7;
    Katydid::KTLogger::ELevel level = manip.get();
    if (static_cast< std::size_t >(level) < levels)
        strm << strings[level];
    else
        strm << skKTOtherColor;

    return strm;
}



namespace Katydid
{


    struct KTLogger::Private
    {
            boost::log::sources::severity_logger< KTLogger::ELevel > fLogger;

            void SetThreshold(ELevel thresh)
            {
                return;
            }

            ELevel fThreshold;
    };

    KTLogger::KTLogger(const char* name) : fPrivate(new Private())
    {
        //Katydid::GetLevels();
        boost::shared_ptr< boost::log::sinks::synchronous_sink< boost::log::sinks::text_ostream_backend > > sink =
            boost::log::add_console_log(std::cout,
                boost::log::keywords::format =
                    (
                        boost::log::expressions::stream
                        << boost::log::expressions::attr< KTLogger::ELevel, severity_color >("Severity")
                        //<< Private::level2Color(level)
                        << boost::log::expressions::format_date_time< boost::posix_time::ptime >("TimeStamp", "%H:%M:%S.")
                        << boost::log::expressions::format_date_time< boost::posix_time::ptime >("TimeStamp", "%f")
                        << ": [" << boost::log::expressions::attr< KTLogger::ELevel, severity_tag >("Severity") << "] "
                        << boost::log::expressions::smessage
                        << EndColor()
                    )
        );
        sink->locked_backend()->auto_flush(true);
        boost::log::add_common_attributes();
        SetLevel(eDebug);
    }

    KTLogger::KTLogger(const std::string& name) : fPrivate(new Private())
    {
        //boost::log::register_simple_formatter_factory< string, char >("Color");
        //Katydid::GetLevels();

        boost::shared_ptr< boost::log::sinks::synchronous_sink< boost::log::sinks::text_ostream_backend > > sink =
            boost::log::add_console_log(std::cout,
                boost::log::keywords::format =
                    (
                            boost::log::expressions::stream
                            << boost::log::expressions::attr< KTLogger::ELevel, severity_color >("Severity")
                            //<< Private::level2Color(level)
                            << boost::log::expressions::format_date_time< boost::posix_time::ptime >("TimeStamp", "%H:%M:%S.%F")
                            << ": [" << boost::log::expressions::attr< KTLogger::ELevel, severity_tag >("Severity") << "] "
                            << boost::log::expressions::smessage
                            << skKTEndColor
                    )
        );
        sink->locked_backend()->auto_flush(true);
        boost::log::add_common_attributes();
        SetLevel(eDebug);
    }

    KTLogger::~KTLogger()
    {
        delete fPrivate;
    }

    bool KTLogger::IsLevelEnabled(ELevel level) const
    {
        return level >= fPrivate->fThreshold;
    }

    void KTLogger::SetLevel(ELevel level) const
    {
#if defined(NDEBUG) && defined(STANDARD)
                fPrivate->fThreshold = level >= eInfo ? level : eInfo;
#elif defined(NDEBUG)
                fPrivate->fThreshold = level >= eProg ? level : eProg;
#else
                fPrivate->fThreshold = level;
#endif
    }

    void KTLogger::Log(ELevel level, const string& message, const Location& loc)
    {
        std::cout << "in KTLogger::Log" << std::endl;
        BOOST_LOG_SEV(fPrivate->fLogger, level) << boost::format(" %1%(%2%) ") % loc.fFileName % loc.fLineNumber << message;
    }
}


//#elif defined(LOG4CXX_FOUND)
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
        cout << "### leven being set to: " << level << endl;
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

#include <cstdio>
#include <iomanip>
#include <sys/time.h>
#include <time.h>

namespace Katydid
{
    struct KTLogger::Private
    {
            static char sDateTimeFormat[16];
            static time_t sRawTime;
            static tm* sProcessedTime;
            static char sTimeBuff[512];
            static size_t getTimeAbsoluteStr()
            {
                time(&KTLogger::Private::sRawTime);
                sProcessedTime = gmtime(&KTLogger::Private::sRawTime);
                return strftime(KTLogger::Private::sTimeBuff, 512,
                        KTLogger::Private::sDateTimeFormat,
                        KTLogger::Private::sProcessedTime);
            }

            const char* fLogger;
            bool fColored;
            ELevel fThreshold;

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
                    case eFatal : return skKTFatalColor; break;
                    default     : return skKTOtherColor;
                }
            }


            void logCout(const char* level, const string& message, const Location& loc, const string& color = skKTOtherColor)
            {
                getTimeAbsoluteStr();
                if (fColored)
                {
                    cout << color << KTLogger::Private::sTimeBuff << " [" << setw(5) << level << "] " << setw(16) << left << loc.fFileName << "(" << loc.fLineNumber  << "): " << message << skKTEndColor << endl;
                    //cout << color << KTLogger::Private::sTimeBuff << " [" << setw(5) << level << "] ";
                    //cout.width(16); cout << left << loc.fFileName << "(" << loc.fLineNumber  << "): ";
                    //cout << message << skKTEndColor << endl;
                }
                else
                    cout << KTLogger::Private::sTimeBuff << " [" << setw(5) << level << "] " << setw(16) << left << loc.fFileName << "(" << loc.fLineNumber  << "): " << message << endl;
            }

            void logCerr(const char* level, const string& message, const Location& loc, const string& color = skKTOtherColor)
            {
                getTimeAbsoluteStr();
                if (fColored)
                    cerr << color << KTLogger::Private::sTimeBuff << " [" << setw(5) << level << "] " << setw(16) << left << loc.fFileName << "(" << loc.fLineNumber  << "): " << message << skKTEndColor << endl;
                else
                    cerr << KTLogger::Private::sTimeBuff <<  " [" << setw(5) << level << "] " << setw(16) << left << loc.fFileName << "(" << loc.fLineNumber  << "): " << message << endl;
            }
    };

    char KTLogger::Private::sDateTimeFormat[16];
    time_t KTLogger::Private::sRawTime;
    tm* KTLogger::Private::sProcessedTime;
    char KTLogger::Private::sTimeBuff[512];

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
        sprintf(KTLogger::Private::sDateTimeFormat,  "%%T");
        SetLevel(eDebug);
    }

    KTLogger::KTLogger(const std::string& name) : fPrivate(new Private())
    {
        fPrivate->fLogger = name.c_str();
        fPrivate->fColored = true;
        sprintf(KTLogger::Private::sDateTimeFormat,  "%%T");
        SetLevel(eDebug);
    }

    KTLogger::~KTLogger()
    {
        delete fPrivate;
    }

    bool KTLogger::IsLevelEnabled(ELevel level) const
    {
        return level >= fPrivate->fThreshold;
    }

    void KTLogger::SetLevel(ELevel level) const
    {
#if defined(NDEBUG) && defined(STANDARD)
                fPrivate->fThreshold = level >= eInfo ? level : eInfo;
#elif defined(NDEBUG)
                fPrivate->fThreshold = level >= eProg ? level : eProg;
#else
                fPrivate->fThreshold = level;
#endif
    }

    void KTLogger::Log(ELevel level, const string& message, const Location& loc)
    {
        const char* levelStr = Private::level2Str(level);

        if (level >= eWarn)
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
