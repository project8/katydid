// $Id$

/*
 Class: KTCoreManagerFactory
 Author: N. Oblath
 
 For full class documentation: KTCoreManagerFactory.hh
 
 Revision History
 Date         Name          Brief description
 -----------------------------------------------
 08/04/2010   N. Oblath     First version
 31/01/2011   N. Oblath     Made this the base class for KTManagerFactoryPrototype
 
 */

#include "KTCoreManagerFactory.hh"
#include "KTCoreManagerTable.hh"

#include "KTFrameworkMessage.hh"

namespace Katydid {
    
    KTCoreManagerFactory::KTCoreManagerFactory(const string& aAppType) :
            fInstance(NULL),
            fApplicationTypeName(aAppType),
            fStatus(eBad)
    {
        if (KTCoreManagerTable::GetInstance()->RegisterCoreManagerFactory(aAppType, this))
        {
#ifdef DEBUG_VERBOSE
            fwmsg < "KTCoreManagerFactory(" < fApplicationTypeName < ")::constructor";
            fwmsg(eDebug) << "Successfully registered a factory for core manager <" << aAppType << ">" << eom;
#endif
            fStatus = eGood;
        }
    }

    KTCoreManagerFactory::KTCoreManagerFactory() :
            fInstance(NULL),
            fApplicationTypeName(""),
            fStatus(eBad)
    {
        // private default constructor
    }
    
    KTCoreManagerFactory::~KTCoreManagerFactory()
    {
#ifdef DEBUG_VERBOSE
        fwmsg < "KTCoreManagerFactory(" < fApplicationTypeName < ")::~KTCoreManagerFactory";
        fwmsg(eDebug) << "Removing factory from the CoreManagerTable, and deleting the core manager" << eom;
#endif
        KTCoreManagerTable::GetInstance()->RemoveCoreManagerFactory(fApplicationTypeName);
        delete fInstance;
    }
    
    KTCoreManager* KTCoreManagerFactory::GetCoreManager()
    {
        if (fStatus == eBad)
        {
            fwmsg < "KTCoreManagerFactory::GetCoreManager";
            fwmsg(eWarning) << "Status of core manager factory <" << fApplicationTypeName << "> is bad!" << eom;
            return NULL;
        }
        if (fInstance == NULL)
        {
            this->CreateCoreManager();
#ifdef DEBUG_VERBOSE
            fwmsg < "KTCoreManagerFactory::GetCoreManager";
            fwmsg(eDebug) << "Creating core manager <" << fApplicationTypeName << ">: " << fInstance << eom;
#endif
        }
        return fInstance;
    }

    const string& KTCoreManagerFactory::GetApplicationTypeName() const
    {
        return fApplicationTypeName;
    }

    KTCoreManagerFactory::FactoryStatus KTCoreManagerFactory::GetStatus() const
    {
        return fStatus;
    }

} // end Katydid namespace
