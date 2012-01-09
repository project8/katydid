#ifndef KTSINGLETON_H_
#define KTSINGLETON_H_

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

        protected:
            KTSingleton();
            ~KTSingleton();
    };

    template< class XType >
    XType* KTSingleton< XType >::fInstance = NULL;

    template< class XType >
    XType* KTSingleton< XType >::GetInstance()
    {
        if( fInstance == NULL )
        {
            fInstance = new XType();
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
