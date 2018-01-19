/*
 * complexpolar_specialization.cc
 *
 *  Created on: Aug 23, 2012
 *      Author: nsoblath
 */

#include "complexpolar/specialization.hh"

namespace Katydid
{
    complexpolar<float>::complexpolar(const float& abs, const float& arg) :
                            fAbs(abs), fArg(arg)
    {}
    complexpolar<float>::complexpolar(const complexpolar<float>& cmplxp) :
                            fAbs(cmplxp.fAbs), fArg(cmplxp.fArg)
    {}
    complexpolar<float>::complexpolar(const std_complex_type& cmplx) :
                            fAbs(std::abs(cmplx)),
                            fArg(std::arg(cmplx))
    {}
    complexpolar<float>::~complexpolar()
    {}

    complexpolar<float>& complexpolar<float>::operator= (const complexpolar& rhs)
    {
        fAbs = rhs.fAbs;
        fArg = rhs.fArg;
        return *this;
    }


    complexpolar<float>& complexpolar<float>::operator= (const std_complex_type& rhs)
    {
        fAbs = std::abs(rhs);
        fArg = std::arg(rhs);
        return *this;
    }


    complexpolar<double>::complexpolar(const double& abs, const double& arg) :
                            fAbs(abs), fArg(arg)
    {}
    complexpolar<double>::complexpolar(const complexpolar<double>& cmplxp) :
                            fAbs(cmplxp.fAbs), fArg(cmplxp.fArg)
    {}
    complexpolar<double>::complexpolar(const std_complex_type& cmplx) :
                            fAbs(std::abs(cmplx)),
                            fArg(std::arg(cmplx))
    {}
    complexpolar<double>::~complexpolar()
    {}

    complexpolar<double>& complexpolar<double>::operator= (const complexpolar& rhs)
    {
        fAbs = rhs.fAbs;
        fArg = rhs.fArg;
        return *this;
    }

    complexpolar<double>& complexpolar<double>::operator= (const std_complex_type& rhs)
    {
        fAbs = std::abs(rhs);
        fArg = std::arg(rhs);
        return *this;
    }




    complexpolar<long double>::complexpolar(const long double& abs, const long double& arg) :
                            fAbs(abs),
                            fArg(arg)
    {}
    complexpolar<long double>::complexpolar(const complexpolar<long double>& cmplxp) :
                            fAbs(cmplxp.fAbs),
                            fArg(cmplxp.fArg)
    {}
    complexpolar<long double>::complexpolar(const std_complex_type& cmplx) :
                            fAbs(std::abs(cmplx)),
                            fArg(std::arg(cmplx))
    {}
    complexpolar<long double>::~complexpolar()
    {}

    complexpolar<long double>& complexpolar<long double>::operator= (const complexpolar& rhs)
    {
        fAbs = rhs.fAbs;
        fArg = rhs.fArg;
        return *this;
    }

    complexpolar<long double>& complexpolar<long double>::operator= (const std_complex_type& rhs)
    {
        fAbs = std::abs(rhs);
        fArg = std::arg(rhs);
        return *this;
    }


} /* namespace Katydid */
