// $Id$

/*
 Class: KTCoreManagereGlobalMessage
 Author: N. Oblath

 For full class documentation: KTCoreManager.hh

 Revision History
 Date         Name          Brief description
 -----------------------------------------------
 29/03/2010   N. Oblath     First version
 01/07/2010   N. Oblath     Removed responsibility for the log to KTExceptionManager
 01/18/2011   N. Oblath     Separated KTCoreManagerSimulation from KTCoreManager


 */

#include "KatydidConfig.hh"

#include "KTCoreManager.hh"

#include "KTTextFile.hh"
#include "KTIOToolbox.hh"
#include "KTCoreMessage.hh"

#include <utility>
using std::pair;
#include <sys/stat.h>
#include <cstdlib>
#include <exception>

namespace Katydid
{

    KTCoreManager::KTCoreManager(Int_t argC, Char_t** argV) :
            KTManager("Core"),
            KTCommandLineUser(),
            fApplicationType(""),
            fApplicationString("")
    {
        fCLHandler->TakeArguments(argC, argV);
        AddManager("IO");
    }

    KTCoreManager::KTCoreManager() :
			KTManager("Core"),
			KTCommandLineUser(),
			fApplicationType(""),
			fApplicationString("")
    {
    	AddManager("IO");
    }

    KTCoreManager::~KTCoreManager()
    {
    }

    //***************

    void KTCoreManager::SetupApplication()
    {
        if (fManagerStatus != KTManager::eInstantiated)
        {
            coremsg < "KTCoreManager(" < GetApplicationType() < ")::SetupApplication";
            coremsg(eError) << "Cannot call SetupApplication unless the core manager is in the Instantiated state (i.e. just after construction)." << eom;
            return;
        }

        coremsg < "KTCoreManager(" < GetApplicationType() < ")::SetupApplication";
        coremsg(eNormal) << "----------------------------------------------------" << ret;
        coremsg << "  Welcome to " << GetApplicationString() << ret;
        coremsg << "  Built with " << fCLHandler->GetPackageString() << ret;
        coremsg << "----------------------------------------------------" << eom;

        coremsg < "KTCoreManager(" < GetApplicationType() < ")::SetupApplication";
        coremsg(eNormal) << "----------------------------------" << ret;
        coremsg << "  Beginning Initialization Phase" << ret;
        coremsg << "----------------------------------" << eom;

        // Simulation Management
        // ---------------------

#ifdef DEBUG_VERBOSE
        coremsg < "KTCoreManager(" < GetApplicationType() < ")::SetupApplication";
        coremsg(eDebug) << "Setting up simulation management" << eom;
#endif

        if (! this->Setup())
        {
            fManagerStatus = KTManager::eBad;
            coremsg < "KTCoreManager::SetupApplication";
            coremsg(eError) << "Something went wrong during the application setup process (Setup)" << eom;
            return;
        }

        // At this point, all of the managers and toolboxes have been created through the Setup functions

#ifdef DEBUG_VERBOSE
        coremsg < "KTCoreManager(" < GetApplicationType() < ")::SetupApplication";
        coremsg(eDebug) << "Reading configuration files" << eom;
#endif

        if (! this->Prepare())
        {
            fManagerStatus = KTManager::eBad;
            coremsg < "KTCoreManager(" < GetApplicationType() < ")::SetupApplication";
            coremsg(eError) << "Something went wrong during the application setup process (Prepare)" << eom;
            return;
        }

#ifdef DEBUG_VERBOSE
        coremsg < "KTCoreManager(" < GetApplicationType() < ")::SetupApplication";
        coremsg(eDebug) << "Initialization phase complete" << eom;
#endif

        return;
    }

    void KTCoreManager::RunApplication()
    {
        if (fManagerStatus != KTManager::ePrepared)
        {
            coremsg < "KTCoreManager(" < GetApplicationType() < ")::RunApplication";
            coremsg(eError) << "Cannot call RunApplication unless the application is in the Prepared state (i.e. just after running SetupApplication)." << eom;
            return;
        }

        coremsg < "KTCoreManager(" < GetApplicationType() < ")::RunApplication";
        coremsg(eNormal) << "-----------------------------" << ret;
        coremsg << "  Beginning Execution Phase" << ret;
        coremsg << "-----------------------------" << eom;

        this->Execute();

#ifdef DEBUG_VERBOSE
        coremsg < "KTCoreManager(" < GetApplicationType() < ")::RunApplication";
        coremsg(eDebug) << "Execution phase complete" << eom;
#endif

        return;
    }

