/*
 * KTDestroyer.hh
 *
 *  Created on: Nov 7, 2011
 *      Author: nsoblath
 */

#ifndef KTDESTROYER_H_
#define KTDESTROYER_H_

namespace Nymph
{

    template< class XDoomed >
    class KTDestroyer
    {
        public:
            KTDestroyer( XDoomed* = 0 );
            ~KTDestroyer();

            void SetDoomed( XDoomed* );

        private:
            // Prevent users from making copies of a KTDestroyer to avoid double deletion:
            KTDestroyer( const KTDestroyer< XDoomed >& );
            void operator=( const KTDestroyer< XDoomed >& );

        private:
            XDoomed* fDoomed;
    };

    template< class XDoomed >
    KTDestroyer< XDoomed >::KTDestroyer( XDoomed* d )
    {
        fDoomed = d;
    }

    template< class XDoomed >
    KTDestroyer< XDoomed >::~KTDestroyer()
    {
        delete fDoomed;
    }

    template< class XDoomed >
    void KTDestroyer< XDoomed >::SetDoomed( XDoomed* d )
    {
        fDoomed = d;
    }

} /* namespace Nymph */
#endif /* KTDESTROYER_H_ */
