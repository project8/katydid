/*
 * complexpolar.hh
 *
 *  Created on: Aug 23, 2012
 *      Author: nsoblath
 */

#ifndef COMPLEXPOLAR_SPECIALIZATION_HH_
#define COMPLEXPOLAR_SPECIALIZATION_HH_

#include "complexpolar/class.hh"

namespace Katydid
{
    template<>
    class complexpolar<float>
    {
            // typedefs
        public:
            typedef float value_type;
            typedef std::complex<float> std_complex_type;

            // constructors (and destructor)
        public:
            complexpolar(const float& abs = 0., const float& arg = 0.);
            complexpolar(const complexpolar<float>& cmplxp);
            complexpolar(const std_complex_type& cmplx);
            template< class X >
            complexpolar(const complexpolar<X>& cmplxp) :
                    fAbs(float(cmplxp.fAbs)), fArg(float(cmplxp.fArg))
            {}
            ~complexpolar();

            // set functions
            void set_polar(const float& abs, const float& arg)
            {
                fAbs = abs;
                fArg = arg;
                return;
            }

            void set_rect(const float& real, const float& imag)
            {
                fAbs = std::sqrt(real*real + imag*imag);
                fArg = std::atan2(imag, real);
                return;
            }

            // operators
        public:
            complexpolar& operator= (const complexpolar& rhs);

            template<class X>
            complexpolar<float>& operator=  (const complexpolar<X>& rhs)
            {
                fAbs = float(rhs.fAbs);
                fArg = float(rhs.fArg);
                return *this;
            }

            template<class X>
            complexpolar<float>& operator+= (const complexpolar<X>& rhs)
            {
                float sumx = fAbs * std::cos(fArg) + rhs.fAbs * std::cos(rhs.fArg);
                float sumy = fAbs * std::sin(fArg) + rhs.fAbs * std::sin(rhs.fArg);
                fAbs = std::sqrt(sumx*sumx + sumy*sumy);
                fArg = std::atan2(sumy, sumx);
                return *this;
            }

            template<class X>
            complexpolar<float>& operator-= (const complexpolar<X>& rhs)
            {
                float diffx = fAbs * std::cos(fArg) - rhs.fAbs * std::cos(rhs.fArg);
                float diffy = fAbs * std::sin(fArg) - rhs.fAbs * std::sin(rhs.fArg);
                fAbs = std::sqrt(diffx*diffx + diffy*diffy);
                fArg = std::atan2(diffy, diffx);
                return *this;
            }

            template<class X>
            complexpolar<float>& operator*= (const complexpolar<X>& rhs)
            {
                fAbs = fAbs * float(rhs.fAbs);
                fArg = fArg + float(rhs.fArg);
                return *this;
            }

            template<class X>
            complexpolar<float>& operator/= (const complexpolar<X>& rhs)
            {
                fAbs = fAbs / float(rhs.fAbs);
                fArg = fArg - float(rhs.fArg);
                return *this;
            }

            complexpolar& operator= (const std_complex_type& rhs);

            template<class X>
            complexpolar<float>& operator=  (const std::complex<X>& rhs)
            {
                fAbs = float(std::abs(rhs));
                fArg = float(std::arg(rhs));
                return *this;
            }

            template<class X>
            complexpolar<float>& operator+= (const std::complex<X>& rhs)
            {
                float sumx = fAbs * std::cos(fArg) + rhs.real();
                float sumy = fAbs * std::sin(fArg) + rhs.imag();
                fAbs = std::sqrt(sumx*sumx + sumy*sumy);
                fArg = std::atan2(sumy, sumx);
                return *this;
            }

            template<class X>
            complexpolar<float>& operator-= (const std::complex<X>& rhs)
            {
                float diffx = fAbs * std::cos(fArg) - rhs.real();
                float diffy = fAbs * std::sin(fArg) - rhs.imag();
                fAbs = std::sqrt(diffx*diffx + diffy*diffy);
                fArg = std::atan2(diffy, diffx);
                return *this;
            }

            template<class X>
            complexpolar<float>& operator*= (const std::complex<X>& rhs)
            {
                fAbs = fAbs * float(std::abs(rhs));
                fArg = fArg + float(std::arg(rhs));
                return *this;
            }

            template<class X>
            complexpolar<float>& operator/= (const std::complex<X>& rhs)
            {
                fAbs = fAbs / float(std::abs(rhs));
                fArg = fArg - float(std::arg(rhs));
                return *this;
            }

