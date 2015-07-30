/*
 * KTFactory.hh
 *
 *  Created on: Jul 31, 2012
 *      Author: nsoblath
 */

#ifndef KTFACTORY_HH_
#define KTFACTORY_HH_

#include "KTSingleton.hh"
#include "KTLogger.hh"

#include <map>
#include <string>

namespace Nymph
{
    KTLOGGER(utillog_factory, "KTFactory");

    template< class XBaseType >
    class KTFactory;

    template< class XBaseType >
    class KTRegistrarBase
    {
        public:
            KTRegistrarBase() {}
            virtual ~KTRegistrarBase() {}

        public:
            friend class KTFactory< XBaseType >;

        protected:
            virtual XBaseType* Create() const = 0;

    };

    template< class XBaseType, class XDerivedType >
    class KTRegistrar : public KTRegistrarBase< XBaseType >
    {
        public:
            KTRegistrar(const std::string& className);
            virtual ~KTRegistrar();

        protected:
            void Register(const std::string& className) const;

            XBaseType* Create() const;

    };


    template< class XBaseType >
    class KTFactory : public KTSingleton< KTFactory< XBaseType > >
    {
        public:
            typedef std::map< std::string, const KTRegistrarBase< XBaseType >* > FactoryMap;
            typedef typename FactoryMap::value_type FactoryEntry;
            typedef typename FactoryMap::iterator FactoryIt;
            typedef typename FactoryMap::const_iterator FactoryCIt;

        public:
            XBaseType* Create(const std::string& className);
            XBaseType* Create(const FactoryCIt& iter);

            void Register(const std::string& className, const KTRegistrarBase< XBaseType >* registrar);

            FactoryCIt GetFactoryMapBegin() const;
            FactoryCIt GetFactoryMapEnd() const;

        protected:
            FactoryMap* fMap;


        protected:
            friend class KTSingleton< KTFactory >;
            friend class KTDestroyer< KTFactory >;
            KTFactory();
            ~KTFactory();
    };

    template< class XBaseType >
    XBaseType* KTFactory< XBaseType >::Create(const FactoryCIt& iter)
    {
        return iter->second->Create();
    }

    template< class XBaseType >
    XBaseType* KTFactory< XBaseType >::Create(const std::string& className)
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
    void KTFactory< XBaseType >::Register(const std::string& className, const KTRegistrarBase< XBaseType >* registrar)
    {
        // A local (static) logger is created inside this function to avoid static initialization order problems
        KTLOGGER(utillog_factory_reg, "KTFactory-Register");

        FactoryCIt it = fMap->find(className);
        if (it != fMap->end())
        {
            KTERROR(utillog_factory_reg, "Already have factory registered for <" << className << ">.");
            return;
        }
        fMap->insert(std::pair< std::string, const KTRegistrarBase< XBaseType >* >(className, registrar));
        KTDEBUG(utillog_factory_reg, "Registered a factory for class " << className << ", factory #" << fMap->size()-1);
    }

    template< class XBaseType >
    KTFactory< XBaseType >::KTFactory() :
        fMap(new FactoryMap())
    {}

    template< class XBaseType >
    KTFactory< XBaseType >::~KTFactory()
    {
        delete fMap;
    }

    template< class XBaseType >
    typename KTFactory< XBaseType >::FactoryCIt KTFactory< XBaseType >::GetFactoryMapBegin() const
    {
        return fMap->begin();
    }

    template< class XBaseType >
    typename KTFactory< XBaseType >::FactoryCIt KTFactory< XBaseType >::GetFactoryMapEnd() const
    {
        return fMap->end();
    }




    template< class XBaseType, class XDerivedType >
    KTRegistrar< XBaseType, XDerivedType >::KTRegistrar(const std::string& className) :
            KTRegistrarBase< XBaseType >()
    {
        Register(className);
    }

    template< class XBaseType, class XDerivedType >
    KTRegistrar< XBaseType, XDerivedType >::~KTRegistrar()
    {}

    template< class XBaseType, class XDerivedType >
    void KTRegistrar< XBaseType, XDerivedType >::Register(const std::string& className) const
    {
        KTFactory< XBaseType >::GetInstance()->Register(className, this);
        return;
    }

    template< class XBaseType, class XDerivedType >
    XBaseType* KTRegistrar< XBaseType, XDerivedType >::Create() const
    {
        return dynamic_cast< XBaseType* >(new XDerivedType());
    }

} /* namespace Nymph */
#endif /* KTFACTORY_HH_ */
