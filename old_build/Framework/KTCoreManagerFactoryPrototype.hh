// $Id$

#ifndef KTCOREMANAGER_FACTORY_PROTOTYPE_H
#define KTCOREMANAGER_FACTORY_PROTOTYPE_H

/**
 @file
 @brief contains KTCoreManagerFactoryPrototype
 @details
 
 <b>Revision History:</b>
 \verbatim
 Date         Name          Brief description
 -----------------------------------------------
 01/26/2011   N. Oblath     First version
 \endverbatim
 
 */

/*!
 * @class Katydid::KTCoreManagerFactoryProtoype
 @author N. Oblath
 
 @brief Templated protoype for CoreManager factories
 
 @details
 <b>Detailed Description:</b><br>
 
 */

#include "KTCoreManagerFactory.hh"
#include "KTCoreManager.hh"

namespace Katydid
{

    template< class XCoreManagerType >
    class KTCoreManagerFactoryPrototype : public KTCoreManagerFactory
    {
        public:
            KTCoreManagerFactoryPrototype(const string& aAppType);
            virtual ~KTCoreManagerFactoryPrototype();

            virtual void CreateCoreManager(Int_t aNArgs = 0, Char_t** anArgList = 0);

    };

    template< class XCoreManagerType >
    KTCoreManagerFactoryPrototype< XCoreManagerType >::KTCoreManagerFactoryPrototype(const string& aAppType) :
            KTCoreManagerFactory(aAppType)
    {
    }

    template< class XCoreManagerType >
    KTCoreManagerFactoryPrototype< XCoreManagerType >::~KTCoreManagerFactoryPrototype()
    {
    }

    template< class XCoreManagerType >
    void KTCoreManagerFactoryPrototype< XCoreManagerType >::CreateCoreManager(Int_t aNArgs, Char_t** anArgList)
    {
        fInstance = new XCoreManagerType(aNArgs, anArgList);
        return;
    }

} // end Katydid namespace

#endif
