/*
 * KTLogger.hh
 * Based on KLogger.h, from KATRIN's Kasper
 *
 *  Created on: Jan 23, 2014
 *      Author: nsoblath
 */

#ifndef KTLOGGER_HH_
#define KTLOGGER_HH_

/**
 * @file
 * @brief Contains the logger class and macros, based on Kasper's KLogger class.
 * @date Created on: 18.11.2011
 * @author Marco Haag <marco.haag@kit.edu>
 *
 */

// UTILITY MACROS

#ifndef LOGGER_UTILITY_MACROS_
#define LOGGER_UTILITY_MACROS_

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define __FILE_LINE__      __FILE__ "(" TOSTRING(__LINE__) ")"
#define __FILENAME_LINE__  (strrchr(__FILE__, '/') ? strrchr(__FILE_LINE__, '/') + 1 : __FILE_LINE__)

#if defined(_MSC_VER)
#if _MSC_VER >= 1300
#define __FUNC__ __FUNCSIG__
#endif
#else
#if defined(__GNUC__)
#define __FUNC__ __PRETTY_FUNCTION__
#endif
#endif
#if !defined(__FUNC__)
#define __FUNC__ ""
#endif

#define va_num_args(...) va_num_args_impl(__VA_ARGS__, 5,4,3,2,1)
#define va_num_args_impl(_1,_2,_3,_4,_5,N,...) N

#define macro_dispatcher(func, ...) macro_dispatcher_(func, va_num_args(__VA_ARGS__))
#define macro_dispatcher_(func, nargs) macro_dispatcher__(func, nargs)
#define macro_dispatcher__(func, nargs) func ## nargs

#endif  /* LOGGER_UTILITY_MACROS_ */

// COLOR DEFINITIONS
#define KTCOLOR_NORMAL "0"
#define KTCOLOR_BRIGHT "1"
#define KTCOLOR_FOREGROUND_RED "31"
#define KTCOLOR_FOREGROUND_GREEN "32"
#define KTCOLOR_FOREGROUND_YELLOW "33"
#define KTCOLOR_FOREGROUND_BLUE "34"
#define KTCOLOR_FOREGROUND_CYAN "36"
#define KTCOLOR_FOREGROUND_WHITE "37"
#define KTCOLOR_PREFIX "\033["
#define KTCOLOR_SUFFIX "m"
#define KTCOLOR_SEPARATOR ";"

// INCLUDES

#include <string>
#include <iostream>
#include <sstream>

// CLASS DEFINITIONS

/**
 * The standard Nymph namespace.
 */
namespace Nymph
{

    /**
     * The Nymph logger.
     *
     * The usage and syntax is inspired by log4j. logger itself uses the log4cxx library if it
     * was available on the system during compiling, otherwise it falls back to std::stream.
     *
     * The logger output can be configured in a file specified with the environment variable
     * @a LOGGER_CONFIGURATION (by default log4cxx.properties in the config directory).
     *
     * In most cases the following macro can be used
     * to instantiate a Logger in your code:
     * <pre>LOGGER(myLogger, "loggerName");</pre>
     *
     * This is equivalent to:
     * <pre>static Nymph::logger myLogger("loggerName");</pre>
     *
     * For logging the following macros can be used. The source code location will then automatically
     * included in the output:
     *
     * <pre>
     * KTLOG(myLogger, level, "message");
     * KTTRACE(myLogger, "message");
     * KTDEBUG(myLogger, "message");
     * KTINFO(myLogger, "message");
     * KTPROG(myLogger, "message");
     * KTWARN(myLogger, "message");
     * KTERROR(myLogger, "message");
     * KTFATAL(myLogger, "message");
     *
     * KTASSERT(myLogger, assertion, "message");
     *
     * KTLOG_ONCE(myLogger, level, "message");
     * KTTRACE_ONCE(myLogger, "message");
     * KTDEBUG_ONCE(myLogger, "message");
     * KTINFO_ONCE(myLogger, "message");
     * KTPROG_ONCE(myLogger, "message");
     * KTWARN_ONCE(myLogger, "message");
     * KTERROR_ONCE(myLogger, "message");
     * KTFATAL_ONCE(myLogger, "message");
     * </pre>
     *
     */
    class KTLogger
    {
        public:
            enum ELevel {
                eTrace, eDebug, eInfo, eProg, eWarn, eError, eFatal
            };

