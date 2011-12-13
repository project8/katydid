/*
 * KTCommandLineHandler.hh
 *
 *  Created on: Nov 21, 2011
 *      Author: nsoblath
 */

#ifndef KTCOMMANDLINEHANDLER_H_
#define KTCOMMANDLINEHANDLER_H_

#include "KTDestroyer.hh"
#include "KTIOMessage.hh"

#include "Rtypes.h"

#include <boost/program_options.hhpp>
namespace po = boost::program_options;

#include <map>
using std::map;
#include <set>
using std::set;
#include <string>
using std::string;
#include <vector>
using std::vector;

namespace Katydid
{
    class KTCommandLineUser;

    class KTCommandLineHandler
    {
        protected:
            typedef map< string, po::options_description* > OptDescMap;
            typedef OptDescMap::iterator OptDescMapIt;
            typedef OptDescMap::const_iterator OptDescMapCIt;

            //**************
            // Singleton
            //**************

        public:
            static KTCommandLineHandler* GetInstance();

        public:
            friend class KTDestroyer< KTCommandLineHandler >;

        protected:
            KTCommandLineHandler();
            virtual ~KTCommandLineHandler();
            static KTCommandLineHandler* fInstance;
            static KTDestroyer< KTCommandLineHandler > fCLHandlerDestroyer;

            //**************
            // Identification
            //**************

        public:
            const string& GetExecutableName() const;
            const string& GetPackageString() const;

        protected:
            string fExecutableName; // from argv[0], if provided
            string fPackageString; // Package name and version

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
            /// Makes a new option group available for command line options. (note: FinalizeNewOoptionGroup must be called to make the options useable)
            Bool_t ProposeNewOptionGroup(const string& aKey, const string& aTitle);

            /// Simple option adding function, with short option (flag only; no values allowed)
            Bool_t AddOption(const string& aKey, const string& aHelpMsg, const string& aLongOpt, Char_t aShortOpt);
            /// Simple option adding function, without short option (flag only; no values allowed)
            Bool_t AddOption(const string& aKey, const string& aHelpMsg, const string& aLongOpt);

            /// Option-with-value adding function with short option
            template< class XValueType >
            Bool_t AddOption(const string& aKey, const string& aHelpMsg, const string& aLongOpt, Char_t aShortOpt='#');
            /// Option-with-value adding function without short option
            template< class XValueType >
            Bool_t AddOption(const string& aKey, const string& aHelpMsg, const string& aLongOpt);

            /// Request access to the options description object for more freedom (and responsibility!) in adding options
            po::options_description* GetOptionsDescription(const string& aKey);

            /// Adds the group of options to the set of useable options groups (note: this must be called to make the options in the group useable)
            Bool_t FinalizeNewOptionGroup(const string& aKey);

        protected:
            OptDescMap fProposedGroups;
            set< string > fAllGroupKeys;
            set< string > fAllOptionsLong;
            set< Char_t > fAllOptionsShort;

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
            vector< string > fCommandLineParseLater;

            //**************
            // CL Users
            //**************

        public:
            virtual void RegisterUser(KTCommandLineUser* aUser);
            virtual void RemoveUser(KTCommandLineUser* aUser);

        protected:
            virtual void AddOptionsFromAllUsers();
            virtual void NotifyUsersOfParsing();

            set< KTCommandLineUser* > fUsers;

            //**************
            // Access to values
            //**************

        public:
            /// Check if a command line option was set
            Bool_t IsCommandLineOptSet(const string& aCLOption);

            /// Return the value of a command line option
            template< class XReturnType >
            XReturnType GetCommandLineValue(const string& aCLOption);

            /// Return the file name provided by the user on the command line for the IO ("User") config file
            const string& GetIOConfigFileName() const;

        protected:
            po::variables_map fCommandLineVarMap;

            Bool_t fPrintHelpMessage;
            Bool_t fPrintVersion;
            string fIOConfigFileName;

            //**************
            // Print useful information
            //**************

