/*
 * complexpolar.hh
 *
 *  Created on: Aug 23, 2012
 *      Author: nsoblath
 */

#ifndef COMPLEXPOLAR_OPERATORS_HH_
#define COMPLEXPOLAR_OPERATORS_HH_

#include "complexpolar_class.hh"
#include "complexpolar_functions.hh"

//#include <iostream>
//using std::cout;
//using std::endl;

namespace Katydid
{
    template<class T>
    complexpolar<T> operator+(const complexpolar<T>& lhs, const complexpolar<T>& rhs)
    {
        return complexpolar<T>(std::complex<T> (real(lhs) + real(rhs), imag(lhs) + imag(rhs)));
    }
    template<class T>
    complexpolar<T> operator+(const complexpolar<T>& lhs, const std::complex<T>& rhs)
    {
        return rect(rhs + lhs);
    }
    template<class T>
    std::complex<T> operator+(const std::complex<T>& lhs, const complexpolar<T>& rhs)
    {
        return std::complex<T>(lhs.real() + real(rhs), lhs.imag() + imag(rhs));
    }
    template<class T>
    complexpolar<T> operator+(const complexpolar<T>& lhs, const T& val)
    {
        return complexpolar<T>(std::complex<T> (real(lhs) + val, imag(lhs)));
    }
    template<class T>
    complexpolar<T> operator+(const T& val, const complexpolar<T>& rhs)
    {
        return rhs + val;
    }

    template<class T>
    complexpolar<T> operator-(const complexpolar<T>& lhs, const complexpolar<T>& rhs)
    {
        return complexpolar<T>(std::complex<T> (real(lhs) - real(rhs), imag(lhs) - imag(rhs)));
    }
    template<class T>
    complexpolar<T> operator-(const complexpolar<T>& lhs, const std::complex<T>& rhs)
    {
        return -rect(rhs - lhs);
    }
    template<class T>
    std::complex<T> operator-(const std::complex<T>& lhs, const complexpolar<T>& rhs)
    {
        return std::complex<T>(lhs.real() - real(rhs), lhs.imag() - imag(rhs));
    }
    template<class T>
    complexpolar<T> operator-(const complexpolar<T>& lhs, const T& val)
    {
        return complexpolar<T>(std::complex<T> (real(lhs) - val, imag(lhs)));
    }
    template<class T>
    complexpolar<T> operator-(const T& val, const complexpolar<T>& rhs)
    {
        return -(rhs - val);
    }

    template<class T>
    complexpolar<T> operator*(const complexpolar<T>& lhs, const complexpolar<T>& rhs)
    {
        return complexpolar<T>(lhs.abs() * rhs.abs(), lhs.arg() + rhs.arg());
    }
    template<class T>
    complexpolar<T> operator*(const complexpolar<T>& lhs, const std::complex<T>& rhs)
    {
        return complexpolar<T>(lhs.abs() * std::abs(rhs), lhs.arg() + std::arg(rhs));
    }
    template<class T>
    std::complex<T> operator*(const std::complex<T>& lhs, const complexpolar<T>& rhs)
    {
        return polar(rhs * lhs);
    }
    template<class T>
    complexpolar<T> operator*(const complexpolar<T>& lhs, const T& val)
    {
        return complexpolar<T>(lhs.abs()*val, lhs.arg());
    }
    template<class T>
    complexpolar<T> operator*(const T& val, const complexpolar<T>& rhs)
    {
        return rhs * val;
    }

