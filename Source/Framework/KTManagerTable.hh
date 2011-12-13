// $Id$

#ifndef KTMANAGER_TABLE_H
#define KTMANAGER_TABLE_H

/**
 @file
 @brief contains KTManagerTable
 @details
 
 <b>Revision History:</b>
 \verbatim
 Date         Name          Brief description
 -----------------------------------------------
 08/04/2010   N. Oblath     First version
 \endverbatim
 
 */

/*!
 * @class Katydid::KTManagerTable
 @author N. Oblath
 
 @brief Singleton lookup table for manager instances
 
 @details
 <b>Detailed Description:</b><br>
 
 */


#include "KTManagerFactory.hh"
#include "KTManager.hh"

#include "KTDestroyer.hh"
#include "KTCoreMessage.hh"

#include "Rtypes.h"

#include <map>
using std::map;
#include <string>
using std::string;

namespace Katydid
{

    class KTManagerTable
    {
        public:
            enum ManagerTableStatus
            {
                eGood,
                eBad,
                eShuttingDown,
                eAborting,
            };

            //*********
            //singleton
            //*********

        public:
            friend class KTDestroyer< KTManagerTable >;

        public:
            static KTManagerTable* GetInstance();

        private:
            KTManagerTable();
            virtual ~KTManagerTable();
            static KTManagerTable* fInstance;
            static KTDestroyer< KTManagerTable > fMgrTableDestroyer;

            //*********
            //managers
            //*********

        public:
            //! Get the pointer to a manager
            KTManager* GetManager(const string& aType) const;

            //! Get the pointer to a manager, dynamic-casting it to a specific type
            template< class XManagerType >
            XManagerType* GetManager(const string& aType) const;

            //! Shutdown all managers
            Bool_t ShutdownManagers();

            //! Aborts all managers
            Bool_t AbortManagers();

            //*********
            //factories
            //*********

        public:
            //! Register the creation of a manager factory
            Bool_t RegisterManagerFactory(const string& aType, KTManagerFactory* aFactory);
            //
            //! Remove the reference to a manager factory
            void RemoveManagerFactory(const string& aType);

        private:
            typedef map< string, KTManagerFactory* > MgrFactoryMap;
            typedef MgrFactoryMap::value_type MgrFactoryMapEntry;
            typedef MgrFactoryMap::iterator MgrFactoryMapIt;
            typedef MgrFactoryMap::const_iterator MgrFactoryMapCIt;

            MgrFactoryMap fManagerFactoryMap;

        public:
            //! Returns the manager table's status
            ManagerTableStatus GetStatus();

        private:
            ManagerTableStatus fStatus;

    };

    template< class XManagerType >
    XManagerType* KTManagerTable::GetManager(const string& aType) const
    {
            KTManager* tManager = this->GetManager(aType);
            if (tManager == NULL)
            {
                coremsg < "KTManagerTable::GetManager (templated)";
                coremsg(eWarning) << "Returning NULL pointer; did not receive a KTManager pointer" << eom;
                return NULL;
            }

            XManagerType* tSpecificManager = dynamic_cast< XManagerType* >(tManager);
            if (tSpecificManager == NULL)
            {
                coremsg < "KTManagerTable::GetManager (templated)";
                coremsg(eError) << "Cannot dynamic-cast the manager with the type name given to the class provided!" << eom;
                return NULL;
            }
            return tSpecificManager;
    }

    inline KTManagerTable::ManagerTableStatus KTManagerTable::GetStatus()
    {
        return fStatus;
    }

} // end Katydid namespace

#endif
