// $Id$

#ifndef KTCOREMANAGER_TABLE_H
#define KTCOREMANAGER_TABLE_H

/**
 @file
 @brief contains KTCoreManagerTable
 @details
 
 <b>Revision History:</b>
 \verbatim
 Date         Name          Brief description
 -----------------------------------------------
 08/04/2010   N. Oblath     First version
 \endverbatim
 
 */

/*!
 * @class Katydid::KTCoreManagerTable
 @author N. Oblath
 
 @brief Singleton lookup table for manager instances
 
 @details
 <b>Detailed Description:</b><br>
 
 */

#include "KTCoreManagerFactory.hh"
#include "KTCoreManager.hh"

#include "KTDestroyer.hh"
#include "KTCoreMessage.hh"

#include "Rtypes.h"

#include <map>
using std::map;
#include <string>
using std::string;

namespace Katydid
{

    class KTCoreManagerTable
    {

            //*********
            //singleton
            //*********

        public:
            friend class KTDestroyer< KTCoreManagerTable >;

        public:
            static KTCoreManagerTable* GetInstance();

        private:
            KTCoreManagerTable();
            virtual ~KTCoreManagerTable();
            static KTCoreManagerTable* fInstance;
            static KTDestroyer< KTCoreManagerTable > fCoreMgrTableDestroyer;

            //************
            //coremanagers
            //************

        public:
            //! Create the coremanager
            KTCoreManager* CreateCoreManager(const string& aAppType, Int_t aNArgs = 0, Char_t** anArgList = 0);

            //! Create the coremanager, dynamic-casting it to a specific type
            template< class XCoreManagerType >
            XCoreManagerType* CreateCoreManager(const string& aAppType, Int_t aNArgs = 0, Char_t** anArgList = 0);

            //! Check if a CoreManager already exists
            Bool_t CoreManagerExists() const;

            //! Get the pointer to a coremanager
            KTCoreManager* GetCoreManager() const;

            //! Get the pointer to a coremanager, dynamic-casting it to a specific type
            template< class XCoreManagerType >
            XCoreManagerType* GetCoreManager() const;

            //! Initiate global shutdown via the CoreManager
            Bool_t ShutdownCoreManager();

            //! Initiate global abort (calls KTManagerTable::AbortManagers())
            Bool_t AbortCoreManager();

            //*********
            //factories
            //*********

        public:
            //! Register the creation of a coremanager factory
            Bool_t RegisterCoreManagerFactory(const string& aType, KTCoreManagerFactory* aFactory);
            //
            //! Remove the reference to a coremanager factory
            void RemoveCoreManagerFactory(const string& aType);

        private:
            typedef map< string, KTCoreManagerFactory* > TBFactoryMap;
            typedef TBFactoryMap::value_type TBFactoryMapEntry;
            typedef TBFactoryMap::iterator TBFactoryMapIt;
            typedef TBFactoryMap::const_iterator TBFactoryMapCIt;

            TBFactoryMap fCoreManagerFactoryMap;

            KTCoreManagerFactory* fSpecifiedFactory;

            //******
            //status
            //******

        public:
            enum TableStatus {
                eUnspecified,
                eSpecified,
                eShuttingDown,
                eAborting
            };
            TableStatus GetStatus() const;

        private:
            TableStatus fStatus;

    };

    template< class XCoreManagerType >
    XCoreManagerType* KTCoreManagerTable::CreateCoreManager(const string& aAppType, Int_t aNArgs, Char_t** anArgList)
    {
            return dynamic_cast< XCoreManagerType* >( this->CreateCoreManager(aAppType, aNArgs, anArgList) );
    }

    template< class XCoreManagerType >
    XCoreManagerType* KTCoreManagerTable::GetCoreManager() const
    {
            KTCoreManager* tCoreManager = this->GetCoreManager();
            if (tCoreManager == NULL)
            {
                coremsg < "KTCoreManagerTable::GetCoreManager (templated)";
                coremsg(eWarning) << "Returning NULL pointer; did not receive a KTCoreManager pointer" << eom;
                return NULL;
            }

            XCoreManagerType* tSpecificCoreManager = dynamic_cast< XCoreManagerType* >(tCoreManager);
            if (tSpecificCoreManager == NULL)
            {
                coremsg < "KTCoreManagerTable::GetCoreManager (templated)";
                coremsg(eError) << "Cannot dynamic-cast the coremanager with the type name given to the class provided!" << eom;
                return NULL;
            }
            return tSpecificCoreManager;
    }

} // end Katydid namespace

#endif