    template<class T>
    complexpolar<T> operator/(const complexpolar<T>& lhs, const complexpolar<T>& rhs)
    {
        return complexpolar<T>(lhs.abs() / rhs.abs(), lhs.arg() - rhs.arg());
    }
    template<class T>
    complexpolar<T> operator/(const complexpolar<T>& lhs, const std::complex<T>& rhs)
    {
        return complexpolar<T>(lhs.abs() / std::abs(rhs), lhs.arg() - std::arg(rhs));
    }
    template<class T>
    std::complex<T> operator/(const std::complex<T>& lhs, const complexpolar<T>& rhs)
    {
        return std::polar(std::abs(lhs) / rhs.abs(), std::arg(lhs) - rhs.arg());
    }
    template<class T>
    complexpolar<T> operator/(const complexpolar<T>& lhs, const T& val)
    {
        return complexpolar<T>(lhs.abs()/val, lhs.arg());
    }
    template<class T>
    complexpolar<T> operator/(const T& val, const complexpolar<T>& rhs)
    {
        return complexpolar<T>(val/rhs.abs(), -rhs.arg());
    }

    template<class T>
    complexpolar<T> operator+(const complexpolar<T>& rhs)
    {
        return complexpolar<T>(rhs);
    }
    template<class T>
    complexpolar<T> operator-(const complexpolar<T>& rhs)
    {
        return complexpolar<T>(-rhs.abs(), rhs.arg());
    }

    template<class T>
    bool operator==(const complexpolar<T>& lhs, const complexpolar<T>& rhs)
    {
        //cout.precision(20);
        //cout.setf(std::ios::fixed,std::ios::floatfield);
        //cout << '\t' << real(lhs) << " == " << real(rhs) << " ? " << bool(real(lhs) == real(rhs)) <<  "\t" << imag(lhs) << " == " << imag(rhs) << " ? " << bool(imag(lhs) == imag(rhs)) << endl;
        return real(lhs) == real(rhs) && imag(lhs) == imag(rhs);
    }
    template<class T>
    bool operator==(const complexpolar<T>& lhs, const std::complex<T>& rhs)
    {
        //cout.precision(20);
        //cout.setf(std::ios::fixed,std::ios::floatfield);
        //cout << '\t' << real(lhs) << " == " << rhs.real() << " ? " << bool(real(lhs) == rhs.real()) << '\t' << imag(lhs) << " == " << rhs.imag() << " ? " << bool(imag(lhs) == rhs.imag()) << endl;
        return real(lhs) == rhs.real() && imag(lhs) == rhs.imag();
    }
    template<class T>
    bool operator==(const std::complex<T>& lhs, const complexpolar<T>& rhs)
    {
        return rhs == lhs;
    }
    template<class T>
    bool operator==(const complexpolar<T>& lhs, const T& val)
    {
        //cout.precision(20);
        //cout.setf(std::ios::fixed,std::ios::floatfield);
        //cout << '\t' << lhs.abs() << " == " << val << " ? " << bool(lhs.abs() == val) << '\t' << lhs.arg() << " == " << T() << " ? " << bool(lhs.arg() == T()) << endl;
        return lhs.abs() == val && lhs.arg() == T();
    }
    template<class T>
    bool operator==(const T& val, const complexpolar<T>& rhs)
    {
        return rhs == val;
    }

    template<class T>
    bool operator!=(const complexpolar<T>& lhs, const complexpolar<T>& rhs)
    {
        return ! (lhs == rhs);
    }
    template<class T>
    bool operator!=(const complexpolar<T>& lhs, const std::complex<T>& rhs)
    {
        return ! (lhs == rhs);
    }
    template<class T>
    bool operator!=(const std::complex<T>& lhs, const complexpolar<T>& rhs)
    {
        return ! (rhs == lhs);
    }
    template<class T>
    bool operator!=(const complexpolar<T>& lhs, const T& val)
    {
        return ! (lhs == val);
    }
    template<class T>
    bool operator!=(const T& val, const complexpolar<T>& rhs)
    {
        return ! (rhs == val);
    }

    /*
    template<class T>
    std::istream&
        operator>> (std::istream& istr, complexpolar<T>& rhs)
    {
    }
    */
    template<class T>
    std::ostream&
        operator<< (std::ostream& ostr, const complexpolar<T>& rhs)
    {
        ostr << "{" << rhs.abs() << "," << rhs.arg() << "}";
        return ostr;
    }

} /* namespace Katydid */
#endif /* COMPLEXPOLAR_OPERATORS_HH_ */
