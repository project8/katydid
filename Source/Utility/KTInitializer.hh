#ifndef KTINITIALIZER_H_
#define KTINITIALIZER_H_

#include <cstddef>

#include <iostream>
using std::cout;
using std::endl;

namespace Katydid
{

    template< class XType >
    class KTInitializer
    {
            //*********************
            //data block management
            //*********************

        public:
            static char fData[sizeof(XType)];

            //*******************
            //instance management
            //*******************

        public:
            KTInitializer();
            ~KTInitializer();

        private:
            static XType* fInstance;
            static UInt_t fCount;
    };

    template< class XType >
    char KTInitializer< XType >::fData[sizeof(XType)] =
        { };

    template< class XType >
    UInt_t KTInitializer< XType >::fCount = 0;

    template< class XType >
    XType* KTInitializer< XType >::fInstance = NULL;

    template< class XType >
    KTInitializer< XType >::KTInitializer()
    {
        if( 0 == fCount++ )
        {
            fInstance = new ( &fData ) XType();
        }
    }
    template< class XType >
    KTInitializer< XType >::~KTInitializer()
    {
        if( 0 == --fCount )
        {
            fInstance->~XType();
        }
    }

}

#endif
