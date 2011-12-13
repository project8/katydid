// $Id$

/*
 Class: KTManager
 Author: N. Oblath

 For full class documentation: KTManager.hh

 Revision History
 Date         Name          Brief description
 -----------------------------------------------
 29/03/2010   N. Oblath     First version
 13/07/2010   D. Furse      Boost pointer maps are out.
 31/01/2011   N. Oblath     Removed manager "Type." Use manager "Key" instead.
 14/04/2011   N. Oblath     Split from KTManager

 */

#include "KTManager.hh"

#include "KTManagerFactory.hh"
#include "KTManagerTable.hh"

#include "KTCoreMessage.hh"

#include <utility>
using std::pair;

namespace Katydid
{

    //*************
    KTManager::KTManager( const string& aType ) :
        fTypeName( aType ),
        fManagerStatus( eInstantiated ),
        fMgrDependencyMap(),
        fMgrDependsOnMeMap(),
        fManagerFactory(NULL)
    {
    }

    KTManager::KTManager() :
    		fTypeName(),
    		fManagerStatus(eBad),
    		fMgrDependencyMap(),
    		fMgrDependsOnMeMap()
    {
    }

    //*************

    KTManager::~KTManager()
    {
    }

    //*************

    Bool_t KTManager::Setup()
    {
        //std::cout << "KTManager::Setup -- " << this->GetTypeName() << std::endl;
        if( !this->RecursiveManagerSetup() )
        {
            coremsg < "KTManager(" < GetTypeName() < ")::Setup";
            coremsg( eError ) << "Manager <" << this->GetTypeName() << "> was not set up properly!" << eom;
            return kFALSE;
        }
        return kTRUE;
    }

    //*************

    Bool_t KTManager::Prepare()
    {
        //std::cout << "KTManager::Prepare -- " << this->GetTypeName() << std::endl;
        if( !this->RecursiveManagerPrepare() )
        {
            coremsg < "KTManager(" < GetTypeName() < ")::Prepare";
            coremsg( eError ) << "Manager <" << this->GetTypeName() << "> did not prepare properly!" << eom;
            return kFALSE;
        }
        return kTRUE;
    }

    //*************

    Bool_t KTManager::Shutdown()
    {
        //std::cout << "KTManager::Shutdown -- " << this->GetTypeName() << std::endl;
        if (fManagerFactory != NULL) fManagerFactory->fStatus = KTManagerFactory::eManagerInactive;
        if (fManagerStatus == eBad) return kFALSE;
        if (! this->RecursiveManagerShutdown())
        {
            coremsg < "KTManager(" < GetTypeName() < ")::Shutdown";
            coremsg( eWarning ) << "Manager <" << this->GetTypeName() << "> did not shutdown properly!" << eom;
            return kFALSE;
        }
        return kTRUE;
    }

    //*************

    Bool_t KTManager::Abort()
    {
        if (fManagerFactory != NULL) fManagerFactory->fStatus = KTManagerFactory::eManagerInactive;
        if (fManagerStatus != eAborted && fManagerStatus != eShutDown && fManagerStatus != eBad)
            this->AbortManager();
        KTManagerTable::GetInstance()->AbortManagers();
        fManagerStatus = eAborted;
        return kTRUE;
    }

    // temporary, while managers don't have their own AbortManager functions
    Bool_t KTManager::AbortManager()
    {
        return this->ShutdownManager();
    }

    //*************

    Bool_t KTManager::RecursiveManagerSetup()
    {
        //std::cout << "KTManager::RecursiveManagerSetup -- " << this->GetTypeName() << std::endl;
        if( fManagerStatus == eSetUp ) return kTRUE;
        if( fManagerStatus == eSettingUp ) return kTRUE;
        if( fManagerStatus == ePreparing ) return kTRUE;
        if( fManagerStatus == ePrepared ) return kTRUE;
        if( fManagerStatus != eInstantiated ) return kFALSE;

        fManagerStatus = eSettingUp;

        if( !this->SetupDependentManagers() )
        {
            coremsg < "KTManager(" < GetTypeName() < ")::RecursiveManagerSetup";
            coremsg( eWarning ) << "There was a problem setting up the dependencies of <" << this->GetTypeName() << ">!" << eom;
            fManagerStatus = eBad;
            return kFALSE;
        }

        if( !this->SetupManager() )
        {
            coremsg < "KTManager(" < GetTypeName() < ")::RecursiveManagerSetup";
            coremsg( eWarning ) << "There was a problem setting up manager <" << this->GetTypeName() << ">!" << eom;
            fManagerStatus = eBad;
            return kFALSE;
        }

        fManagerStatus = eSetUp;
        return kTRUE;
    }

