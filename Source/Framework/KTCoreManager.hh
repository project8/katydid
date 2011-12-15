#ifndef KTCORE_MANAGER_H
#define KTCORE_MANAGER_H
/**
 * @file

 * @brief contains KTCoreManager

 <b>Revision History:</b>
 \verbatim
 Date         Name          Brief description
 -----------------------------------------------
 29 Mar 2010   N. Oblath     First version
 13 Apr 2010   N. Oblath     Made a singleton; added parameter status map
 14 Jul 2010   D. Furse      Major changes in preparation for radon run
 07 Sep 2010   M. Babutzka   Added command line variables (fargc, fargv) with getters and setters
 18 Jan 2011   N. Oblath     Separated KTCoreManagerSimulation from KTCoreManager
 \endverbatim

 *
 */

/*!
 * @class Katydid::KTCoreManager
 @author N. Oblath

 @brief Base class for the primary manager for applications in Katydid

 @details
 <b>Detailed Description:</b><br>
 Classes derived from KTCoreManager (e.g. KTCoreManagerSimulation) perform the actions specific to each application.  See KTCoreManagerSimulation for notes on how to make a derived class of KTCoreManager for an application.

 */

#include "KTManager.hh"
//#include "KTCommandLineUser.hh"

#include "KTFrameworkMessage.hh"

#include <vector>
#include <deque>
#include <map>

namespace Katydid
{
    class KTCoreManagerFactory;
    class KTCoreManagerTable;

    class KTCoreManager :
        public KTManager//,
        //public KTCommandLineUser
    {
        public:
            friend class KTCoreManagerFactory;
            friend class KTCoreManagerTable;

        protected:
            KTCoreManager(Int_t aNArgs = 0, Char_t** anArgList = 0);
            KTCoreManager();
            virtual ~KTCoreManager();

            //*************
            // Application interfaces (usually not overridden)

        public:
            //! Responsible for running Setup() and Prepare()
            virtual void SetupApplication();
            //! Responsible for running Execute()
            virtual void RunApplication();
            //! Responsible for running Shutdown()
            virtual void ShutdownApplication();

            //*************
            //Top-level control functions (usually not overridden or used; these are called by the Application methods)

        public:
            // From the Toolboxes/Managers
            //! Establishes the manager hierarchy
            virtual Bool_t Setup();
            //! Reads configuration files
            virtual Bool_t Prepare();
            //! Shutdown manager hierarchy
            virtual Bool_t Shutdown();
            //! Abort the core manager
            virtual Bool_t Abort();

            //! Starts the application (must be overridden)
            virtual Bool_t Execute() = 0;

            //*************
            //CoreManager-specific control functions

        protected:
            //! Setup for the CoreManager
            virtual Bool_t SetupManager() = 0;
            //! Prepare for the CoreManager
            virtual Bool_t PrepareManager() = 0;
            //! Shutdown for the CoreManager
            virtual Bool_t ShutdownManager() = 0;

            //*************
            //Recursive shutdown functions (usually not overridden)

        protected:
            ///sets up this manager's dependencies
            virtual Bool_t SetupDependentManagers();
            ///protected method for starting the shutdown process for this core manager's manager aspect and its dependencies
            virtual Bool_t RecursiveManagerShutdown();
            ///shuts down this core manager's manager depencencies
            virtual Bool_t ShutdownDependentManagers();

            //*************
            // Setup the management dependencies (should not be overridden)

        public:
            virtual void AddManager(const string& aType);

            /// You might as well use the version without the bool, because it's not used for CoreManagers
            virtual void AddManager(const string& aType, Bool_t /*b*/);


            //*************
            // Identification

        public:
            const string& GetApplicationType() const;
            const string& GetApplicationString() const;

        protected:
            string fApplicationType;
            string fApplicationString;
/*
            //*************
            // Command line options

        public:
            virtual void AddCommandLineOptions();
            virtual void UseParsedCommandLineImmediately();

            virtual void AddDerivedCommandLineOptions() = 0;
            virtual void UseDerivedParsedCommandLineImmediately() = 0;
*/
    };

    inline const string& KTCoreManager::GetApplicationType() const
    {
        return fApplicationType;
    }
    inline const string& KTCoreManager::GetApplicationString() const
    {
        return fApplicationString;
    }


} // end Katydid namespace

#endif // KTCORE_MANAGER_H
