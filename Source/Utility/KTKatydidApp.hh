/**
 @file KTKatydidApp.hh
 @brief Contains KTApplication
 @details Interface for the command-line handler and the parameter store
 @author: N. S. Oblath
 @date: Created on: Aug 5, 2012
 */

#ifndef KTKATYDIDAPP_HH_
#define KTKATYDIDAPP_HH_

#include "KTApplication.hh"

#ifdef ROOT_FOUND
#include "TApplication.h"
#endif


namespace Katydid
{
    /*!
     @class KTKatydidApp
     @author N. S. Oblath

     @brief Interface for the command-line handler and the configurator with an optional ROOT TApplication.

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
    class KTKatydidApp : public Nymph::KTApplication
    {
        public:
            KTKatydidApp(bool makeTApp=false);
            /// Constructor to use with command-line options; includes parsing of the command line by KTCommandLineHandler (except for config-file-dependent options)
            KTKatydidApp(int argC, char** argV, bool makeTApp=false, bool requireArgs=true, scarab::param_node* defaultConfig=NULL);
            virtual ~KTKatydidApp();

        public:
            virtual bool Configure(const scarab::param_node* node);

#ifdef ROOT_FOUND
        public:
            bool StartTApplication();
            TApplication* GetTApplication() const;

        protected:
            TApplication* fTApp;
#endif
    };

#ifdef ROOT_FOUND
    TApplication* KTKatydidApp::GetTApplication() const
    {
        return fTApp;
    }
#endif

} /* namespace Katydid */
#endif /* KTKATYDIDAPP_HH_ */