    void KTCoreManager::ShutdownApplication()
    {
        coremsg < "KTCoreManager(" < GetApplicationType() < ")::ShutdownApplication";
        coremsg(eNormal) << "----------------------------" << ret;
        coremsg << "  Beginning Shutdown Phase" << ret;
        coremsg << "----------------------------" << eom;

        if (! this->Shutdown())
        {
            fManagerStatus = KTManager::eBad;
            coremsg < "KTCoreManager(" < GetApplicationType() < ")::ShutdownApplication";
            coremsg(eWarning) << "The CoreManager did not shut down properly!" << eom;
            return;
        }

#ifdef DEBUG_VERBOSE
        coremsg < "KTCoreManager(" < GetApplicationType() < ")::ShutdownApplication";
        coremsg(eDebug) << "Shutdown phase complete" << eom;
#endif

        return;
    }

    //************

    Bool_t KTCoreManager::Setup()
    {
        //std::cout << "KTCoreManager::Setup -- " << this->GetApplicationType() << std::endl;
        if (fManagerStatus != KTManager::eInstantiated)
        {
            coremsg < "KTCoreManager(" < GetApplicationType() < ")::Setup";
            coremsg(eError) << "Cannot call Setup unless the core manager is in the Instantiated state (i.e. just after construction)." << eom;
            return kFALSE;
        }

        if (! this->RecursiveManagerSetup())
        {
            fManagerStatus = KTManager::eBad;
            coremsg < "KTCoreManager(" < GetApplicationType() < ")::Setup";
            coremsg(eWarning) << "Something went wrong during the CoreManager's Setup" << eom;
            return kFALSE;
        }

        fManagerStatus = KTManager::eSetUp;
        return kTRUE;
    }

    Bool_t KTCoreManager::Prepare()
    {
        //std::cout << "KTCoreManager::Prepare -- " << this->GetApplicationType() << std::endl;
        if (fManagerStatus != KTManager::eSetUp)
        {
            coremsg < "KTCoreManager(" < GetApplicationType() < ")::Prepare";
            coremsg(eError) << "Cannot call Prepare unless the application is in the eSetup state (i.e. just after Setup is run)." << eom;
            return kFALSE;
        }

        // Initial Configuration
        // ---------------------

        // Parse the command line options that remain after the initial parsing
        fCLHandler->ProcessCommandLine();

        // All other Configuration files
        // -----------------------------

        // All configuration files (besides the initial and final ones) are read in during the RecursivePrepare process

        if (! this->RecursiveManagerPrepare())
        {
            fManagerStatus = KTManager::eBad;
            coremsg < "KTCoreManager(" < GetApplicationType() < ")::Prepare";
            coremsg(eWarning) << "Something went wrong during the CoreManager's recursive Prepare" << eom;
            return kFALSE;
        }

#ifdef DEBUG_VERBOSE
        coremsg < "KTCoreManager(" < GetApplicationType() < ")::Prepare";
        coremsg(eDebug) << "Setup complete" << eom;
#endif

        fManagerStatus = KTManager::ePrepared;
        return kTRUE;
    }

    Bool_t KTCoreManager::Shutdown()
    {
        //std::cout << "KTCoreManager::Shutdown -- " << this->GetApplicationType() << std::endl;
        if (!this->RecursiveManagerShutdown())
        {
            fManagerStatus = KTManager::eBad;
            coremsg < "KTCoreManager(" < GetApplicationType() < ")::Shutdown";
            coremsg(eWarning) << "Something went wrong during the CoreManager's Shutdown" << eom;
            return kFALSE;
        }
        return kTRUE;
    }

    Bool_t KTCoreManager::Abort()
    {
        if (fManagerStatus != eAborted && fManagerStatus != eShutDown && fManagerStatus != eBad)
            this->AbortManager();
        fManagerStatus = eAborted;
        return kTRUE;
    }

    //************