        public:
            /**
             * A simple struct used by the Logger macros to pass information about the filename and line number.
             * Not to be used directly by the user!
             */
            struct Location {
                Location(const char* const fileName = "", const char* const functionName = "", int lineNumber = -1) :
                    fLineNumber(lineNumber), fFileName(fileName), fFunctionName(functionName)
                { }
                int fLineNumber;
                std::string fFileName;
                std::string fFunctionName;
            };

        public:
            static KTLogger& GetRootLogger() {
                static KTLogger rootLogger;
                return rootLogger;
            }

        public:
            /**
             * Standard constructor assigning a name to the logger instance.
             * @param name The logger name.
             */
            KTLogger(const char* name = 0);
            /// @overload
            KTLogger(const std::string& name);

            virtual ~KTLogger();

            /**
             * Check whether a certain log-level is enabled.
             * @param level The log level as string representation.
             * @return
             */
            bool IsLevelEnabled(ELevel level) const;

            /**
             * Set a loggers minimum logging level
             * @param level string identifying the log level
             */
            void SetLevel(ELevel level) const;

            /**
             * Log a message with the specified level.
             * Use the macro LOG(logger, level, message).
             * @param level The log level.
             * @param message The message.
             * @param loc Source code location (set automatically by the corresponding macro).
             */
            void Log(ELevel level, const std::string& message, const Location& loc = Location());

            /**
             * Log a message at TRACE level.
             * Use the macro TRACE(logger, message).
             * @param message The message.
             * @param loc Source code location (set automatically by the corresponding macro).
             */
            void LogTrace(const std::string& message, const Location& loc = Location())
            {
                Log(eTrace, message, loc);
            }
            /**
             * Log a message at DEBUG level.
             * Use the macro DEBUG(logger, message).
             * @param message The message.
             * @param loc Source code location (set automatically by the corresponding macro).
             */
            void LogDebug(const std::string& message, const Location& loc = Location())
            {
                Log(eDebug, message, loc);
            }
            /**
             * Log a message at INFO level.
             * Use the macro INFO(logger, message).
             * @param message The message.
             * @param loc Source code location (set automatically by the corresponding macro).
             */
            void LogInfo(const std::string& message, const Location& loc = Location())
            {
                Log(eInfo, message, loc);
            }
            /**
             * Log a message at PROG level.
             * Use the macro PROG(logger, message).
             * @param message The message.
             * @param loc Source code location (set automatically by the corresponding macro).
             */
            void LogProg(const std::string& message, const Location& loc = Location())
            {
                Log(eProg, message, loc);
            }
            /**
             * Log a message at WARN level.
             * Use the macro WARN(logger, message).
             * @param message The message.
             * @param loc Source code location (set automatically by the corresponding macro).
             */
            void LogWarn(const std::string& message, const Location& loc = Location())
            {
                Log(eWarn, message, loc);
            }
            /**
             * Log a message at ERROR level.
             * Use the macro ERROR(logger, message).
             * @param message The message.
             * @param loc Source code location (set automatically by the corresponding macro).
             */
            void LogError(const std::string& message, const Location& loc = Location())
            {
                Log(eError, message, loc);
            }
            /**
             * Log a message at FATAL level.
             * Use the macro FATAL(logger, message).
             * @param message The message.
             * @param loc Source code location (set automatically by the corresponding macro).
             */
            void LogFatal(const std::string& message, const Location& loc = Location())
            {
                Log(eFatal, message, loc);
            }

        private:
            struct Private;
            Private* fPrivate;
    };

}

// PRIVATE MACROS

#define __KTDEFAULT_LOGGER        ::Nymph::KTLogger::GetRootLogger()

#define __KTLOG_LOCATION         ::Nymph::KTLogger::Location(__FILE__, __FUNC__, __LINE__)

