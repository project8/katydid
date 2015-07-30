/*
 * KTException.hh
 *
 *  Created on: Feb 25, 2014
 *      Author: nsoblath
 */


#ifndef KTEXCEPTION_HH_
#define KTEXCEPTION_HH_

#include <sstream>
#include <exception>

namespace Nymph
{

    class KTException :
        public std::exception
    {
        public:
            KTException();
            KTException( const KTException& );
            ~KTException() throw ();

            template< class XStreamable >
            KTException& operator<<( const XStreamable& a_fragment )
            {
                    fException << a_fragment;
                return *this;
            }

            virtual const char* what() const throw();

        private:
            std::stringstream fException;
    };

}

#endif /* KTEXCEPTION_HH_ */
