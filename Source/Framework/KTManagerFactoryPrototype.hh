// $Id$

#ifndef KTMANAGER_FACTORY_PROTOTYPE_H
#define KTMANAGER_FACTORY_PROTOTYPE_H

/**
 @file
 @brief contains KTManagerFactoryPrototype
 @details

 <b>Revision History:</b>
 \verbatim
 Date         Name          Brief description
 -----------------------------------------------
 01/26/2011   N. Oblath     First version
 \endverbatim

 */

/*!
 * @class Katydid::KTManagerFactoryProtoype
 @author N. Oblath

 @brief Templated protoype for manager factories

 @details
 <b>Detailed Description:</b><br>

 */

#include "KTManagerFactory.hh"
#include "KTManager.hh"

namespace Katydid
{

    template< class XManagerType >
    class KTManagerFactoryPrototype :
        public KTManagerFactory
    {
        public:
            KTManagerFactoryPrototype( const string& aType );
            virtual ~KTManagerFactoryPrototype();

        protected:
            virtual void CreateManager();

    };

    template< class XManagerType >
    KTManagerFactoryPrototype< XManagerType >::KTManagerFactoryPrototype( const string& aType ) :
        KTManagerFactory( aType )
    {
    }

    template< class XManagerType >
    KTManagerFactoryPrototype< XManagerType >::~KTManagerFactoryPrototype()
    {
    }

    template< class XManagerType >
    void KTManagerFactoryPrototype< XManagerType >::CreateManager()
    {
        fInstance = new XManagerType();
        return;
    }

} // end Katydid namespace

#endif