#define __KTLOG_LOG_4(I,L,M,O) \
        { \
    if (I.IsLevelEnabled(::Nymph::KTLogger::e##L)) { \
        static bool _sLoggerMarker = false; \
        if (!O || !_sLoggerMarker) { \
            _sLoggerMarker = true; \
            ::std::ostringstream stream; stream << M; \
            I.Log(::Nymph::KTLogger::e##L, stream.str(), __KTLOG_LOCATION); \
        } \
    } \
        }

#define __KTLOG_LOG_3(I,L,M)     __KTLOG_LOG_4(I,L,M,false)
#define __KTLOG_LOG_2(L,M)       __KTLOG_LOG_4(__KTDEFAULT_LOGGER,L,M,false)
#define __KTLOG_LOG_1(M)         __KTLOG_LOG_4(__KTDEFAULT_LOGGER,Debug,M,false)

#define __KTLOG_TRACE_2(I,M)     __KTLOG_LOG_4(I,Trace,M,false)
#define __KTLOG_TRACE_1(M)       __KTLOG_LOG_4(__KTDEFAULT_LOGGER,Trace,M,false)

#ifndef NDEBUG
#define __KTLOG_DEBUG_2(I,M)     __KTLOG_LOG_4(I,Debug,M,false)
#define __KTLOG_DEBUG_1(M)       __KTLOG_LOG_4(__KTDEFAULT_LOGGER,Debug,M,false)
#else
#define __KTLOG_DEBUG_2(I,M)     __KTLOG_LOG_4(I,Debug,"",false)
#define __KTLOG_DEBUG_1(M)       __KTLOG_LOG_4(__KTDEFAULT_LOGGER,Debug,"",false)
#endif

#define __KTLOG_INFO_2(I,M)      __KTLOG_LOG_4(I,Info,M,false)
#define __KTLOG_INFO_1(M)        __KTLOG_LOG_4(__KTDEFAULT_LOGGER,Info,M,false)

#define __KTLOG_PROG_2(I,M)      __KTLOG_LOG_4(I,Prog,M,false)
#define __KTLOG_PROG_1(M)        __KTLOG_LOG_4(__KTDEFAULT_LOGGER,Prog,M,false)

#define __KTLOG_WARN_2(I,M)      __KTLOG_LOG_4(I,Warn,M,false)
#define __KTLOG_WARN_1(M)        __KTLOG_LOG_4(__KTDEFAULT_LOGGER,Warn,M,false)

#define __KTLOG_ERROR_2(I,M)     __KTLOG_LOG_4(I,Error,M,false)
#define __KTLOG_ERROR_1(M)       __KTLOG_LOG_4(__KTDEFAULT_LOGGER,Error,M,false)

#define __KTLOG_FATAL_2(I,M)     __KTLOG_LOG_4(I,Fatal,M,false)
#define __KTLOG_FATAL_1(M)       __KTLOG_LOG_4(__KTDEFAULT_LOGGER,Fatal,M,false)

#define __KTLOG_ASSERT_3(I,C,M)  if (!(C)) { __MTLOG_ERROR_2(I,M) }
#define __KTLOG_ASSERT_2(C,M)    __KTLOG_ASSERT_3(__KTDEFAULT_LOGGER,C,M)


#define __KTLOG_LOG_ONCE_3(I,L,M)     __KTLOG_LOG_4(I,L,M,true)
#define __KTLOG_LOG_ONCE_2(L,M)       __KTLOG_LOG_4(__KTDEFAULT_LOGGER,L,M,true)
#define __KTLOG_LOG_ONCE_1(M)         __KTLOG_LOG_4(__KTDEFAULT_LOGGER,Debug,M,true)

#define __KTLOG_TRACE_ONCE_2(I,M)     __KTLOG_LOG_4(I,Trace,M,true)
#define __KTLOG_TRACE_ONCE_1(M)       __KTLOG_LOG_4(__KTDEFAULT_LOGGER,Trace,M,true)

#define __KTLOG_DEBUG_ONCE_2(I,M)     __KTLOG_LOG_4(I,Debug,M,true)
#define __KTLOG_DEBUG_ONCE_1(M)       __KTLOG_LOG_4(__KTDEFAULT_LOGGER,Debug,M,true)

#define __KTLOG_INFO_ONCE_2(I,M)      __KTLOG_LOG_4(I,Info,M,true)
#define __KTLOG_INFO_ONCE_1(M)        __KTLOG_LOG_4(__KTDEFAULT_LOGGER,Info,M,true)

#define __KTLOG_PROG_ONCE_2(I,M)      __KTLOG_LOG_4(I,Prog,M,true)
#define __KTLOG_PROG_ONCE_1(M)        __KTLOG_LOG_4(__KTDEFAULT_LOGGER,Prog,M,true)

#define __KTLOG_WARN_ONCE_2(I,M)      __KTLOG_LOG_4(I,Warn,M,true)
#define __KTLOG_WARN_ONCE_1(M)        __KTLOG_LOG_4(__KTDEFAULT_LOGGER,Warn,M,true)

#define __KTLOG_ERROR_ONCE_2(I,M)     __KTLOG_LOG_4(I,Error,M,true)
#define __KTLOG_ERROR_ONCE_1(M)       __KTLOG_LOG_4(__KTDEFAULT_LOGGER,Error,M,true)

#define __KTLOG_FATAL_ONCE_2(I,M)     __KTLOG_LOG_4(I,Fatal,M,true)
#define __KTLOG_FATAL_ONCE_1(M)       __KTLOG_LOG_4(__KTDEFAULT_LOGGER,Fatal,M,true)


// PUBLIC MACROS

#define KTLOGGER(I,K)      static ::Nymph::KTLogger I(K);

#define KTLOG(...)         macro_dispatcher(__KTLOG_LOG_, __VA_ARGS__)(__VA_ARGS__)
#define KTTRACE(...)       macro_dispatcher(__KTLOG_TRACE_, __VA_ARGS__)(__VA_ARGS__)
#define KTDEBUG(...)       macro_dispatcher(__KTLOG_DEBUG_, __VA_ARGS__)(__VA_ARGS__)
#define KTINFO(...)        macro_dispatcher(__KTLOG_INFO_, __VA_ARGS__)(__VA_ARGS__)
#define KTPROG(...)        macro_dispatcher(__KTLOG_PROG_, __VA_ARGS__)(__VA_ARGS__)
#define KTWARN(...)        macro_dispatcher(__KTLOG_WARN_, __VA_ARGS__)(__VA_ARGS__)
#define KTERROR(...)       macro_dispatcher(__KTLOG_ERROR_, __VA_ARGS__)(__VA_ARGS__)
#define KTFATAL(...)       macro_dispatcher(__KTLOG_FATAL_, __VA_ARGS__)(__VA_ARGS__)
#define KTASSERT(...)      macro_dispatcher(__KTLOG_ASSERT_, __VA_ARGS__)(__VA_ARGS__)

#define KTLOG_ONCE(...)    macro_dispatcher(__KTLOG_LOG_ONCE_, __VA_ARGS__)(__VA_ARGS__)
#define KTTRACE_ONCE(...)  macro_dispatcher(__KTLOG_TRACE_ONCE_, __VA_ARGS__)(__VA_ARGS__)
#define KTDEBUG_ONCE(...)  macro_dispatcher(__KTLOG_DEBUG_ONCE_, __VA_ARGS__)(__VA_ARGS__)
#define KTINFO_ONCE(...)   macro_dispatcher(__KTLOG_INFO_ONCE_, __VA_ARGS__)(__VA_ARGS__)
#define KTPROG_ONCE(...)   macro_dispatcher(__KTLOG_PROG_ONCE_, __VA_ARGS__)(__VA_ARGS__)
#define KTWARN_ONCE(...)   macro_dispatcher(__KTLOG_WARN_ONCE_, __VA_ARGS__)(__VA_ARGS__)
#define KTERROR_ONCE(...)  macro_dispatcher(__KTLOG_ERROR_ONCE_, __VA_ARGS__)(__VA_ARGS__)
#define KTFATAL_ONCE(...)  macro_dispatcher(__KTLOG_FATAL_ONCE_, __VA_ARGS__)(__VA_ARGS__)

#endif /* KTLOGGER_HH_ */
