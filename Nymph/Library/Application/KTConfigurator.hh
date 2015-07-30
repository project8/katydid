/*
 * KTConfigurator.hh
 *
 *  Created on: Nov 5, 2013
 *      Author: nsoblath
 */

#ifndef KTCONFIGURATOR_HH_
#define KTCONFIGURATOR_HH_

#include "KTSingleton.hh"

#include "KTParam.hh"

#include "KTException.hh"

#include <string>

namespace Nymph
{

    class KTConfigurator : public KTSingleton< KTConfigurator >
    {
        private:
            friend class KTSingleton< KTConfigurator >;
            friend class KTDestroyer< KTConfigurator >;

            KTConfigurator();
            virtual ~KTConfigurator();

        public:
            void Merge(const KTParamNode& aNode);

            KTParamNode* Config();
            const KTParamNode* Config() const;

            template< typename XReturnType >
            XReturnType Get( const std::string& aName ) const;

            template< typename XReturnType >
            XReturnType Get( const std::string& aName, XReturnType aDefault ) const;

        private:
            KTParamNode* fMasterConfig;

            mutable KTParam* fParamBuffer;

            std::string fStringBuffer;
    };

    template< typename XReturnType >
    XReturnType KTConfigurator::Get( const std::string& aName ) const
    {
        fParamBuffer = const_cast< KTParam* >( fMasterConfig->At( aName ) );
        if( fParamBuffer != NULL && fParamBuffer->IsValue() )
        {
            return fParamBuffer->AsValue().Get< XReturnType >();
        }
        throw KTException() << "configurator does not have a value for <" << aName << ">";
    }

    template< typename XReturnType >
    XReturnType KTConfigurator::Get( const std::string& aName, XReturnType aDefault ) const
    {
        fParamBuffer = const_cast< KTParam* >( fMasterConfig->At( aName ) );
        if( fParamBuffer != NULL && fParamBuffer->IsValue() )
        {
            return fParamBuffer->AsValue().Get< XReturnType >();
        }
        return aDefault;

    }


} /* namespace Nymph */
#endif /* KTCONFIGURATOR_HH_ */
