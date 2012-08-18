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
#include "KTSingleton.hh"

#include "Rtypes.h"

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

namespace Katydid
{
    KTLOGGER(utillog_clh, "katydid.utility");

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
      - The first argument should be the configuration filename, if you're using one.
      - All further arguments should either use the "long format" or the "short format":
          o Long format: --long-option-name[=value]
          o Short format: -s[value]
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

        protected:
            std::string fExecutableName; // from argv[0], if provided
            std::string fPackageString; // Package name and version

            //**************
            // Command line arguments
            //**************

        public:
            Bool_t TakeArguments(Int_t argC, Char_t** argV);
            Bool_t GetArgumentsTaken();
            Int_t GetNArgs();
            Char_t** GetArgV();

        protected:
            Int_t fNArgs;
            Char_t** fArgV;
            Bool_t fArgumentsTaken;

            //**************
            // Option-adding interface
            //**************

        public:
            /// Makes a new option group available for command line options.
            OptDescMapIt CreateNewOptionGroup(const std::string& aTitle);

            /// Simple option adding function, with short option (flag only; no values allowed)
            Bool_t AddOption(const std::string& aTitle, const std::string& aHelpMsg, const std::string& aLongOpt, Char_t aShortOpt);
            /// Simple option adding function, without short option (flag only; no values allowed)
            Bool_t AddOption(const std::string& aTitle, const std::string& aHelpMsg, const std::string& aLongOpt);

            /// Option-with-value adding function with short option
            template< class XValueType >
            Bool_t AddOption(const std::string& aTitle, const std::string& aHelpMsg, const std::string& aLongOpt, Char_t aShortOpt);
            /// Option-with-value adding function without short option
            template< class XValueType >
            Bool_t AddOption(const std::string& aTitle, const std::string& aHelpMsg, const std::string& aLongOpt);

            /// Request access to the options description object for more freedom (and responsibility!) in adding options
            po::options_description* GetOptionsDescription(const std::string& aKey);

            /// Adds the groups of options to the set of usable options groups (note: this must be called to make the options in the groups usable)
            Bool_t FinalizeNewOptionGroups();

        protected:
            OptDescMap fProposedGroups;
            std::set< std::string > fAllGroupKeys;
            std::set< std::string > fAllOptionsLong;
            std::set< Char_t > fAllOptionsShort;

            po::options_description fCommandLineOptions;
            po::options_description fPrintHelpOptions;

        protected:
            /// Adds a set of command line options
            Bool_t AddCommandLineOptions(const po::options_description& aSetOfOpts);

            //**************
            // Parsing
            //**************

        public:
            /// Parses the command line (besides the general options, which are automatically processed)
            void ProcessCommandLine();

        private:
            // Do NOT make this virtual. It is called from the constructor.
            /// Parses the general options and stores the remaining options available for later parsing
            void InitialCommandLineProcessing();

        protected:
            std::vector< std::string > fCommandLineParseLater;

            //**************
            // Access to values
            //**************

        public:
            /// Check if a command line option was set
            Bool_t IsCommandLineOptSet(const std::string& aCLOption);

            /// Return the value of a command line option; throws an exception if the value was not set
            template< class XReturnType >
            XReturnType GetCommandLineValue(const std::string& aCLOption);

            /// Return the value of a command line option; returns the default if the value was not set
            template< class XReturnType >
            XReturnType GetCommandLineValue(const std::string& aCLOption, XReturnType defaultValue);

            /// Return the file name provided by the user on the command line for the config file
            const std::string& GetConfigFilename() const;

            const po::parsed_options* GetParsedOptions() const;
            const po::variables_map* GetVariablesMap() const;

        protected:
            po::parsed_options fParsedOptions;
            po::variables_map fCommandLineVarMap;

            Bool_t fPrintHelpMessageAfterConfig;
            std::string fConfigFilename;

            //**************
            // Print useful information
            //**************

        public:
            Bool_t GetPrintHelpMessageAfterConfigFlag();

            virtual void PrintHelpMessageAndExit();
            virtual void PrintVersionMessageAndExit(const std::string& aApplicationType="", const std::string& aApplicationString="");
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
    Bool_t KTCommandLineHandler::AddOption(const std::string& aTitle, const std::string& aHelpMsg, const std::string& aLongOpt, Char_t aShortOpt)
    {
        if (fAllOptionsLong.find(aLongOpt) != fAllOptionsLong.end())
        {
            KTWARN(utillog_clh, "There is already an option called <" << aLongOpt << ">");
            return kFALSE;
        }
        if (fAllOptionsShort.find(aShortOpt) != fAllOptionsShort.end())
        {
            KTWARN(utillog_clh, "There is already a short option called <" << aShortOpt << ">");
            return kFALSE;
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

        return kTRUE;

    }

    template< class XValueType >
    Bool_t KTCommandLineHandler::AddOption(const std::string& aTitle, const std::string& aHelpMsg, const std::string& aLongOpt)
    {
        if (fAllOptionsLong.find(aLongOpt) != fAllOptionsLong.end())
        {
            KTWARN(utillog_clh, "There is already an option called <" << aLongOpt << ">");
            return kFALSE;
        }

        // option is okay at this point

        OptDescMapIt tIter = fProposedGroups.find(aTitle);
        if (tIter == fProposedGroups.end())
        {
            tIter = CreateNewOptionGroup(aTitle);
        }

        fAllOptionsLong.insert(aLongOpt);

        tIter->second->add_options()(aLongOpt.c_str(), po::value< XValueType >(), aHelpMsg.c_str());

        return kTRUE;

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

    inline Bool_t KTCommandLineHandler::GetPrintHelpMessageAfterConfigFlag()
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

} /* namespace Katydid */
#endif /* KTCOMMANDLINEHANDLER_H_ */