            template<class X>
            complexpolar<float>& operator=  (const X& rhs)
            {
                fAbs = float(rhs);
                fArg = 0.;
                return *this;
            }

            template<class X>
            complexpolar<float>& operator+= (const X& rhs)
            {
                float sumx = fAbs * std::cos(fArg) + float(rhs);
                float sumy = fAbs * std::sin(fArg);
                fAbs = std::sqrt(sumx*sumx + sumy*sumy);
                fArg = std::atan2(sumy, sumx);
                return *this;
            }

            template<class X>
            complexpolar<float>& operator-= (const X& rhs)
            {
                float diffx = fAbs * std::cos(fArg) - float(rhs);
                float diffy = fAbs * std::sin(fArg);
                fAbs = std::sqrt(diffx*diffx + diffy*diffy);
                fArg = std::atan2(diffy, diffx);
                return *this;
            }

            template<class X>
            complexpolar<float>& operator*= (const X& rhs)
            {
                fAbs = fAbs * float(rhs);
                return *this;
            }

            template<class X>
            complexpolar<float>& operator/= (const X& rhs)
            {
                fAbs = fAbs / float(rhs);
                return *this;
            }

        public:
            complexpolar<float>& conj()
            {
                fArg = -fArg;
                return *this;
            }

        public:
            float abs() const;
            float arg() const;

        private:
            float fAbs;
            float fArg;
    };

    inline float complexpolar<float>::abs() const
    {
        return fAbs;
    }
    inline float complexpolar<float>::arg() const
    {
        return fArg;
    }


    template<>
    class complexpolar<double>
    {
            // typedefs
        public:
            typedef double value_type;
            typedef std::complex<double> std_complex_type;

            // constructors (and destructor)
        public:
            complexpolar(const double& abs = 0., const double& arg = 0.);
            complexpolar(const complexpolar<double>& cmplxp);
            complexpolar(const std_complex_type& cmplx);
            template< class X >
            complexpolar(const complexpolar<X>& cmplxp) :
                    fAbs(double(cmplxp.fAbs)), fArg(double(cmplxp.fArg))
            {}
            ~complexpolar();

            // set functions
            void set_polar(const double& abs, const double& arg)
            {
                fAbs = abs;
                fArg = arg;
                return;
            }

            void set_rect(const double& real, const double& imag)
            {
                fAbs = std::sqrt(real*real + imag*imag);
                fArg = std::atan2(imag, real);
                return;
            }

            // operators
        public:
            complexpolar& operator= (const complexpolar& rhs);

            template<class X>
            complexpolar<double>& operator=  (const complexpolar<X>& rhs)
            {
                fAbs = double(rhs.fAbs);
                fArg = double(rhs.fArg);
                return *this;
            }

            template<class X>
            complexpolar<double>& operator+= (const complexpolar<X>& rhs)
            {
                double sumx = fAbs * std::cos(fArg) + rhs.fAbs * std::cos(rhs.fArg);
                double sumy = fAbs * std::sin(fArg) + rhs.fAbs * std::sin(rhs.fArg);
                fAbs = std::sqrt(sumx*sumx + sumy*sumy);
                fArg = std::atan2(sumy, sumx);
                return *this;
            }

            template<class X>
            complexpolar<double>& operator-= (const complexpolar<X>& rhs)
            {
                double diffx = fAbs * std::cos(fArg) - rhs.fAbs * std::cos(rhs.fArg);
                double diffy = fAbs * std::sin(fArg) - rhs.fAbs * std::sin(rhs.fArg);
                fAbs = std::sqrt(diffx*diffx + diffy*diffy);
                fArg = std::atan2(diffy, diffx);
                return *this;
            }

            template<class X>
            complexpolar<double>& operator*= (const complexpolar<X>& rhs)
            {
                fAbs = fAbs * double(rhs.fAbs);
                fArg = fArg + double(rhs.fArg);
                return *this;
            }

            template<class X>
            complexpolar<double>& operator/= (const complexpolar<X>& rhs)
            {
                fAbs = fAbs / double(rhs.fAbs);
                fArg = fArg - double(rhs.fArg);
                return *this;
            }

            complexpolar& operator= (const std_complex_type& rhs);

            template<class X>
            complexpolar<double>& operator=  (const std::complex<X>& rhs)
            {
                fAbs = double(std::abs(rhs));
                fArg = double(std::arg(rhs));
                return *this;
            }