    //*************

    Bool_t KTManager::RecursiveManagerPrepare()
    {
        //std::cout << "KTManager::RecursiveManagerPrepare -- " << this->GetTypeName() << std::endl;
        if( fManagerStatus == ePreparing )
        {
            coremsg < "KTManager(" < GetTypeName() < ")::RecursiveManagerPrepare";
            coremsg( eWarning ) << "A circular dependence has been found while preparing managers!" << ret;
            coremsg << "The circle is completed with manager <" << this->GetTypeName() << ">." << eom;
            fManagerStatus = eBad;
            return kFALSE;
        }

        if( fManagerStatus == ePrepared ) return kTRUE;
        if( fManagerStatus != eSetUp ) return kFALSE;

        fManagerStatus = ePreparing;

        if( !this->PrepareOrderedDependentManagers() )
        {
            coremsg < "KTManager(" < GetTypeName() < ")::RecursiveManagerPrepare";
            coremsg( eWarning ) << "There was a problem preparing the prepare-order-dependent managers of <" << this->GetTypeName() << ">!" << eom;
            fManagerStatus = eBad;
            return kFALSE;
        }

        if( !this->PrepareManager() )
        {
            coremsg < "KTManager(" < GetTypeName() < ")::RecursiveManagerPrepare";
            coremsg( eWarning ) << "There was a problem preparing manager <" << this->GetTypeName() << ">!" << eom;
            fManagerStatus = eBad;
            return kFALSE;
        }

        fManagerStatus = ePrepared;

        if( !this->PrepareUnorderedDependentManagers() )
        {
            coremsg < "KTManager(" < GetTypeName() < ")::RecursiveManagerPrepare";
            coremsg( eWarning ) << "There was a problem preparing the non-prepare-order-dependent managers of <" << this->GetTypeName() << ">!" << eom;
            fManagerStatus = eBad;
            return kFALSE;
        }

        return kTRUE;
    }

    //*************

    Bool_t KTManager::RecursiveManagerShutdown()
    {
        //std::cout << "KTManager::RecursiveManagerShutdown -- " << this->GetTypeName() << std::endl;
        if( fManagerStatus == eInstantiated )
        {
            fManagerStatus = eShutDown;
            return kTRUE;
        }
        if (fManagerStatus == eShuttingDown)
        {
            coremsg < "KTManager(" < GetTypeName() < ")::RecursiveManagerShutdown";
            coremsg(eWarning) << "A circular dependence has been found while shutting down managers!" << ret;
            coremsg << "The circle is completed with manager <" << this->GetTypeName() << ">." << eom;
            fManagerStatus = eBad;
            return kFALSE;
        }

        if (fManagerStatus == eShutDown) return kTRUE;

        fManagerStatus = eShuttingDown;

        if (! this->ShutdownOrderedDependentManagers())
        {
            coremsg < "KTManager(" < GetTypeName() < ")::RecursiveManagerShutdown";
            coremsg(eWarning) << "There was a problem shutting down the ordered dependent managers of <" << this->GetTypeName() << ">!" << eom;
            fManagerStatus = eBad;
            return kFALSE;
        }

        if (! this->ShutdownManager())
        {
            coremsg < "KTManager(" < GetTypeName() < ")::RecursiveManagerShutdown";
            coremsg( eWarning ) << "There was a problem shutting down <" << this->GetTypeName() << ">!" << eom;
            fManagerStatus = eBad;
            return kFALSE;
        }

        if (! this->ShutdownUnorderedDependentManagers())
        {
            coremsg < "KTManager(" < GetTypeName() < ")::RecursiveManagerShutdown";
            coremsg(eWarning) << "There was a problem shutting down the unordered dependent managers of <" << this->GetTypeName() << ">!" << eom;
            fManagerStatus = eBad;
            return kFALSE;
        }

        fManagerStatus = eShutDown;
        return kTRUE;
    }

