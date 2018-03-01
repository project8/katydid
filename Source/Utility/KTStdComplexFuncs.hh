/*
 * KTStdComplexFuncs.hh
 *
 *  Created on: Feb 25, 2018
 *      Author: obla999
 */

#include "complexpolar.hh"

#include <fftw3.h>

#include <complex>

namespace Katydid
{
    /// Addition-assignment for fftw_complex and std::complex<double>
    std::complex<double> operator+=(std::complex<double> lhs, const fftw_complex& rhs)
    {
        lhs.real(lhs.real() + rhs[0]);
        lhs.imag(lhs.imag() + rhs[1]);
        return lhs;
    }

    /// Subtraction-assignment for fftw_complex and std::complex<double>
    std::complex<double> operator-=(std::complex<double> lhs, const fftw_complex& rhs)
    {
        lhs.real(lhs.real() - rhs[0]);
        lhs.imag(lhs.imag() - rhs[1]);
        return lhs;
    }

    /// Copies the values from an fftw_complex to a std::complex<double>
    void Assign(std::complex<double>& lhs, const fftw_complex& rhs)
    {
        lhs.real(rhs[0]);
        lhs.imag(rhs[1]);
        return;
    }

    /// Addition-assignment for complexpolar<double> and std::complex<double>
    std::complex<double> operator+=(std::complex<double> lhs, const complexpolar<double>& rhs)
    {
        lhs.real(lhs.real() + real(rhs));
        lhs.imag(lhs.imag() + imag(rhs));
        return lhs;
    }

    /// Subtraction-assigment for complexpolar<double> and std::complex<double>
    std::complex<double> operator-=(std::complex<double> lhs, const complexpolar<double>& rhs)
    {
        lhs.real(lhs.real() - real(rhs));
        lhs.imag(lhs.imag() - imag(rhs));
        return lhs;
    }

    /// Copies the values from a complexpolar<double> to a std::complex<double>
    void Assign(std::complex<double>& lhs, const complexpolar<double>& rhs)
    {
        lhs.real(real(rhs));
        lhs.imag(imag(rhs));
        return;
    }
}