            template<class X>
            complexpolar<double>& operator+= (const std::complex<X>& rhs)
            {
                double sumx = fAbs * std::cos(fArg) + rhs.real();
                double sumy = fAbs * std::sin(fArg) + rhs.imag();
                fAbs = std::sqrt(sumx*sumx + sumy*sumy);
                fArg = std::atan2(sumy, sumx);
                return *this;
            }

            template<class X>
            complexpolar<double>& operator-= (const std::complex<X>& rhs)
            {
                double diffx = fAbs * std::cos(fArg) - rhs.real();
                double diffy = fAbs * std::sin(fArg) - rhs.imag();
                fAbs = std::sqrt(diffx*diffx + diffy*diffy);
                fArg = std::atan2(diffy, diffx);
                return *this;
            }

            template<class X>
            complexpolar<double>& operator*= (const std::complex<X>& rhs)
            {
                fAbs = fAbs * double(std::abs(rhs));
                fArg = fArg + double(std::arg(rhs));
                return *this;
            }

            template<class X>
            complexpolar<double>& operator/= (const std::complex<X>& rhs)
            {
                fAbs = fAbs / double(std::abs(rhs));
                fArg = fArg - double(std::arg(rhs));
                return *this;
            }

            template<class X>
            complexpolar<double>& operator=  (const X& rhs)
            {
                fAbs = double(rhs);
                fArg = 0.;
                return *this;
            }

            template<class X>
            complexpolar<double>& operator+= (const X& rhs)
            {
                double sumx = fAbs * std::cos(fArg) + double(rhs);
                double sumy = fAbs * std::sin(fArg);
                fAbs = std::sqrt(sumx*sumx + sumy*sumy);
                fArg = std::atan2(sumy, sumx);
                return *this;
            }

            template<class X>
            complexpolar<double>& operator-= (const X& rhs)
            {
                double diffx = fAbs * std::cos(fArg) - double(rhs);
                double diffy = fAbs * std::sin(fArg);
                fAbs = std::sqrt(diffx*diffx + diffy*diffy);
                fArg = std::atan2(diffy, diffx);
                return *this;
            }

            template<class X>
            complexpolar<double>& operator*= (const X& rhs)
            {
                fAbs = fAbs * double(rhs);
                return *this;
            }

            template<class X>
            complexpolar<double>& operator/= (const X& rhs)
            {
                fAbs = fAbs / double(rhs);
                return *this;
            }

        public:
            complexpolar<double>& conj()
            {
                fArg = -fArg;
                return *this;
            }

        public:
            double abs() const;
            double arg() const;

        private:
            double fAbs;
            double fArg;
    };

    inline double complexpolar<double>::abs() const
    {
        return fAbs;
    }
    inline double complexpolar<double>::arg() const
    {
        return fArg;
    }



    template<>
    class complexpolar<long double>
    {
            // typedefs
        public:
            typedef long double value_type;
            typedef std::complex<long double> std_complex_type;

            // constructors (and destructor)
        public:
            complexpolar(const long double& abs = 0., const long double& arg = 0.);
            complexpolar(const complexpolar<long double>& cmplxp);
            complexpolar(const std_complex_type& cmplx);
            template< class X >
            complexpolar(const complexpolar<X>& cmplxp) :
                    fAbs((long double)(cmplxp.fAbs)), fArg((long double)(cmplxp.fArg))
            {}
            ~complexpolar();

            // set functions
            void set_polar(const long double& abs, const long double& arg)
            {
                fAbs = abs;
                fArg = arg;
                return;
            }

            void set_rect(const long double& real, const long double& imag)
            {
                fAbs = std::sqrt(real*real + imag*imag);
                fArg = std::atan2(imag, real);
                return;
            }

            // operators
        public:
            complexpolar& operator= (const complexpolar& rhs);

            template<class X>
            complexpolar<long double>& operator=  (const complexpolar<X>& rhs)
            {
                fAbs = (long double)(rhs.fAbs);
                fArg = (long double)(rhs.fArg);
                return *this;
            }

            template<class X>
            complexpolar<long double>& operator+= (const complexpolar<X>& rhs)
            {
                long double sumx = fAbs * std::cos(fArg) + rhs.fAbs * std::cos(rhs.fArg);
                long double sumy = fAbs * std::sin(fArg) + rhs.fAbs * std::sin(rhs.fArg);
                fAbs = std::sqrt(sumx*sumx + sumy*sumy);
                fArg = std::atan2(sumy, sumx);
                return *this;
            }

