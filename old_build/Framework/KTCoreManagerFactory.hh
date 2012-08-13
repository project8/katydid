// $Id$

#ifndef KTCOREMANAGER_FACTORY_H_
#define KTCOREMANAGER_FACTORY_H_

/**
 @file
 @brief contains KTCoreManagerFactory
 @details
 
 <b>Revision History:</b>
 \verbatim
 Date         Name          Brief description
 -----------------------------------------------
 13/04/2011   N. Oblath     First version
 \endverbatim
 
 */

/*!
 * @class Katydid::KTCoreManagerFactory
 @author N. Oblath
 
 @brief Base class for CoreManager factories
 
 @details
 <b>Detailed Description:</b><br>
 
 */

#include "Rtypes.h"

#include <string>
using std::string;

namespace Katydid
{
    class KTCoreManager;

    class KTCoreManagerFactory
    {
        public:
            KTCoreManagerFactory(const string& aAppType);
            virtual ~KTCoreManagerFactory();

        private:
            KTCoreManagerFactory();

            //********
            //creation
            //********

        public:
            KTCoreManager* GetCoreManager();

            virtual void CreateCoreManager(Int_t aNArgs = 0, Char_t** anArgList = 0) = 0;

        protected:
            KTCoreManager* fInstance;

            //**************
            //identification
            //**************

        public:
            const string& GetApplicationTypeName() const;

            enum FactoryStatus {
                eGood,
                eBad
            };
            FactoryStatus GetStatus() const;

        private:
            string fApplicationTypeName;

            FactoryStatus fStatus;

    };

} // end Katydid namespace

#endif
