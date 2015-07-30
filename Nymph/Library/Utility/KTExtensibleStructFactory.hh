/*
 * KTExtensibleStructFactory.hh
 *
 *  Created on: Sept 22, 2014
 *      Author: nsoblath
 */

#ifndef KTEXTENSIBLESTRUCTFACTORY_HH_
#define KTEXTENSIBLESTRUCTFACTORY_HH_

#include "KTExtensibleStruct.hh"
#include "KTSingleton.hh"
#include "KTLogger.hh"

#include <map>
#include <string>

namespace Nymph
{
    KTLOGGER(utillog_esfactory, "KTExtensibleStructFactory");

    template< class XBaseType >
    class KTExtensibleStructFactory;

    template< class XBaseType >
    class KTExtensibleStructRegistrarBase
    {
        public:
            KTExtensibleStructRegistrarBase() {}
            virtual ~KTExtensibleStructRegistrarBase() {}

        public:
            friend class KTExtensibleStructFactory< XBaseType >;

        protected:
            virtual KTExtensibleStructCore< XBaseType >* Create() const = 0;
            virtual KTExtensibleStructCore< XBaseType >* Create(KTExtensibleStructCore< XBaseType >* object) const = 0;

    };

    template< class XBaseType, class XDerivedType >
    class KTExtensibleStructRegistrar : public KTExtensibleStructRegistrarBase< XBaseType >
    {
        public:
            KTExtensibleStructRegistrar(const std::string& className);
            virtual ~KTExtensibleStructRegistrar();

        protected:
            void Register(const std::string& className) const;

            KTExtensibleStructCore< XBaseType >* Create() const;
            KTExtensibleStructCore< XBaseType >* Create(KTExtensibleStructCore< XBaseType >* object) const;

    };


    template< class XBaseType >
    class KTExtensibleStructFactory : public KTSingleton< KTExtensibleStructFactory< XBaseType > >
    {
        public:
            typedef std::map< std::string, const KTExtensibleStructRegistrarBase< XBaseType >* > FactoryMap;
            typedef typename FactoryMap::value_type FactoryEntry;
            typedef typename FactoryMap::iterator FactoryIt;
            typedef typename FactoryMap::const_iterator FactoryCIt;

        public:
            KTExtensibleStructCore< XBaseType >* Create(const std::string& className);
            KTExtensibleStructCore< XBaseType >* Create(const FactoryCIt& iter);

            KTExtensibleStructCore< XBaseType >* Create(const std::string& className, KTExtensibleStructCore< XBaseType >* object);
            KTExtensibleStructCore< XBaseType >* Create(const FactoryCIt& iter, KTExtensibleStructCore< XBaseType >* object);

            void Register(const std::string& className, const KTExtensibleStructRegistrarBase< XBaseType >* registrar);

            FactoryCIt GetFactoryMapBegin() const;
            FactoryCIt GetFactoryMapEnd() const;

        protected:
            FactoryMap* fMap;


        protected:
            friend class KTSingleton< KTExtensibleStructFactory >;
            friend class KTDestroyer< KTExtensibleStructFactory >;
            KTExtensibleStructFactory();
            ~KTExtensibleStructFactory();
    };

    template< class XBaseType >
    KTExtensibleStructCore< XBaseType >* KTExtensibleStructFactory< XBaseType >::Create(const FactoryCIt& iter)
    {
        return iter->second->Create();
    }

    template< class XBaseType >
    KTExtensibleStructCore< XBaseType >* KTExtensibleStructFactory< XBaseType >::Create(const std::string& className)
    {
        FactoryCIt it = fMap->find(className);
        if (it == fMap->end())
        {
            KTERROR(utillog_esfactory, "Did not find factory for <" << className << ">.");
            return NULL;
        }

        return it->second->Create();
    }

    template< class XBaseType >
    KTExtensibleStructCore< XBaseType >* KTExtensibleStructFactory< XBaseType >::Create(const FactoryCIt& iter, KTExtensibleStructCore< XBaseType >* object)
    {
        return iter->second->Create(object);
    }

    template< class XBaseType >
    KTExtensibleStructCore< XBaseType >* KTExtensibleStructFactory< XBaseType >::Create(const std::string& className, KTExtensibleStructCore< XBaseType >* object)
    {
        FactoryCIt it = fMap->find(className);
        if (it == fMap->end())
        {
            KTERROR(utillog_esfactory, "Did not find factory for <" << className << ">.");
            return NULL;
        }

        return it->second->Create(object);
    }

    template< class XBaseType >
    void KTExtensibleStructFactory< XBaseType >::Register(const std::string& className, const KTExtensibleStructRegistrarBase< XBaseType >* registrar)
    {
        // A local (static) logger is created inside this function to avoid static initialization order problems
        KTLOGGER(utillog_esfactory_reg, "KTExtensibleStructFactory-Register");

        FactoryCIt it = fMap->find(className);
        if (it != fMap->end())
        {
            KTERROR(utillog_esfactory_reg, "Already have factory registered for <" << className << ">.");
            return;
        }
        fMap->insert(std::pair< std::string, const KTExtensibleStructRegistrarBase< XBaseType >* >(className, registrar));
        KTDEBUG(utillog_esfactory_reg, "Registered a factory for class " << className << ", factory #" << fMap->size()-1);
    }

    template< class XBaseType >
    KTExtensibleStructFactory< XBaseType >::KTExtensibleStructFactory() :
        fMap(new FactoryMap())
    {}

    template< class XBaseType >
    KTExtensibleStructFactory< XBaseType >::~KTExtensibleStructFactory()
    {
        delete fMap;
    }

    template< class XBaseType >
    typename KTExtensibleStructFactory< XBaseType >::FactoryCIt KTExtensibleStructFactory< XBaseType >::GetFactoryMapBegin() const
    {
        return fMap->begin();
    }

    template< class XBaseType >
    typename KTExtensibleStructFactory< XBaseType >::FactoryCIt KTExtensibleStructFactory< XBaseType >::GetFactoryMapEnd() const
    {
        return fMap->end();
    }




    template< class XBaseType, class XDerivedType >
    KTExtensibleStructRegistrar< XBaseType, XDerivedType >::KTExtensibleStructRegistrar(const std::string& className) :
            KTExtensibleStructRegistrarBase< XBaseType >()
    {
        Register(className);
    }

    template< class XBaseType, class XDerivedType >
    KTExtensibleStructRegistrar< XBaseType, XDerivedType >::~KTExtensibleStructRegistrar()
    {}

    template< class XBaseType, class XDerivedType >
    void KTExtensibleStructRegistrar< XBaseType, XDerivedType >::Register(const std::string& className) const
    {
        KTExtensibleStructFactory< XBaseType >::GetInstance()->Register(className, this);
        return;
    }

    template< class XBaseType, class XDerivedType >
    KTExtensibleStructCore< XBaseType >* KTExtensibleStructRegistrar< XBaseType, XDerivedType >::Create() const
    {
        return dynamic_cast< KTExtensibleStructCore< XBaseType >* >(new XDerivedType());
    }

    template< class XBaseType, class XDerivedType >
    KTExtensibleStructCore< XBaseType >* KTExtensibleStructRegistrar< XBaseType, XDerivedType >::Create(KTExtensibleStructCore< XBaseType >* object) const
    {
        return dynamic_cast< KTExtensibleStructCore< XBaseType >* >(&object->template Of< XDerivedType >());
    }

} /* namespace Nymph */
#endif /* KTEXTENSIBLESTRUCTFACTORY_HH_ */