    Bool_t KTCoreManager::RecursiveManagerShutdown()
    {
        //std::cout << "KTCoreManager::RecursiveManagerShutdown -- " << this->GetApplicationType() << std::endl;
        fManagerStatus = KTManager::eShuttingDown;

        if (! this->ShutdownDependentManagers())
        {
            coremsg < "KTCoreManager(" < GetApplicationType() < ")::RecursiveManagerShutdown";
            coremsg(eWarning) << "There was a problem shutting down the dependent managers of core manager <" << this->GetApplicationType() << ">!" << eom;
            fManagerStatus = KTManager::eBad;
            return kFALSE;
        }

        if (! this->ShutdownManager())
        {
            coremsg < "KTCoreManager(" < GetApplicationType() < ")::RecursiveManagerShutdown";
            coremsg(eWarning) << "There was a problem shutting down the manager aspect of core manager <" << this->GetApplicationType() << ">!" << eom;
            fManagerStatus = KTManager::eBad;
            return kFALSE;
        }

        fManagerStatus = KTManager::eShutDown;

        return kTRUE;
    }

    Bool_t KTCoreManager::SetupDependentManagers()
    {
        //std::cout << "SetupDependentManagers for " << this->GetTypeName() << std::endl;
        Bool_t tDepManagersSetUp = kTRUE;
        for( MgrDepMapIt iter = fMgrDependencyMap.begin(); iter != fMgrDependencyMap.end(); iter++ )
        {
            //std::cout << "   calling setup for " << iter->first << std::endl;
            iter->second.Manager = KTManagerTable::GetInstance()->GetManager( iter->first );

            if( iter->second.Manager == NULL)
            {
                coremsg < "KTManager(" < GetTypeName() < ")::SetupDependentManager";
                coremsg( eWarning ) << "Could not get manager <" << iter->first << ">!" << eom;
                tDepManagersSetUp = kFALSE;
                continue;
            }

            // at this point the dependent manager is known to exist and its pointer has been obtained
            if( !iter->second.Manager->Setup() )
            {
                coremsg < "KTManager(" < GetTypeName() < ")::SetupDependentManager";
                coremsg( eWarning ) << "Manager <" << iter->first << "> failed to be set up correctly!" << eom;
                tDepManagersSetUp = kFALSE;
                continue;
            }
            iter->second.Manager->AddManagerThatDependsOnMe(this,iter->second.obeyOrdering);
        }
        return tDepManagersSetUp;
    }

    Bool_t KTCoreManager::ShutdownDependentManagers()
    {
        //std::cout << "ShutdownDependentManagers for " << this->GetApplicationType() << std::endl;
        Bool_t tDepManagersShutdown = kTRUE;
        for (MgrDepMapIt iter = fMgrDependencyMap.begin(); iter != fMgrDependencyMap.end(); iter++)
        {
            //std::cout << "   calling shutdown for " << iter->first << std::endl;
            if (! iter->second.Manager->Shutdown())
            {
                coremsg < "KTCoreManager(" < GetApplicationType() < ")::ShutdownDependentManager";
                coremsg(eWarning) << "Manager <" << iter->first << "> failed to shutdown correctly!" << eom;
                tDepManagersShutdown = kFALSE;
                continue;
            }
            fMgrDependencyMap.erase(iter);
        }
        return tDepManagersShutdown;
    }

    //*************

    void KTCoreManager::AddManager(const string& aType)
    {
        this->AddManager(aType, kTRUE);
        return;
    }

    void KTCoreManager::AddManager(const string& aType, Bool_t /*b*/)
    {
        MgrDepMapCIt iter = fMgrDependencyMap.find(aType);
        if (iter != fMgrDependencyMap.end())
        {
            coremsg < "KTCoreManager(" < GetApplicationType() < ")::AddManagerDependence";
            coremsg(eWarning) << "Attempting to add manager <" << aType << "> as a dependency of core manager <" << fApplicationType << ">," << ret;
            coremsg << "but such a dependency already exists." << eom;
            return;
        }
        DependentManagerInfo tNewDepInfo;
        tNewDepInfo.Manager = NULL;
        tNewDepInfo.obeyOrdering = kTRUE;
        fMgrDependencyMap.insert(pair< string, DependentManagerInfo >(aType, tNewDepInfo));
        return;
    }

    //*************

    void KTCoreManager::AddCommandLineOptions()
    {
        return;
    }

    void KTCoreManager::UseParsedCommandLineImmediately()
    {
        return;
    }

} // end Katydid namespace