        public:
            Bool_t GetPrintHelpMessageFlag();
            Bool_t GetPrintVersionFlag();

            virtual void PrintHelpMessageAndExit(const string& aApplicationType="");
            virtual void PrintVersionMessageAndExit(const string& aApplicationType="", const string& aApplicationString="");
    };

    inline const string& KTCommandLineHandler::GetExecutableName() const
    {
        return fExecutableName;
    }
    inline const string& KTCommandLineHandler::GetPackageString() const
    {
        return fPackageString;
    }

    template< class XValueType >
    Bool_t KTCommandLineHandler::AddOption(const string& aKey, const string& aHelpMsg, const string& aLongOpt, Char_t aShortOpt)
    {
        OptDescMapIt tIter = fProposedGroups.find(aKey);
        if (tIter == fProposedGroups.end())
        {
            iomsg < "KTCommandLineHandler::AddOption";
            iomsg(eWarning) << "There no proposed option group with key <" << aKey << ">" << eom;
            return kFALSE;
        }
        if (fAllOptionsLong.find(aLongOpt) != fAllOptionsLong.end())
        {
            iomsg < "KTCommandLineHandler::AddOption";
            iomsg(eWarning) << "There is already an option called <" << aLongOpt << ">" << eom;
            return kFALSE;
        }
        if (fAllOptionsShort.find(aShortOpt) != fAllOptionsShort.end())
        {
            iomsg < "KTCommandLineHandler::AddOption";
            iomsg(eWarning) << "There is already a short option called <" << aShortOpt << ">" << eom;
            return kFALSE;
        }

        // option is okay at this point

        string tOptionName = aLongOpt;
        fAllOptionsLong.insert(aLongOpt);
        tOptionName += "," + string(&aShortOpt);
        fAllOptionsShort.insert(aShortOpt);

        tIter->second->add_options()(tOptionName.c_str(), po::value< XValueType >(), aHelpMsg.c_str());

        return kTRUE;

    }

    template< class XValueType >
    Bool_t KTCommandLineHandler::AddOption(const string& aKey, const string& aHelpMsg, const string& aLongOpt)
    {
        OptDescMapIt tIter = fProposedGroups.find(aKey);
        if (tIter == fProposedGroups.end())
        {
            iomsg < "KTCommandLineHandler::AddOption";
            iomsg(eWarning) << "There no proposed option group with key <" << aKey << ">" << eom;
            return kFALSE;
        }
        if (fAllOptionsLong.find(aLongOpt) != fAllOptionsLong.end())
        {
            iomsg < "KTCommandLineHandler::AddOption";
            iomsg(eWarning) << "There is already an option called <" << aLongOpt << ">" << eom;
            return kFALSE;
        }

        // option is okay at this point

        fAllOptionsLong.insert(aLongOpt);

        tIter->second->add_options()(aLongOpt.c_str(), po::value< XValueType >(), aHelpMsg.c_str());

        return kTRUE;

    }


    template< class XReturnType >
    XReturnType KTCommandLineHandler::GetCommandLineValue(const string& aCLOption)
    {
        if (!IsCommandLineOptSet(aCLOption))
        {
            iomsg < "KTCommandLineHandler::GetCommandLineValue";
            iomsg(eError) << "Command line option <" << aCLOption << "> was not set!" << ret;
            iomsg << "Next time check whether it's set before calling this function." << eom;
        }
        return fCommandLineVarMap[aCLOption].as< XReturnType >();
    }

    inline const string& KTCommandLineHandler::GetIOConfigFileName() const
    {
        return fIOConfigFileName;
    }

    inline Bool_t KTCommandLineHandler::GetPrintHelpMessageFlag()
    {
        return fPrintHelpMessage;
    }

    inline Bool_t KTCommandLineHandler::GetPrintVersionFlag()
    {
        return fPrintVersion;
    }

} /* namespace Katydid */
#endif /* KTCOMMANDLINEHANDLER_H_ */
