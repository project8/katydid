// $Id$

/*
 Class: KTCoreManagerTable
 Author: N. Oblath

 For full class documentation: KTCoreManagerTable.hh

 Revision History
 Date         Name          Brief description
 -----------------------------------------------
 08/04/2010   N. Oblath     First version

 */

#include "KTCoreManagerTable.hh"
#include "KTManagerTable.hh"

//#include "KTCommandLineHandler.hh"

namespace Katydid
{

    KTCoreManagerTable* KTCoreManagerTable::fInstance = NULL;
    KTDestroyer< KTCoreManagerTable > KTCoreManagerTable::fCoreMgrTableDestroyer;

    KTCoreManagerTable* KTCoreManagerTable::GetInstance()
    {
        if (fInstance == NULL)
        {
            fInstance = new KTCoreManagerTable();
            fCoreMgrTableDestroyer.SetDoomed(fInstance);
        }
        return fInstance;
    }

    KTCoreManagerTable::KTCoreManagerTable() :
            fCoreManagerFactoryMap(), fSpecifiedFactory(NULL), fStatus(eUnspecified)
    {
#ifdef DEBUG_VERBOSE
        fwmsg < "KTCoreManagerTable::KTCoreManagerTable";
        fwmsg( eDebug ) << "CoreManager table has been created." << eom;
#endif
    }

    KTCoreManagerTable::~KTCoreManagerTable()
    {
    }

    KTCoreManager* KTCoreManagerTable::CreateCoreManager(const string& aAppType, Int_t aNArgs, Char_t** anArgList)
    {
        if (fStatus == eSpecified)
        {
            fwmsg < "KTCoreManagerTable::CreateCoreManager";
            fwmsg(eError) << "A core manager has already been created; only one can exist." << eom;
            return NULL;
        }

        TBFactoryMapCIt tIter = fCoreManagerFactoryMap.find(aAppType);
        if (tIter != fCoreManagerFactoryMap.end())
        {
            tIter->second->CreateCoreManager(aNArgs, anArgList);
            KTCoreManager* tCM = tIter->second->GetCoreManager();
            if (tCM != NULL)
            {
                fStatus = eSpecified;
                fSpecifiedFactory = tIter->second;
#ifdef DEBUG_VERBOSE
                fwmsg < "KTCoreManagerTable::CreateCoreManager";
                fwmsg(eDebug) << "Core manager <" << aAppType << "> has been created" << eom;
#endif
                /*
                KTCommandLineHandler* tCLHandler = KTCommandLineHandler::GetInstance();
                if (tCLHandler->GetPrintHelpMessageFlag())
                {
                    tCM->Setup();
                    tCLHandler->PrintHelpMessageAndExit(tCM->GetApplicationType());
                }
                if (tCLHandler->GetPrintVersionFlag())
                {
                    tCLHandler->PrintVersionMessageAndExit(tCM->GetApplicationType(), tCM->GetApplicationString());
                }
                */
            }
            else
            {
                fwmsg < "KTCoreManagerTable::CreateCoreManager";
                fwmsg(eError) << "Core manager <" << aAppType << "> was not created!" << eom;
            }
            return tCM;
        }

        fwmsg < "KTCoreManagerTable::CreateCoreManager";
        fwmsg(eError) << "There is no core manager factory with type <" << aAppType << "> registered" << eom;
        return NULL;
    }

    Bool_t KTCoreManagerTable::CoreManagerExists() const
    {
        if (fStatus == eSpecified) return kTRUE;
        return kFALSE;
    }

    KTCoreManager* KTCoreManagerTable::GetCoreManager() const
    {
        if (fStatus != eSpecified)
        {
            fwmsg < "KTCoreManagerTable::GetCoreManager";
            fwmsg(eWarning) << "Core manager has not yet been created!" << eom;
            return NULL;
        }
        return fSpecifiedFactory->GetCoreManager();
    }

    Bool_t KTCoreManagerTable::ShutdownCoreManager()
    {
        if (fStatus == eShuttingDown) return kTRUE;
        if (fStatus == eUnspecified)
        {
            fwmsg < "KTCoreManagerTable::ShutdownCoreManager";
            fwmsg(eWarning) << "Core manager has not yet been created!" << eom;
            return kFALSE;
        }
        if (fStatus == eAborting) return kFALSE;
        fStatus = eShuttingDown;

#ifdef DEBUG_VERBOSE
        fwmsg < "KTCoreManagerTable::ShutdownCoreManager";
        fwmsg(eDebug) << "Initiating global shutdown via the CoreManager" << eom;
#endif

        fSpecifiedFactory->GetCoreManager()->ShutdownApplication();
        return kTRUE;
    }

    Bool_t KTCoreManagerTable::AbortCoreManager()
    {
        if (fStatus == eAborting) return kTRUE;
        if (fStatus == eUnspecified) return kTRUE;
        fStatus = eAborting;

        KTCoreManager* tCoreManager = fSpecifiedFactory->GetCoreManager();
        if (tCoreManager == NULL) return kTRUE;

        if (tCoreManager->GetManagerStatus() == KTCoreManager::eAborted ||
                tCoreManager->GetManagerStatus() == KTCoreManager::eShutDown) return kTRUE;

#ifdef DEBUG_VERBOSE
        fwmsg < "KTCoreManagerTable::AbortCoreManager";
        fwmsg(eDebug) << "Initiating abort of the core manager" << eom;
#endif

        if (! tCoreManager->Abort()) return kFALSE;
        if (! KTManagerTable::GetInstance()->AbortManagers()) return kFALSE;
        return kTRUE;
    }

    Bool_t KTCoreManagerTable::RegisterCoreManagerFactory(const string& aAppType, KTCoreManagerFactory* aFactory)
    {
        TBFactoryMapIt tIter = fCoreManagerFactoryMap.find(aAppType);
        if (tIter != fCoreManagerFactoryMap.end())
        {
            fwmsg < "KTCoreManagerInstanceMap::RegisterCoreManagerFactory";
            fwmsg(eWarning) << "A core manager factory with type <" << aAppType << "> has already been registered!" << ret;
            fwmsg << "This factory was not added." << eom;
            return kFALSE;
        }

        fCoreManagerFactoryMap.insert(TBFactoryMapEntry(aAppType, aFactory));

        return kTRUE;
    }

    void KTCoreManagerTable::RemoveCoreManagerFactory(const string& aAppType)
    {
        TBFactoryMapIt tIter = fCoreManagerFactoryMap.find(aAppType);
        if (tIter != fCoreManagerFactoryMap.end())
        {
            if (tIter->second == fSpecifiedFactory)
            {
                fSpecifiedFactory = NULL;
                fStatus = eUnspecified;
            }
            fCoreManagerFactoryMap.erase(tIter);
            return;
        }
        fwmsg < "KTCoreManagerTable::RemoveCoreManagerFactory";
        fwmsg(eWarning) << "No core manager with type <" << aAppType << "> has been registered in the table!" << eom;
        return;
    }

    KTCoreManagerTable::TableStatus KTCoreManagerTable::GetStatus() const
    {
        return fStatus;
    }

} // end Katydid namespace
