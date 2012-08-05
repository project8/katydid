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

#include "TApplication.h"

#include "Rtypes.h"

#include <string>

namespace Katydid
{
    /*!
     @class KTRectanKTApplicationgularWindow
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
     2. Call KTApplication::ProcessCommandLine() to parse the remaining command-line options.
     3. Call KTApplication::ReadConfigFile() to read the config file and store the values in the parameter store.
     4. Use KTAppilcation::GetNode(address) to get parameter-store nodes.

    */
    class KTApplication
    {
        public:
            KTApplication(Bool_t makeTApp=false);
            KTApplication(int argC, char** argV, Bool_t makeTApp=false);
            virtual ~KTApplication();

            void ProcessCommandLine();

            Bool_t ReadConfigFile();

            KTPStoreNode* GetNode(const std::string address) const;

        public:
            KTCommandLineHandler* GetCommandLineHandler() const;
            KTParameterStore* GetParameterStore() const;

            const std::string& GetConfigFilename() const;

        protected:
            KTCommandLineHandler* fCLHandler;
            KTParameterStore* fParamStore;

            std::string fConfigFilename;

        public:
            TApplication* GetTApplication() const;

        protected:
            TApplication* fTApp;
    };

    inline void KTApplication::ProcessCommandLine()
    {
        fCLHandler->ProcessCommandLine();
        return;
    }

    inline Bool_t KTApplication::ReadConfigFile()
    {
        return fParamStore->ReadConfigFile(fConfigFilename);
    }

    inline KTPStoreNode* KTApplication::GetNode(const std::string address) const
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

    TApplication* KTApplication::GetTApplication() const
    {
        return fTApp;
    }


} /* namespace Katydid */
#endif /* KTAPPLICATION_HH_ */