    //*************

    Bool_t KTManager::SetupDependentManagers()
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
            iter->second.Manager->AddManagerThatDependsOnMe(this, iter->second.obeyOrdering);
        }
        return tDepManagersSetUp;
    }

    Bool_t KTManager::PrepareOrderedDependentManagers()
    {
        //std::cout << "PrepareOrderedDependentManagers for " << this->GetTypeName() << std::endl;
        Bool_t tDepManagersPrepared = kTRUE;
        for( MgrDepMapCIt iter = fMgrDependencyMap.begin(); iter != fMgrDependencyMap.end(); iter++ )
        {
            if (! iter->second.obeyOrdering) continue;
            //std::cout << "   calling prepare for " << iter->first << std::endl;
            if( !iter->second.Manager->Prepare() )
            {
                coremsg < "KTManager(" < GetTypeName() < ")::PrepareOrderedDependentManager";
                coremsg( eWarning ) << "Manager <" << iter->first << "> failed to be prepared correctly!" << eom;
                tDepManagersPrepared = kFALSE;
                continue;
            }
        }
        return tDepManagersPrepared;
    }

    Bool_t KTManager::PrepareUnorderedDependentManagers()
    {
        //std::cout << "PrepareUnorderedDependentManagers for " << this->GetTypeName() << std::endl;
        Bool_t tDepManagersPrepared = kTRUE;
        for( MgrDepMapCIt iter = fMgrDependencyMap.begin(); iter != fMgrDependencyMap.end(); iter++ )
        {
            //std::cout << "   calling prepare for " << iter->first << std::endl;
            if( !iter->second.Manager->Prepare() )
            {
                coremsg < "KTManager(" < GetTypeName() < ")::PrepareUnorderedDependentManager";
                coremsg( eWarning ) << "Manager <" << iter->first << "> failed to be prepared correctly!" << eom;
                tDepManagersPrepared = kFALSE;
                continue;
            }
        }
        return tDepManagersPrepared;
    }

    Bool_t KTManager::ShutdownOrderedDependentManagers()
    {
        //std::cout << "ShutdownOrderedDependentManagers for " << this->GetTypeName() << std::endl;
        Bool_t tDepManagersShutdown = kTRUE;
        for (MgrDepOnMeMapIt iter = fMgrDependsOnMeMap.begin(); iter != fMgrDependsOnMeMap.end(); iter++)
        {
            if (iter->second.Manager->GetTypeName() == string("Core")) continue;
            if (iter->second.obeyOrdering == kFALSE) continue;
            //std::cout << "   calling shutdown for " << iter->first << std::endl;
            if (! iter->second.Manager->Shutdown())
            {
                coremsg < "KTManager(" < GetTypeName() < ")::ShutdownOrderedDependentManager";
                coremsg(eWarning) << "Manager <" << iter->first << "> failed to shutdown correctly!" << eom;
                tDepManagersShutdown = kFALSE;
                continue;
            }
            fMgrDependsOnMeMap.erase(iter);
        }
        return tDepManagersShutdown;
    }

    Bool_t KTManager::ShutdownUnorderedDependentManagers()
    {
        //std::cout << "ShutdownUnorderedDependentManagers for " << this->GetTypeName() << std::endl;
        Bool_t tDepManagersShutdown = kTRUE;
        for (MgrDepOnMeMapIt iter = fMgrDependsOnMeMap.begin(); iter != fMgrDependsOnMeMap.end(); iter++)
        {
            if (iter->second.Manager->GetTypeName() == string("Core")) continue;
            //std::cout << "   calling shutdown for " << iter->first << std::endl;
            if (! iter->second.Manager->Shutdown())
            {
                coremsg < "KTManager(" < GetTypeName() < ")::ShutdownUnorderedDependentManager";
                coremsg(eWarning) << "Manager <" << iter->first << "> failed to shutdown correctly!" << eom;
                tDepManagersShutdown = kFALSE;
                continue;
            }
            fMgrDependsOnMeMap.erase(iter);
        }
        return tDepManagersShutdown;
    }

    //*************
    void KTManager::AddManager( const string& aType, Bool_t aPrepareBeforeMe )
    {
        MgrDepMapCIt iter = fMgrDependencyMap.find( aType );
        if( iter != fMgrDependencyMap.end() )
        {
            coremsg < "KTManager(" < GetTypeName() < ")::AddManagerDependence";
            coremsg( eWarning ) << "Attempting to add manager <" << aType << "> as a dependency of <" << this->GetTypeName() << ">," << ret;
            coremsg << "but such a dependency already exists." << eom;
            return;
        }
        DependentManagerInfo tNewDepInfo;
        tNewDepInfo.Manager = NULL;
        tNewDepInfo.obeyOrdering = aPrepareBeforeMe;
        fMgrDependencyMap.insert( pair< string, DependentManagerInfo >( aType, tNewDepInfo ) );
        return;
    }

    //*************

    void KTManager::PrintDependentManagers() const
    {
        coremsg < "KTManager(" < GetTypeName() < ")::PrintDependentManagers";
        for( MgrDepMapCIt iter = fMgrDependencyMap.begin(); iter != fMgrDependencyMap.end(); iter++ )
        {
            coremsg(eNormal) << iter->first << "  " << iter->second.obeyOrdering << ret;
        }
        coremsg << eom;
        return;
    }

    //*************

    Bool_t KTManager::CheckManagerDependence( const string& aType )
    {
        MgrDepMapCIt iter = fMgrDependencyMap.find( aType );
        if( iter == fMgrDependencyMap.end() ) return kFALSE;
        return kTRUE;
    }

    //*************

    Bool_t KTManager::CheckManagerPrepareOrderDependence( const string& aType )
    {
        MgrDepMapCIt iter = fMgrDependencyMap.find( aType );
        if( iter == fMgrDependencyMap.end() ) return kFALSE;
        return iter->second.obeyOrdering;
    }

    //*************

    void KTManager::AddManagerThatDependsOnMe(KTManager* aManager, Bool_t aShutdownBeforeMe)
    {
        // this is just for debugging; it's a warning so the yellow text sticks out
        //coremsg < "KTManager::AddManagerThatDependsOnMe";
        //coremsg(eWarning) << "Request to add manager <" << aManager->GetTypeName() << "> as a depending on <" << this->GetTypeName() << ">; flag: " << aShutdownBeforeMe << eom;

        string tType = aManager->GetTypeName();
        MgrDepOnMeMapCIt iter = fMgrDependsOnMeMap.find(tType);
        if (iter != fMgrDependsOnMeMap.end())
        {
            coremsg < "KTManager(" < GetTypeName() < ")::AddManagerThatDependsOnMe";
            coremsg(eWarning) << "Attempting to add manager <" << tType << "> as depending on <" << this->GetTypeName() << ">," << ret;
            coremsg << "but such a dependency already exists." << eom;
            return;
        }
        DependentManagerInfo tNewDepInfo;
        tNewDepInfo.Manager = aManager;
        tNewDepInfo.obeyOrdering = aShutdownBeforeMe;
        fMgrDependsOnMeMap.insert(pair< string, DependentManagerInfo >(tType, tNewDepInfo));
        return;
    }

    //*************

    void KTManager::PrintDependsOnMeManagers() const
    {
        coremsg < "KTManager(" < GetTypeName() < ")::PrintDependsOnMeManagers";
        for (MgrDepOnMeMapCIt iter = fMgrDependsOnMeMap.begin(); iter != fMgrDependsOnMeMap.end(); iter++)
        {
            coremsg(eNormal) << iter->first << "  " << iter->second.obeyOrdering << ret;
        }
        coremsg << eom;
        return;
    }

    //*************

    Bool_t KTManager::CheckIfManagerDependsOnMe(const string& aType)
    {
        MgrDepOnMeMapCIt iter = fMgrDependsOnMeMap.find(aType);
        if (iter == fMgrDependsOnMeMap.end()) return kFALSE;
        return kTRUE;
    }

    //*************

    Bool_t KTManager::CheckManagerShutdownOrderDependence(const string& aType)
    {
        MgrDepOnMeMapCIt iter = fMgrDependsOnMeMap.find(aType);
        if (iter == fMgrDependsOnMeMap.end()) return kFALSE;
        return iter->second.obeyOrdering;
    }

    //*************

} // end Katydid namespace
