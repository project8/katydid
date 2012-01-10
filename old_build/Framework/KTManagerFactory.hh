// $Id$

#ifndef KTMANAGER_FACTORY_BASE_H
#define KTMANAGER_FACTORY_BASE_H

/**
 @file
 @brief contains KTManagerFactory
 @details
 
 <b>Revision History:</b>
 \verbatim
 Date         Name          Brief description
 -----------------------------------------------
 08/04/2010   N. Oblath     First version
 31/01/2011   N. Oblath     Made this the base class for KTManagerFactoryPrototype
 \endverbatim
 
 */

/*!
 * @class Katydid::KTManagerFactory
 @author N. Oblath
 
 @brief Base class for manager factories
 
 @details
 <b>Detailed Description:</b><br>
 
 */

#include <string>
using std::string;

namespace Katydid
{
    class KTManager;

    class KTManagerFactory
    {
        public:
            friend class KTManager;

        public:
            KTManagerFactory(const string& aType);
            virtual ~KTManagerFactory();

        private:
            KTManagerFactory();

            //********
            //creation
            //********

        public:
            KTManager* GetManager();

        protected:
            virtual void CreateManager() = 0;
            KTManager* fInstance;

            //**************
            //identification
            //**************

        public:
            const string& GetTypeName() const;

            enum FactoryStatus {
                eNoManager,
                eManagerExists,
                eManagerInactive,
                eBad
            };
            FactoryStatus GetStatus() const;

        private:
            string fTypeName;

            FactoryStatus fStatus;

    };

} // end Katydid namespace

#endif
