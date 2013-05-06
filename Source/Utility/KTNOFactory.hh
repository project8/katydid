/*
 * KTNOFactory.hh -- Named-Object Factory
 *
 *  Created on: Jul 31, 2012
 *      Author: nsoblath
 */

#ifndef KTNOFACTORY_HH_
#define KTNOFACTORY_HH_

#include "KTSingleton.hh"
#include "KTLogger.hh"

#include <map>
#include <string>

namespace Katydid
{
    KTLOGGER(utillog_factory, "katydid.utility");

    template< class XBaseType >
    class KTNOFactory;

    template< class XBaseType >
    class KTNORegistrar
    {
        public:
            KTNORegistrar() {}
            virtual ~KTNORegistrar() {}

        public:
            friend class KTNOFactory< XBaseType >;

        protected:
            virtual XBaseType* Create() const = 0;
            virtual XBaseType* CreateNamed(const std::string& name) const = 0;

    };

    template< class XBaseType, class XDerivedType >
    class KTDerivedNORegistrar : public KTNORegistrar< XBaseType >
    {
        public:
            KTDerivedNORegistrar(const std::string& className);
            virtual ~KTDerivedNORegistrar();

        protected:
            void Register(const std::string& className) const;

            XBaseType* Create() const;
            XBaseType* CreateNamed(const std::string& objectName) const;

    };


    template< class XBaseType >
    class KTNOFactory : public KTSingleton< KTNOFactory< XBaseType > >
    {
        public:
            typedef std::map< std::string, const KTNORegistrar< XBaseType >* > FactoryMap;
            typedef typename FactoryMap::value_type FactoryEntry;
            typedef typename FactoryMap::iterator FactoryIt;
            typedef typename FactoryMap::const_iterator FactoryCIt;

        public:
            XBaseType* Create(const std::string& className);
            XBaseType* Create(const FactoryCIt& iter);

            XBaseType* CreateNamed(const std::string& className);
            XBaseType* CreateNamed(const FactoryCIt& iter);

            void Register(const std::string& className, const KTNORegistrar< XBaseType >* registrar);

            FactoryCIt GetFactoryMapBegin() const;
            FactoryCIt GetFactoryMapEnd() const;

        protected:
            FactoryMap* fMap;


        protected:
            friend class KTSingleton< KTNOFactory >;
            friend class KTDestroyer< KTNOFactory >;
            KTNOFactory();
            ~KTNOFactory();
    };

    template< class XBaseType >
    XBaseType* KTNOFactory< XBaseType >::Create(const FactoryCIt& iter)
    {
        return iter->second->Create();
    }

    template< class XBaseType >
    XBaseType* KTNOFactory< XBaseType >::Create(const std::string& className)
    {
        FactoryCIt it = fMap->find(className);
        if (it == fMap->end())
        {
            KTERROR(utillog_factory, "Did not find factory for <" << className << ">.");
            return NULL;
        }

        return it->second->Create();
    }

    template< class XBaseType >
    XBaseType* KTNOFactory< XBaseType >::CreateNamed(const FactoryCIt& iter)
    {
        return iter->second->CreateNamed(iter->first);
    }

    template< class XBaseType >
    XBaseType* KTNOFactory< XBaseType >::CreateNamed(const std::string& className)
    {
        FactoryCIt it = fMap->find(className);
        if (it == fMap->end())
        {
            KTERROR(utillog_factory, "Did not find factory for <" << className << ">.");
            return NULL;
        }

        return it->second->CreateNamed(className);
    }

    template< class XBaseType >
    void KTNOFactory< XBaseType >::Register(const std::string& className, const KTNORegistrar< XBaseType >* registrar)
    {
        // A local (static) logger is created inside this function to avoid static initialization order problems
        KTLOGGER(utillog_factory_reg, "katydid.utility");

        FactoryCIt it = fMap->find(className);
        if (it != fMap->end())
        {
            KTERROR(utillog_factory_reg, "Already have factory registered for <" << className << ">.");
            return;
        }
        fMap->insert(std::pair< std::string, const KTNORegistrar< XBaseType >* >(className, registrar));
        KTDEBUG(utillog_factory_reg, "Registered a factory for class " << className << ", factory #" << fMap->size()-1);
    }

    template< class XBaseType >
    KTNOFactory< XBaseType >::KTNOFactory() :
        fMap(new FactoryMap())
    {}

    template< class XBaseType >
    KTNOFactory< XBaseType >::~KTNOFactory()
    {
        delete fMap;
    }

    template< class XBaseType >
    typename KTNOFactory< XBaseType >::FactoryCIt KTNOFactory< XBaseType >::GetFactoryMapBegin() const
    {
        return fMap->begin();
    }

    template< class XBaseType >
    typename KTNOFactory< XBaseType >::FactoryCIt KTNOFactory< XBaseType >::GetFactoryMapEnd() const
    {
        return fMap->end();
    }




    template< class XBaseType, class XDerivedType >
    KTDerivedNORegistrar< XBaseType, XDerivedType >::KTDerivedNORegistrar(const std::string& className) :
            KTNORegistrar< XBaseType >()
    {
        Register(className);
    }

    template< class XBaseType, class XDerivedType >
    KTDerivedNORegistrar< XBaseType, XDerivedType >::~KTDerivedNORegistrar()
    {}

    template< class XBaseType, class XDerivedType >
    void KTDerivedNORegistrar< XBaseType, XDerivedType >::Register(const std::string& className) const
    {
        KTNOFactory< XBaseType >::GetInstance()->Register(className, this);
        return;
    }

    template< class XBaseType, class XDerivedType >
    XBaseType* KTDerivedNORegistrar< XBaseType, XDerivedType >::Create() const
    {
        return dynamic_cast< XBaseType* >(new XDerivedType());
    }

    template< class XBaseType, class XDerivedType >
    XBaseType* KTDerivedNORegistrar< XBaseType, XDerivedType >::CreateNamed(const std::string& objectName) const
    {
        return dynamic_cast< XBaseType* >(new XDerivedType(objectName));
    }

} /* namespace Katydid */
#endif /* KTNOFACTORY_HH_ */
