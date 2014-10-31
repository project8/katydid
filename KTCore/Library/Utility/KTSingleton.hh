#ifndef KTSINGLETON_H_
#define KTSINGLETON_H_

#include "KTDestroyer.hh"

#include <cstddef>

namespace Katydid
{

    template< class XType >
    class KTSingleton
    {
        public:
            static XType* GetInstance();

        private:
            static XType* fInstance;
            static KTDestroyer< XType > fDestroyer;

        protected:
            KTSingleton();

            friend class KTDestroyer< XType >;
            ~KTSingleton();
    };

    template< class XType >
    XType* KTSingleton< XType >::fInstance = NULL;

    template< class XType >
    KTDestroyer< XType > KTSingleton< XType >::fDestroyer;

    template< class XType >
    XType* KTSingleton< XType >::GetInstance()
    {
        if( fInstance == NULL )
        {
            fInstance = new XType();
            fDestroyer.SetDoomed(fInstance);
        }
        return fInstance;
    }

    template< class XType >
    KTSingleton< XType >::KTSingleton()
    {
    }
    template< class XType >
    KTSingleton< XType >::~KTSingleton()
    {
    }

}

#endif
