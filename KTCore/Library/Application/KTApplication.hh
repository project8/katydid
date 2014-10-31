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
#include "KTConfigurable.hh"
#include "KTConfigurator.hh"

#ifdef ROOT_FOUND
#include "TApplication.h"
#endif

#include <set>
#include <string>

namespace Katydid
{
    class KTEventLoop;
    class KTParamNode;

    /*!
     @class KTApplication
     @author N. S. Oblath

     @brief Interface for the command-line handler and the configurator.

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

     Event Loops:
     KTApplication can oversee the running of event loops.  It takes a very light-handed approach to that oversight.
     In the event that the KTApplication object is deleted, all loops it knows about will be stopped.
     Event loops are not deleted.
     If an event loop is going out of scope before the KTApplication object, the user should make sure to remove it from
     KTApplication's oversight.
    */
    class KTApplication : public KTConfigurable
    {
        public:
            KTApplication(bool makeTApp=false);
            /// Constructor to use with command-line options; includes parsing of the command line by KTCommandLineHandler (except for config-file-dependent options)
            KTApplication(int argC, char** argV, bool makeTApp=false, bool requireArgs=true, KTParamNode* defaultConfig=NULL);
            virtual ~KTApplication();

        public:
            bool Configure(const KTParamNode* node);

        public:
            KTCommandLineHandler* GetCommandLineHandler() const;
            const KTConfigurator* GetConfigurator() const;

            const std::string& GetConfigFilename() const;

            void AddEventLoop(KTEventLoop* loop); /// Adds loop to the set of event loops overseen by KTApplication.  Does NOT assume ownership of an event loop
            void RemoveEventLoop(KTEventLoop* loop); /// Removes loop from the set of event loops overseen by KTApplication. Does not stop the loop.

        protected:
            KTCommandLineHandler* fCLHandler;
            KTConfigurator* fConfigurator;

            std::string fConfigFilename;

            std::set< KTEventLoop* > fEventLoops;

#ifdef ROOT_FOUND
        public:
            bool StartTApplication();
            TApplication* GetTApplication() const;

        protected:
            TApplication* fTApp;
#endif
    };

    inline KTCommandLineHandler* KTApplication::GetCommandLineHandler() const
    {
        return fCLHandler;
    }

    inline const KTConfigurator* KTApplication::GetConfigurator() const
    {
        return fConfigurator;
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
