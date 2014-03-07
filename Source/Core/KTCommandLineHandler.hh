/**
 @file KTCommandLineHandler.hh
 @brief Contains KTCommandLineHandler
 @details Parses and stores command-line options
 @author: N. S. Oblath
 @date: Created on: Nov 21, 2011
 */

#ifndef KTCOMMANDLINEHANDLER_H_
#define KTCOMMANDLINEHANDLER_H_

#include "KTLogger.hh"
#include "KTParam.hh"
#include "KTSingleton.hh"

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

namespace Katydid
{
    KTLOGGER(utillog_clh, "KTCommandLineHandler.hh");

    class CommandLineHandlerException : public std::logic_error
    {
        public:
            CommandLineHandlerException(std::string const& why);
    };


    /*!
     @class KTCommandLineHandler
     @author N. S. Oblath

     @brief Parses and stores command-line options

     @details
     The basic format for the command line arguments are as follows:
      <!-- - The first argument should be the configuration filename, if you're using one. -->
      - All further arguments should either use the "long format" or the "short format":
          o Long format: --long-option-name[=value]
                         --long-option-name [value]
          o Short format: -s[value]
                          -s [value]
        Strings with spaces should be put in quotation marks.

     The configuration file will be automatically extracted from the command line.
     If --help (-h) or --version (-v) were given, those will be handled immediately, and the program will exit.
    */

    class KTCommandLineHandler : public KTSingleton< KTCommandLineHandler >
    {
        protected:
            typedef std::map< std::string, po::options_description* > OptDescMap;
            typedef OptDescMap::iterator OptDescMapIt;
            typedef OptDescMap::const_iterator OptDescMapCIt;

            //**************
            // Singleton
            //**************

        protected:
            friend class KTSingleton< KTCommandLineHandler >;
            friend class KTDestroyer< KTCommandLineHandler >;
            KTCommandLineHandler();
            virtual ~KTCommandLineHandler();

            //**************
            // Identification
            //**************

        public:
            const std::string& GetExecutableName() const;
            const std::string& GetPackageString() const;

        private:
            std::string fExecutableName; // from argv[0], if provided
            std::string fPackageString; // Package name and version

            //**************
            // Command line arguments
            //**************

        public:
            bool TakeArguments(int argC, char** argV);
            bool GetArgumentsTaken();
            int GetNArgs();
            char** GetArgV();

        private:
            int fNArgs;
            char** fArgV;
            bool fArgumentsTaken;

            bool ProcessCommandLine();

            //**************
            // Option-adding interface
            //**************

        public:
            /// Makes a new option group available for command line options.
            OptDescMapIt CreateNewOptionGroup(const std::string& aTitle);

            /// Simple option adding function, with short option (flag only; no values allowed)
            bool AddOption(const std::string& aTitle, const std::string& aHelpMsg, const std::string& aLongOpt, char aShortOpt, bool aWarnOnDuplicate=true);
            /// Simple option adding function, without short option (flag only; no values allowed)
            bool AddOption(const std::string& aTitle, const std::string& aHelpMsg, const std::string& aLongOpt, bool aWarnOnDuplicate=true);

            /// Option-with-value adding function with short option
            template< class XValueType >
            bool AddOption(const std::string& aTitle, const std::string& aHelpMsg, const std::string& aLongOpt, char aShortOpt, bool aWarnOnDuplicate=true);
            /// Option-with-value adding function without short option
            template< class XValueType >
            bool AddOption(const std::string& aTitle, const std::string& aHelpMsg, const std::string& aLongOpt, bool aWarnOnDuplicate=true);

            /// Request access to the options description object for more freedom (and responsibility!) in adding options
            po::options_description* GetOptionsDescription(const std::string& aKey);

            /// Adds the groups of options to the set of usable options groups (note: this must be called to make the options in the groups usable)
            bool FinalizeNewOptionGroups();

        protected:
            OptDescMap fProposedGroups;
            std::set< std::string > fAllGroupKeys;
            std::set< std::string > fAllOptionsLong;
            std::set< char > fAllOptionsShort;

            po::options_description fCommandLineOptions;
            po::options_description fPrintHelpOptions;

        protected:
            /// Adds a set of command line options
            bool AddCommandLineOptions(const po::options_description& aSetOfOpts);

            //**************
            // Parsing
            //**************

        public:
            /// Parses the remaining command line options (those that weren't parsed during the InitialCommandLineProcessing
            bool DelayedCommandLineProcessing();

        private:
            /// Parses the general options and stores the remaining options available for later parsing
            void InitialCommandLineProcessing();

        private:
            std::vector< std::string > fCommandLineParseLater;

            //**************
            // Access to values
            //**************

        public:
            /// Check if a command line option was set
            bool IsCommandLineOptSet(const std::string& aCLOption);

            /// Return the value of a command line option; throws an exception if the value was not set
            template< class XReturnType >
            XReturnType GetCommandLineValue(const std::string& aCLOption);

            /// Return the value of a command line option; returns the default if the value was not set
            template< class XReturnType >
            XReturnType GetCommandLineValue(const std::string& aCLOption, XReturnType defaultValue);

            bool GetPrintHelpMessageFlag() const;
            bool GetPrintHelpMessageAfterConfigFlag() const;
            bool GetPrintVersionMessage() const;

