/*
 * TestComplexPolar.cc
 *
 *  Created on: Aug 23, 2012
 *      Author: nsoblath
 */

#include "complexpolar.hh"

#include <complex>
#include <iostream>

using namespace Katydid;
using namespace std;

int main()
{
    cout << "Testing contruction and member functions" << endl;
    complexpolar<double> cp1(1.,2.), cp2(2., 3.);
    cout << "cp1: " << cp1 << endl;
    cout << "cp2: " << cp2 << endl;

    // member operators

    // conj
    cout << "cp2: " << cp2 << endl;
    cp2.conj();
    cout << "after cp2.conj: " << cp2 << endl;
    cout << "conj(cp2): " << conj(cp2) << endl;
    cout << "cp2: " << cp2 << endl;

    cout << endl;

    // operator=
    complexpolar<double> cp3(0.,0.);
    cp3 = cp1;
    cout << "cp3 = cp1: " << cp3 << endl;

    // operators +=, -=, *=, /=
    cp3 += cp1;
    cout << "cp3 += cp1: " << cp3 << endl;
    cp3 -= cp1;
    cout << "cp3 -= cp1: " << cp3 << endl;
    cp3 *= cp1;
    cout << "cp3 *= cp1: " << cp3 << endl;
    cp3 /= cp1;
    cout << "cp3 /= cp1: " << cp3 << endl;

    // non-member operators

    cout << endl;
    cout << "Testing non-member functions" << endl;

    cout << "cp1: " << cp1 << "   cp2: " << cp2 << "   cp3: " << cp3 << endl;

    // operators ==, !=
    cout << "cp1 == cp3: " << bool(cp1 == cp3) << endl;
    cout << "cp1 != cp3: " << bool(cp1 != cp3) << endl;
    cout << "cp1 == cp2: " << bool(cp1 == cp2) << endl;
    cout << "cp1 != cp2: " << bool(cp1 != cp2) << endl;

    // operators +, -, /, *
    cout << "cp1 + cp3: " << cp1 + cp3 << endl;
    cout << "cp1 - cp3: " << cp1 - cp3 << endl;
    cout << "cp1 * cp3: " << cp1 * cp3 << endl;
    cout << "cp1 / cp3: " << cp1 / cp3 << endl;

    cout << endl;
    cout << "Testing non-member partially-real functions" << endl;

    complexpolar<double> cp7(4.,0.);
    double d7 = 4.;
    cout << "cp7: " << cp7 << "   d7: " << d7 << endl;
    cout << "cp2: " << cp2 << endl;

    // operators ==, !=
    cout << "cp7 == d7: " << bool(cp7 == d7) << endl;
    cout << "d7 == cp2: " << bool(d7 == cp7) << endl;
    cout << "cp7 != d7: " << bool(cp7 != d7) << endl;
    cout << "d7 != cp2: " << bool(d7 != cp7) << endl;
    cout << "cp2 == d7: " << bool(cp2 == d7) << endl;
    cout << "d7 == cp2: " << bool(d7 == cp7) << endl;
    cout << "cp2 != d7: " << bool(cp2 != d7) << endl;
    cout << "d7 != cp2: " << bool(d7 != cp7) << endl;

    // operators +, -, /, *
    cout << "cp2 + d7: " << cp2 + d7 << endl;
    cout << "d7 + cp2: " << d7 + cp2 << endl;
    cout << "cp2 - d7: " << cp2 - d7 << endl;
    cout << "d7 - cp2: " << d7 - cp2 << endl;
    cout << "cp2 * d7: " << cp2 * d7 << endl;
    cout << "d7 * cp2: " << d7 * cp2 << endl;
    cout << "cp2 / d7: " << cp2 / d7 << endl;
    cout << "d7 / cp2: " << d7 / cp2 << endl;


    // compatibility with std::complex

    cout << endl;
    cout << "Testing compatibility with std::complex" << endl;

    complex<double> c4(3.,3.), c5(1.,4.);
    cout << "c4: " << c4 << "   c5: " << c5 << endl;

    complexpolar<double> cp4(c4);
    cout << "abs(c4), arg(c4): " << abs(c4) << ", " << arg(c4) << endl;
    cout << "cp4 (=c4): " << cp4 << endl;

    complexpolar<double> cp5(0.,0.);
    cp5 = c5;
    cout << "abs(c5), arg(c5): " << abs(c5) << ", " << arg(c5) << endl;
    cout << "cp5 = c5: " << cp5 << endl;

    complexpolar<double> cp8(cp4);
    cout << "cp8 (=cp4): " << cp8 << endl;

    cout << endl;

    // operators +=, -=, *=, /=
    cp4 += cp5;
    c4 += c5;
    cp8 += c5;
    cout << "cp4 += cp5: " << cp4 << endl;
    cout << "c4 += c5; abs, arg: " << abs(c4) << ", " << arg(c4) << endl;
    cout << "cp8 += c5: " << cp8 << endl;
    cp4 -= cp5;
    c4 -= c5;
    cp8 -= c5;
    cout << "cp4 -= cp5: " << cp4 << endl;
    cout << "c4 -= c5; abs, arg: " << abs(c4) << ", " << arg(c4) << endl;
    cout << "cp8 -= c5: " << cp8 << endl;
    cp4 *= cp5;
    c4 *= c5;
    cp8 *= c5;
    cout << "cp4 *= cp5: " << cp4 << endl;
    cout << "c4 *= c5; abs, arg: " << abs(c4) << ", " << arg(c4) << endl;
    cout << "cp8 *= c5: " << cp8 << endl;
    cp4 /= cp5;
    c4 /= c5;
    cp8 /= c5;
    cout << "cp4 /= cp5: " << cp4 << endl;
    cout << "c4 /= c5; abs, arg: " << abs(c4) << ", " << arg(c4) << endl;
    cout << "cp8 /= c5: " << cp8 << endl;

    cout << endl;

    // mixed operators ==, !=
    complex<double> c9(10.,18.);
    complexpolar<double> cp9(c9);
    cout << "cp9: " << cp9 << endl;
    cout << "c9; abs, arg: " << abs(c9) << ", " << arg(c9) << endl;
    cout << "cp9 == c9: " << bool(cp9 == c9) << endl;
    cout << "c9 == cp9: " << bool(c9 == cp9) << endl;
    cout << "cp9 != c9: " << bool(cp9 != c9) << endl;
    cout << "c9 != cp9: " << bool(c9 != cp9) << endl;

    cout << endl;

    // mixed operators +, -, *, /
    complex<double> c7(8., 2.);
    cout << "cp2: " << cp2 << endl;
    cout << "c7; abs, arg: " << abs(c7) << ", " << arg(c7) << endl;
    cout << "cp2 + c7: " << cp2 + c7 << endl;
    cout << "c7 + cp2: " << complexpolar<double>(c7 + cp2) << endl;
    cout << "cp2 - c7: " << cp2 - c7 << endl;
    cout << "c7 - cp2: " << complexpolar<double>(c7 - cp2) << endl;
    cout << "cp2 * c7: " << cp2 * c7 << endl;
    cout << "c7 * cp2: " << complexpolar<double>(c7 * cp2) << endl;
    cout << "cp2 / c7: " << cp2 / c7 << endl;
    cout << "c7 / cp2: " << complexpolar<double>(c7 / cp2) << endl;

    cout << endl;

    // external functions
    cout << "Testing external functions" << endl;
    cout << "cp9: " << cp9 << endl;
    cout << "c9; abs, arg: " << abs(c9) << ", " << arg(c9) << endl;

    cout << endl;

    cout << "cp9; real, imag: " << real(cp9) << ", " << imag(cp9) << endl;
    cout << "c9: " << c9 << endl;

    cout << endl;

    cout << "cp9: " << cp9 << endl;
    cout << "rect(c9): " << rect(c9) << endl;
    cout << "rect(c9.real(), c9.imag()): " << rect(c9.real(), c9.imag()) << endl;
    cout << "complexpolar<double>(c9): " << complexpolar<double>(c9) << endl;

    cout << endl;

    cout << "c9: " << c9 << endl;
    cout << "polar(cp9): " << polar(cp9) << endl;
    cout << "polar(cp9.abs(), cp9.arg()): " << polar(cp9.abs(), cp9.arg()) << endl;

    cout << endl;

    cout << "norm(c9): " << norm(c9) << endl;
    cout << "norm(cp9): " << norm(cp9) << endl;

    cout << endl;

    cout << "conj(c9); abs, arg: " << abs(conj(c9)) << ", " << arg(conj(c9)) << endl;
    cout << "conj(cp9): " << conj(cp9) << endl;

    return 0;
}

