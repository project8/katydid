/*
 * complexpolar.hh
 *
 *  Created on: Aug 23, 2012
 *      Author: nsoblath
 */

#ifndef COMPLEXPOLAR_CLASS_HH_
#define COMPLEXPOLAR_CLASS_HH_

#include <cmath>
#include <complex>
#include <istream>
#include <ostream>

namespace Katydid
{
    template< class T >
    class complexpolar
    {
            // typedefs
        public:
            typedef T value_type;
            typedef std::complex<T> std_complex_type;

            // constructors (and destructor)
        public:
            complexpolar(const T& abs = T(), const T& arg = T()) :
                    fAbs(abs),
                    fArg(arg)
            {}
            complexpolar(const complexpolar& cmplxp) :
                    fAbs(cmplxp.fAbs),
                    fArg(cmplxp.fArg)
            {}
            complexpolar(const std_complex_type& cmplx) :
                    fAbs(std::abs(cmplx)),
                    fArg(std::arg(cmplx))
            {}
            template< class X >
            complexpolar(const complexpolar<X>& cmplxp) :
                    fAbs(T(cmplxp.fAbs)),
                    fArg(T(cmplxp.fArg))
            {}
            ~complexpolar()
            {}

            // set functions
            void set_polar(const T& abs, const T& arg)
            {
                fAbs = abs;
                fArg = arg;
                return;
            }

            void set_rect(const T& real, const T& imag)
            {
                fAbs = std::sqrt(real*real + imag*imag);
                fArg = std::atan2(imag, real);
                return;
            }

            // operators
        public:
            complexpolar<T>& operator= (const complexpolar<T>& rhs)
            {
                fAbs = rhs.fAbs;
                fArg = rhs.fArg;
                return *this;
            }

            template<class X>
            complexpolar<T>& operator= (const complexpolar<X>& rhs)
            {
                fAbs = T(rhs.fAbs);
                fArg = T(rhs.fArg);
                return *this;
            }

            template<class X>
            complexpolar<T>& operator+= (const complexpolar<X>& rhs)
            {
                T sumx = fAbs * std::cos(fArg) + rhs.fAbs * std::cos(rhs.fArg);
                T sumy = fAbs * std::sin(fArg) + rhs.fAbs * std::sin(rhs.fArg);
                fAbs = std::sqrt(sumx*sumx + sumy*sumy);
                fArg = std::atan2(sumy, sumx);
                return *this;
            }

            template<class X>
            complexpolar<T>& operator-= (const complexpolar<X>& rhs)
            {
                T diffx = fAbs * std::cos(fArg) - rhs.fAbs * std::cos(rhs.fArg);
                T diffy = fAbs * std::sin(fArg) - rhs.fAbs * std::sin(rhs.fArg);
                fAbs = std::sqrt(diffx*diffx + diffy*diffy);
                fArg = std::atan2(diffy, diffx);
                return *this;
            }

            template<class X>
            complexpolar<T>& operator*= (const complexpolar<X>& rhs)
            {
                fAbs = fAbs * T(rhs.fAbs);
                fArg = fArg + T(rhs.fArg);
                return *this;
            }

            template<class X>
            complexpolar<T>& operator/= (const complexpolar<X>& rhs)
            {
                fAbs = fAbs / T(rhs.fAbs);
                fArg = fArg - T(rhs.fArg);
                return *this;
            }

            complexpolar<T>& operator= (const std_complex_type& rhs)
            {
                fAbs = std::abs(rhs);
                fArg = std::arg(rhs);
                return *this;
            }

            template<class X>
            complexpolar<T>& operator=  (const std::complex<X>& rhs)
            {
                fAbs = T(std::abs(rhs));
                fArg = T(std::arg(rhs));
                return *this;
            }

            template<class X>
            complexpolar<T>& operator+= (const std::complex<X>& rhs)
            {
                T sumx = fAbs * std::cos(fArg) + rhs.real();
                T sumy = fAbs * std::sin(fArg) + rhs.imag();
                fAbs = std::sqrt(sumx*sumx + sumy*sumy);
                fArg = std::atan2(sumy, sumx);
                return *this;
            }

            template<class X>
            complexpolar<T>& operator-= (const std::complex<X>& rhs)
            {
                T diffx = fAbs * std::cos(fArg) - rhs.real();
                T diffy = fAbs * std::sin(fArg) - rhs.imag();
                fAbs = std::sqrt(diffx*diffx + diffy*diffy);
                fArg = std::atan2(diffy, diffx);
                return *this;
            }

            template<class X>
            complexpolar<T>& operator*= (const std::complex<X>& rhs)
            {
                fAbs = fAbs * T(std::abs(rhs));
                fArg = fArg + T(std::arg(rhs));
                return *this;
            }

            template<class X>
            complexpolar<T>& operator/= (const std::complex<X>& rhs)
            {
                fAbs = fAbs / T(std::abs(rhs));
                fArg = fArg - T(std::arg(rhs));
                return *this;
            }


            template<class X>
            complexpolar<T>& operator=  (const X& rhs)
            {
                fAbs = T(rhs);
                fArg = T();
                return *this;
            }

            template<class X>
            complexpolar<T>& operator+= (const X& rhs)
            {
                T sumx = fAbs * std::cos(fArg) + T(rhs);
                T sumy = fAbs * std::sin(fArg);
                fAbs = std::sqrt(sumx*sumx + sumy*sumy);
                fArg = std::atan2(sumy, sumx);
                return *this;
            }

            template<class X>
            complexpolar<T>& operator-= (const X& rhs)
            {
                T diffx = fAbs * std::cos(fArg) - T(rhs);
                T diffy = fAbs * std::sin(fArg);
                fAbs = std::sqrt(diffx*diffx + diffy*diffy);
                fArg = std::atan2(diffy, diffx);
                return *this;
            }

            template<class X>
            complexpolar<T>& operator*= (const X& rhs)
            {
                fAbs = fAbs * T(rhs);
                fArg = fArg + T();
                return *this;
            }

            template<class X>
            complexpolar<T>& operator/= (const X& rhs)
            {
                fAbs = fAbs / T(rhs);
                fArg = fArg - T();
                return *this;
            }

        public:
            T abs() const
            {
                return fAbs;
            }

            T arg() const
            {
                return fArg;
            }

        private:
            T fAbs;
            T fArg;
    };


} /* namespace Katydid */
#endif /* COMPLEXPOLAR_CLASS_HH_ */
