/*
 * TestVector.cc
 *
 *  Created on: Jan 10, 2012
 *      Author: nsoblath
 */

#include "KTPhysicalArray.hh"

using namespace Katydid;

int main ()
{
    // 1D physical array test

    KTPhysicalArray< 1, double > v1(3, 0., 1.), v2(3, 0., 1.);

    for (unsigned i = 0; i < std::min(v1.size(), v2.size()); i++)
    {
        v1(i) = v2(i) = i+1;
    }

    std::cout << v1 + v2 << std::endl;
    std::cout << v1 - v2 << std::endl;
    v1 /= v2;
    std::cout << v1 << std::endl;

    // 2D physical array test

    KTPhysicalArray< 2, double > m1(5, -1., 2., 10, 15., 20.);
    KTPhysicalArray< 2, double > m2(5, -1., 2., 10, 15., 20.);

    std::cout << m1.size(1) << "  " << m1.size(2) << std::endl;
    std::cout << m2.size(1) << "  " << m2.size(2) << std::endl;

    for (unsigned i = 0; i < std::min (m1.size(1), m2.size(1)); i++)
    {
        std::cout << "i=" << i << "  m1.size1=" << m1.size(1) << std::endl;
        for (unsigned j = 0; j < std::min(m1.size(2), m2.size(2)); j++)
        {
            std::cout << "   j=" << j << "  m1.size2=" << m1.size(2) << std::endl;
            m1(i, j) = m2(i, j) = i+j+1;
        }
    }

    std::cout << m1 << std::endl;
    std::cout << m2 << std::endl;
    KTPhysicalArray< 2, double > m3(m1);
    std::cout << m3 << std::endl;
    std::cout << m1 + m2 << std::endl;
    std::cout << m1 - m2 << std::endl;
    m1 /= m2;
    std::cout << m1 << std::endl;

    return 0;
}
