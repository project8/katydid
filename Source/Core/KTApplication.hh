/**
 @file KTApplication.hh
 @brief Contains KTApplication
 @details Interface for the command-line handler and the parameter store
 @author: N. S. Oblath
 @date: Created on: Aug 5, 2012
 */

#ifndef KTAPPLICATION_HH_
#define KTAPPLICATION_HH_

#include "KTCommandLineHandler.hh"
#include "KTParameterStore.hh"
#include "KTPStoreNode.hh"

#ifdef ROOT_FOUND
#include "TApplication.h"
#endif

#include "Rtypes.h"

#include <string>

namespace Katydid
{
    class KTConfigurable;

    /*!
     @class KTApplication
     @author N. S. Oblath

     @brief Interface for the command-line handler and the parameter store.

     @details
     The interface implemented here is meant to simplify the use of the command line and parameter store.

     The basic format for the command line arguments are as follows:
      - The first argument should be the configuration filename, if you're using one.
      - All further arguments should either use the "long format" or the "short format":
          o Long format: --long-option-name[=value]
          o Short format: -s[value]
        Strings with spaces should be put in quotation marks.

     When creating an application, the user would perform the following steps:
     1. Create the KTApplication and pass it the command-line arguments (argc and argv).
        Note: if the user wants to build a ROOT application, there's a flag which will create a TApplication as well.
        The configuration file will be automatically extracted from the command line.
        If --help (-h) or --version (-v) were given, those will be handled immediately, and the program will exit.
     2. Call KTApplication::ReadConfigFile() to read the config file and store the values in the parameter store.
     3. Use KTAppilcation::GetNode(address) to get parameter-store nodes.

    */
    class KTApplication
    {
        public:
            KTApplication(Bool_t makeTApp=false);
            /// Constructor to use with command-line optiosn; includes parsing of the command line by KTCommandLineHandler (except for config-file-dependent options)
            KTApplication(int argC, char** argV, Bool_t makeTApp=false);
            virtual ~KTApplication();

            /// Parse the config file and store the results (performed by KTParameterStore)
            Bool_t ReadConfigFile();

            /// Parse any unparsed parts of command line and store the results (performed by KTCommandLineHandler)
            /// This is called from ReadConfigFile
            void FinishProcessingCommandLine();

            /// Configure a KTConfigurable object
            /// If baseAddress is given, the KTConfigurable's config name will be appended before attempting to get the parameter store node.
            /// If no baseAddress is given, no parameter store node will be used.
            /// Use baseAddress="" if the parameter store node to be used is a top-level node.
            Bool_t Configure(KTConfigurable* toBeConfigured, const std::string& baseAddress);

            /// Get a node from the parameter store tree
            KTPStoreNode* GetNode(const std::string& address) const;

        protected:
            void AddConfigOptionsToCLHandler(const KTParameterStore::PStoreTree* tree, const std::string& addressOfTree="");
            void ApplyCLOptionsToParamStore(const po::parsed_options* parsedOpts);

        public:
            KTCommandLineHandler* GetCommandLineHandler() const;
            KTParameterStore* GetParameterStore() const;

            const std::string& GetConfigFilename() const;

        protected:
            KTCommandLineHandler* fCLHandler;
            KTParameterStore* fParamStore;

            std::string fConfigFilename;

#ifdef ROOT_FOUND
        public:
            TApplication* GetTApplication() const;

        protected:
            TApplication* fTApp;
#endif
    };

    inline KTPStoreNode* KTApplication::GetNode(const std::string& address) const
    {
        return fParamStore->GetNode(address);
    }

    inline KTCommandLineHandler* KTApplication::GetCommandLineHandler() const
    {
        return fCLHandler;
    }

    inline KTParameterStore* KTApplication::GetParameterStore() const
    {
        return fParamStore;
    }

    inline const std::string& KTApplication::GetConfigFilename() const
    {
        return fConfigFilename;
    }

#ifdef ROOT_FOUND
    TApplication* KTApplication::GetTApplication() const
    {
        return fTApp;
    }
#endif

} /* namespace Katydid */
#endif /* KTAPPLICATION_HH_ */