            template<class X>
            complexpolar<long double>& operator-= (const complexpolar<X>& rhs)
            {
                long double diffx = fAbs * std::cos(fArg) - rhs.fAbs * std::cos(rhs.fArg);
                long double diffy = fAbs * std::sin(fArg) - rhs.fAbs * std::sin(rhs.fArg);
                fAbs = std::sqrt(diffx*diffx + diffy*diffy);
                fArg = std::atan2(diffy, diffx);
                return *this;
            }

            template<class X>
            complexpolar<long double>& operator*= (const complexpolar<X>& rhs)
            {
                fAbs = fAbs * (long double)(rhs.fAbs);
                fArg = fArg + (long double)(rhs.fArg);
                return *this;
            }

            template<class X>
            complexpolar<long double>& operator/= (const complexpolar<X>& rhs)
            {
                fAbs = fAbs / (long double)(rhs.fAbs);
                fArg = fArg - (long double)(rhs.fArg);
                return *this;
            }

            complexpolar& operator= (const std_complex_type& rhs);

            template<class X>
            complexpolar<long double>& operator=  (const std::complex<X>& rhs)
            {
                fAbs = (long double)(std::abs(rhs));
                fArg = (long double)(std::arg(rhs));
                return *this;
            }

            template<class X>
            complexpolar<long double>& operator+= (const std::complex<X>& rhs)
            {
                long double sumx = fAbs * std::cos(fArg) + rhs.real();
                long double sumy = fAbs * std::sin(fArg) + rhs.imag();
                fAbs = std::sqrt(sumx*sumx + sumy*sumy);
                fArg = std::atan2(sumy, sumx);
                return *this;
            }

            template<class X>
            complexpolar<long double>& operator-= (const std::complex<X>& rhs)
            {
                long double diffx = fAbs * std::cos(fArg) - rhs.real();
                long double diffy = fAbs * std::sin(fArg) - rhs.imag();
                fAbs = std::sqrt(diffx*diffx + diffy*diffy);
                fArg = std::atan2(diffy, diffx);
                return *this;
            }

            template<class X>
            complexpolar<long double>& operator*= (const std::complex<X>& rhs)
            {
                fAbs = fAbs * (long double)(std::abs(rhs));
                fArg = fArg + (long double)(std::arg(rhs));
                return *this;
            }

            template<class X>
            complexpolar<long double>& operator/= (const std::complex<X>& rhs)
            {
                fAbs = fAbs / (long double)(std::abs(rhs));
                fArg = fArg - (long double)(std::arg(rhs));
                return *this;
            }

            template<class X>
            complexpolar<long double>& operator=  (const X& rhs)
            {
                fAbs = (long double)(rhs);
                fArg = 0.;
                return *this;
            }

            template<class X>
            complexpolar<long double>& operator+= (const X& rhs)
            {
                long double sumx = fAbs * std::cos(fArg) + (long double)(rhs);
                long double sumy = fAbs * std::sin(fArg);
                fAbs = std::sqrt(sumx*sumx + sumy*sumy);
                fArg = std::atan2(sumy, sumx);
                return *this;
            }

            template<class X>
            complexpolar<long double>& operator-= (const X& rhs)
            {
                long double diffx = fAbs * std::cos(fArg) - (long double)(rhs);
                long double diffy = fAbs * std::sin(fArg);
                fAbs = std::sqrt(diffx*diffx + diffy*diffy);
                fArg = std::atan2(diffy, diffx);
                return *this;
            }

            template<class X>
            complexpolar<long double>& operator*= (const X& rhs)
            {
                fAbs = fAbs * (long double)(rhs);
                return *this;
            }

            template<class X>
            complexpolar<long double>& operator/= (const X& rhs)
            {
                fAbs = fAbs / (long double)(rhs);
                return *this;
            }

        public:
            complexpolar<long double>& conj()
            {
                fArg = -fArg;
                return *this;
            }

        public:
            long double abs() const;
            long double arg() const;

        private:
            long double fAbs;
            long double fArg;
    };

    inline long double complexpolar<long double>::abs() const
    {
        return fAbs;
    }
    inline long double complexpolar<long double>::arg() const
    {
        return fArg;
    }



} /* namespace Katydid */
#endif /* COMPLEXPOLAR_SPECIALIZATION_HH_ */