            /// Return the file name provided by the user on the command line for the config file
            const std::string& GetConfigFilename() const;

            /// Return the string of json provided by the user on the command line
            const std::string& GetCommandLineJSON() const;

            const po::parsed_options* GetParsedOptions() const;
            const po::variables_map* GetVariablesMap() const;

            const KTParamNode* GetConfigOverride() const;

        private:
            po::parsed_options fParsedOptions;
            po::variables_map fCommandLineVarMap;

            KTParamNode fConfigOverrideValues;
            static const char fDash = '-';
            static const char fSeparator = '=';
            static const char fNodeSeparator = '.';

            bool fPrintHelpMessage;
            bool fPrintHelpMessageAfterConfig;
            bool fPrintVersionMessage;

            std::string fConfigFilename;
            std::string fCommandLineJSON;

            //**************
            // Print useful information
            //**************

        public:
            virtual void PrintHelpMessage();
            virtual void PrintVersionMessage();
    };

    inline const std::string& KTCommandLineHandler::GetExecutableName() const
    {
        return fExecutableName;
    }
    inline const std::string& KTCommandLineHandler::GetPackageString() const
    {
        return fPackageString;
    }

    template< class XValueType >
    bool KTCommandLineHandler::AddOption(const std::string& aTitle, const std::string& aHelpMsg, const std::string& aLongOpt, char aShortOpt, bool aWarnOnDuplicate)
    {
        if (fAllOptionsLong.find(aLongOpt) != fAllOptionsLong.end())
        {
            if (aWarnOnDuplicate)
                KTWARN(utillog_clh, "There is already an option called <" << aLongOpt << ">");
            return false;
        }
        if (fAllOptionsShort.find(aShortOpt) != fAllOptionsShort.end())
        {
            if (aWarnOnDuplicate)
                KTWARN(utillog_clh, "There is already a short option called <" << aShortOpt << ">");
            return false;
        }

        // option is okay at this point

        OptDescMapIt tIter = fProposedGroups.find(aTitle);
        if (tIter == fProposedGroups.end())
        {
            tIter = CreateNewOptionGroup(aTitle);
        }

        std::string tOptionName = aLongOpt;
        fAllOptionsLong.insert(aLongOpt);
        tOptionName += "," + std::string(&aShortOpt);
        fAllOptionsShort.insert(aShortOpt);

        tIter->second->add_options()(tOptionName.c_str(), po::value< XValueType >(), aHelpMsg.c_str());

        return true;

    }

    template< class XValueType >
    bool KTCommandLineHandler::AddOption(const std::string& aTitle, const std::string& aHelpMsg, const std::string& aLongOpt, bool aWarnOnDuplicate)
    {
        if (fAllOptionsLong.find(aLongOpt) != fAllOptionsLong.end())
        {
            if (aWarnOnDuplicate)
                KTWARN(utillog_clh, "There is already an option called <" << aLongOpt << ">");
            return false;
        }

        // option is okay at this point

        OptDescMapIt tIter = fProposedGroups.find(aTitle);
        if (tIter == fProposedGroups.end())
        {
            tIter = CreateNewOptionGroup(aTitle);
        }

        fAllOptionsLong.insert(aLongOpt);

        tIter->second->add_options()(aLongOpt.c_str(), po::value< XValueType >(), aHelpMsg.c_str());

        return true;

    }


    template< class XReturnType >
    XReturnType KTCommandLineHandler::GetCommandLineValue(const std::string& aCLOption)
    {
        if (! IsCommandLineOptSet(aCLOption))
        {
            KTWARN(utillog_clh, "Command line option <" << aCLOption << "> was not set!\n"
                   "Next time check whether it's set before calling this function.");
            throw CommandLineHandlerException("Command line option " + aCLOption + " was not set!");
        }
        return fCommandLineVarMap[aCLOption].as< XReturnType >();
    }

    template< class XReturnType >
    XReturnType KTCommandLineHandler::GetCommandLineValue(const std::string& aCLOption, XReturnType defaultValue)
    {
        if (! IsCommandLineOptSet(aCLOption))
        {
            return defaultValue;
        }
        return fCommandLineVarMap[aCLOption].as< XReturnType >();
    }

    inline const std::string& KTCommandLineHandler::GetConfigFilename() const
    {
        return fConfigFilename;
    }

    inline const std::string& KTCommandLineHandler::GetCommandLineJSON() const
    {
        return fCommandLineJSON;
    }

    inline bool KTCommandLineHandler::GetPrintVersionMessage() const
    {
        return fPrintVersionMessage;
    }

    inline bool KTCommandLineHandler::GetPrintHelpMessageFlag() const
    {
        return fPrintHelpMessage;
    }

    inline bool KTCommandLineHandler::GetPrintHelpMessageAfterConfigFlag() const
    {
        return fPrintHelpMessageAfterConfig;
    }

    inline const po::parsed_options* KTCommandLineHandler::GetParsedOptions() const
    {
        return &fParsedOptions;
    }

    inline const po::variables_map* KTCommandLineHandler::GetVariablesMap() const
    {
        return &fCommandLineVarMap;
    }

    inline const KTParamNode* KTCommandLineHandler::GetConfigOverride() const
    {
        return &fConfigOverrideValues;
    }

} /* namespace Katydid */
#endif /* KTCOMMANDLINEHANDLER_H_ */
