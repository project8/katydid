/*
 * complexpolar.hh
 *
 *  Created on: Aug 23, 2012
 *      Author: nsoblath
 */

#ifndef COMPLEXPOLAR_FUNCTIONS_HH_
#define COMPLEXPOLAR_FUNCTIONS_HH_

#include "complexpolar/class.hh"

namespace Katydid
{
    template<class T>
    T real(const complexpolar<T>& x)
    {
        return x.abs() * std::cos(x.arg());
    }

    template<class T>
    T imag(const complexpolar<T>& x)
    {
        return x.abs() * std::sin(x.arg());
    }

    template<class T>
    T abs(const complexpolar<T>& x)
    {
        return x.abs();
    }

    template<class T>
    T arg(const complexpolar<T>& x)
    {
        return x.arg();
    }

    template<class T>
    T norm(const complexpolar<T>& x)
    {
        return x.abs() * x.abs();
    }

    template<class T>
    complexpolar<T> conj(const complexpolar<T>& x)
    {
        return complexpolar<T>(x.abs(), -x.arg());
    }

    template<class T>
    complexpolar<T> rect(const std::complex<T>& x)
    {
        return complexpolar<T>(x);
    }

    template<class T>
    complexpolar<T> rect(const T& real, const T& imag = 0)
    {
        return rect(std::complex<T>(real, imag));
    }

    template<class T>
    std::complex<T> polar(const complexpolar<T>& x)
    {
        return std::polar(x.abs(), x.arg());
    }


} /* namespace Katydid */
#endif /* COMPLEXPOLAR_FUNCTIONS_HH_ */
