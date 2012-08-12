/*
 * KTConnection.hh
 *
 *  Created on: Aug 8, 2012
 *      Author: nsoblath
 */

#ifndef KTCONNECTION_HH_
#define KTCONNECTION_HH_

#include <boost/signals2.hpp>

//#include <climits>

namespace Katydid
{
    typedef boost::signals2::connection KTConnection;
    /*
    class KTConnection : public boost::signals2::connection
    {
        public:
            KTConnection();
            virtual ~KTConnection();

    };
    */
    /*

    class KTConnection
    {
        public:
            KTConnection();
            virtual ~KTConnection();



            unsigned GetOrder() const = 0;
            void SetOrder(unsigned order) = 0;

    };

    template< typename XSignature >
    class KTFunctionConnection : public KTConnection
    {
        private:
            KTFunctionConnection();

        public:
            KTFunctionConnection(XSignature* function, unsigned order=UINT_MAX);
            virtual ~KTFunctionConnection();

            unsigned GetOrder() const;
            void SetOrder(unsigned order);

        protected:
            unsigned fOrder;

            XSignature* fFunction;
    };

    template< typename XSignature >
    KTFunctionConnection< XSignature >::KTFunctionConnection() :
            KTConnection(),
            fOrder(UINT_MAX),
            fFunction(0)
    {
    }

    template< typename XSignature >
    KTFunctionConnection< XSignature >::KTFunctionConnection(XSignature* function, unsigned order) :
            KTConnection(),
            fOrder(order),
            fFunction(function)
    {
    }
    template< typename XSignature >
    KTFunctionConnection< XSignature >::~KTFunctionConnection()
    {
    }

    template< typename XSignature >
    inline unsigned KTFunctionConnection< XSignature >::GetOrder() const
    {
        return fOrder;
    }

    template< typename XSignature >
    inline void KTFunctionConnection< XSignature >::SetOrder(unsigned order)
    {
        fOrder = order;
    }
    */

} /* namespace Katydid */
#endif /* KTCONNECTION_HH_ */
