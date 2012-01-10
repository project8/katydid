// $Id$

/*
 Class: KTManagerTable
 Author: N. Oblath
 
 For full class documentation: KTManagerTable.hh
 
 Revision History
 Date         Name          Brief description
 -----------------------------------------------
 08/04/2010   N. Oblath     First version
 
 */

#include "KTManagerTable.hh"
#include "KTCoreManagerTable.hh"

namespace Katydid
{

    KTManagerTable* KTManagerTable::fInstance = NULL;
    KTDestroyer< KTManagerTable > KTManagerTable::fMgrTableDestroyer;

    KTManagerTable* KTManagerTable::GetInstance()
    {
        if (fInstance == NULL)
        {
            fInstance = new KTManagerTable();
            fMgrTableDestroyer.SetDoomed(fInstance);
        }
        return fInstance;
    }

    KTManagerTable::KTManagerTable() :
            fManagerFactoryMap(),
            fStatus(eGood)
    {
#ifdef DEBUG_VERBOSE
        fwmsg < "KTManagerTable::KTManagerTable";
        fwmsg(eDebug) << "Manager table has been created."<< eom;
#endif
    }

    KTManagerTable::~KTManagerTable()
    {
    }

    KTManager* KTManagerTable::GetManager(const string& aType) const
    {
        if (fStatus == eBad) return NULL;
        MgrFactoryMapCIt tIter = fManagerFactoryMap.find(aType);
        if (tIter != fManagerFactoryMap.end())
        {
            return tIter->second->GetManager();
        }
        fwmsg < "KTManagerTable::GetManager";
        fwmsg(eWarning) << "There is no manager factory with type <"<< aType <<"> registered" << eom;
        return NULL;
    }

    Bool_t KTManagerTable::ShutdownManagers()
    {
        if (fStatus == eShuttingDown) return kTRUE;
        if (fStatus != eGood) return kFALSE;
        fStatus = eShuttingDown;

        Bool_t tSuccess = kTRUE;
#ifdef DEBUG_VERBOSE
        fwmsg < "KTManagerTable::ShutdownManagers";
        fwmsg(eDebug) << "Shutting down all remaining active managers" << eom;
#endif
        for (MgrFactoryMapIt tIter=fManagerFactoryMap.begin(); tIter != fManagerFactoryMap.end(); tIter++)
        {
            if (tIter->second->GetStatus() == KTManagerFactory::eManagerExists)
            {
#ifdef DEBUG_VERBOSE
                fwmsg < "KTManagerTable::ShutdownManagers";
                fwmsg(eDebug) << "Shutting down manager <" << tIter->first << ">" << eom;
#endif
                tSuccess = tSuccess && tIter->second->GetManager()->Shutdown();
            }
        }
        return tSuccess;
    }

    Bool_t KTManagerTable::AbortManagers()
    {
        if (fStatus == eAborting) return kTRUE;
        if (fStatus == eBad) return kFALSE;
        fStatus = eAborting;

        Bool_t tSuccess = kTRUE;
#ifdef DEBUG_VERBOSE
        fwmsg < "KTManagerTable::AbortManagers";
        fwmsg(eDebug) << "Aborting all remaining active managers" << eom;
#endif
        for (MgrFactoryMapIt tIter=fManagerFactoryMap.begin(); tIter != fManagerFactoryMap.end(); tIter++)
        {
            if (tIter->second->GetStatus() == KTManagerFactory::eManagerExists)
            {
#ifdef DEBUG_VERBOSE
                fwmsg < "KTManagerTable::AbortManagers";
                fwmsg(eDebug) << "Aborting manager <" << tIter->first << ">" << eom;
#endif
                tSuccess = tSuccess && tIter->second->GetManager()->Abort();
            }
        }
        tSuccess = tSuccess && KTCoreManagerTable::GetInstance()->AbortCoreManager();
        return tSuccess;
    }

    Bool_t KTManagerTable::RegisterManagerFactory(const string& aType, KTManagerFactory* aFactory)
    {
        if (fStatus != eGood) return kFALSE;

        MgrFactoryMapIt tIter = fManagerFactoryMap.find(aType);
        if (tIter != fManagerFactoryMap.end())
        {
            fwmsg < "KTManagerInstanceMap::RegisterManagerFactory";
            fwmsg(eWarning) << "A manager factory with type <" << aType << "> has already been registered!" << ret;
            fwmsg << "This factory was not added." << eom;
            return kFALSE;
        }

        fManagerFactoryMap.insert(MgrFactoryMapEntry(aType, aFactory));

        return kTRUE;
    }

    void KTManagerTable::RemoveManagerFactory(const string& aType)
    {
        MgrFactoryMapIt tIter = fManagerFactoryMap.find(aType);
        if (tIter != fManagerFactoryMap.end())
        {
            fManagerFactoryMap.erase(tIter);
            return;
        }
        fwmsg < "KTManagerTable::RemoveManagerFactory";
        fwmsg(eWarning) << "No manager with type <" << aType << "> has been registered in the table!" << eom;
        return;
    }

} // end Katydid namespace
