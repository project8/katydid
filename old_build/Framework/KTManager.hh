#ifndef KTMANAGER_H_
#define KTMANAGER_H_

/**
 @file
 @brief contains KTManager
 @details
 <b>Revision History:</b>
 \verbatim
 Date       Name          Brief description
 -----------------------------------------------
 26/03/2010   N. Oblath     First version
 13/07/2010   D. Furse      Boost pointer maps are out (having to use the try-catch mechanism to check if a key is in the map is inconvenient). Added an add key function to be called in constructors of derived classes.
 31/01/2011   N. Oblath     Removed manager "Type." Use manager "Key" instead.
 15/04/2011   N. Oblath     Split from KTManagerBase
 \endverbatim

 */

#include "KTManagerTable.hh"

#include "Rtypes.h"

#include <map>
using std::map;

namespace Katydid
{
    class KTCommand;

    /*!
     * @class Katydid::KTManager
     @author N. Oblath

     @brief Base class for Managers

     @details
     <b>Detailed Description:</b><br>

     */

    class KTManager
    {
        public:
            enum ManagerStatus
            {
                eBad,
                eInstantiated,
                eSettingUp,
                eSetUp,
                ePreparing,
                ePrepared,
                eShuttingDown,
                eShutDown,
                eAborted
            };

        protected:
            struct DependentManagerInfo
            {
                KTManager* Manager;
                Bool_t obeyOrdering;;
            };

            typedef map< string, DependentManagerInfo > ManagerDependencyMap;
            typedef ManagerDependencyMap::iterator MgrDepMapIt;
            typedef ManagerDependencyMap::const_iterator MgrDepMapCIt;

            typedef map< string, DependentManagerInfo > ManagerDependsOnMeMap;
            typedef ManagerDependsOnMeMap::iterator MgrDepOnMeMapIt;
            typedef ManagerDependsOnMeMap::const_iterator MgrDepOnMeMapCIt;

        public:
            friend class KTManagerFactory;

        protected:
            KTManager( const string& aType );
            virtual ~KTManager();

        protected:
            KTManager();

        public:

            //*************
            //control:
            //
            ///public method for starting the setup process for this manager and its dependencies
            virtual Bool_t Setup();
            //
            ///public method for starting the prepare process for this manager and its dependencies
            virtual Bool_t Prepare();
            //
            ///public method for starting the shutdown process for this manager and its dependencies
            virtual Bool_t Shutdown();
            //
            ///public method for starting the abort process for this manager (also calls KTManagerTable::InitiateAbort())
            virtual Bool_t Abort();
            //
            ///get the status of the manager
            virtual ManagerStatus GetManagerStatus() const;

        protected:
            //
            ///performs setup actions specific to this manager
            virtual Bool_t SetupManager() = 0;
            //
            ///performs prepare actions specific to this manager
            virtual Bool_t PrepareManager() = 0;
            //
            ///performs shutdown actions specific to this manager
            virtual Bool_t ShutdownManager() = 0;
            //
            ///performs abort actions specific to this manager
            virtual Bool_t AbortManager();
            //
            ///protected method for starting the setup process for this manager and its dependencies
            virtual Bool_t RecursiveManagerSetup();
            ///sets up this manager's dependencies
            virtual Bool_t SetupDependentManagers();
            //
            ///protected method for starting the prepare process for this manager and its dependencies
            virtual Bool_t RecursiveManagerPrepare();
            ///prepares the managers that this manager requires be prepared first
            virtual Bool_t PrepareOrderedDependentManagers();
            ///prepares managers that this manager does not require be prepared first
            virtual Bool_t PrepareUnorderedDependentManagers();
            //
            ///protected method for starting the shutdown process for this manager and its dependencies
            virtual Bool_t RecursiveManagerShutdown();
            ///shuts down this manager's depencencies
            virtual Bool_t ShutdownOrderedDependentManagers();
            virtual Bool_t ShutdownUnorderedDependentManagers();

        public:
            //*************
            //dependency access
            //
            ///add a manager dependency (i.e. this manager depends on aType)
            ///the optional parameter determines whether or not there is a prepare-order dependence
            virtual void AddManager( const string& aType, Bool_t aPrepareBeforeMe = kFALSE );
            //
            ///prints out the list of managers that this manager depends on
            void PrintDependentManagers() const;
            //
            ///returns whether this manager requires manager aType to exist
            Bool_t CheckManagerDependence( const string& aType );
            //
            ///returns whether this manager requires aType to have been prepared first
            Bool_t CheckManagerPrepareOrderDependence( const string& aType );

            //*************
            //depends-on-me access
            //
        public:
            ///add a manager that depends on me
            void AddManagerThatDependsOnMe(KTManager* aManager, Bool_t aShutdownBeforeMe=kFALSE);
            //
            ///prints out the list of managers that depend on this manager
            void PrintDependsOnMeManagers() const;
            //
            ///returns whether aType depends on this manager
            Bool_t CheckIfManagerDependsOnMe(const string& aType);
            //
            ///returns whether this manager will shutdown aType first
            Bool_t CheckManagerShutdownOrderDependence(const string& aType);

        public:
            //*************
            //identification
            //
            ///returns manager's type
            const string& GetTypeName() const;

        protected:
            string fTypeName;

            ManagerStatus fManagerStatus;

            ManagerDependencyMap fMgrDependencyMap;
            ManagerDependsOnMeMap fMgrDependsOnMeMap;

            KTManagerFactory* fManagerFactory;

    };

    inline KTManager::ManagerStatus KTManager::GetManagerStatus() const
    {
        return fManagerStatus;
    }

    inline const string& KTManager::GetTypeName() const
    {
        return fTypeName;
    }

} // end Katydid namespace

#endif // KTMANAGER_H_
