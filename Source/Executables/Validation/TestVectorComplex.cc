/*
 * TestVectorEigen.cc
 *
 *  Created on: Feb 05, 2021
 *      Author: F. Thomas
 */

#include "KTPhysicalArrayComplex.hh"

using namespace Katydid;
using Complex = std::complex<double>;
using ArrayType = KTPhysicalArray< 1, Complex >;

template <typename f_type>
void operation(ArrayType& v1, ArrayType& v2, f_type f)
{
    f(v1, v2);
    std::cout << v1 << '\n' << std::endl;
    std::cout << v2 << '\n' << std::endl;
}

int main ()
{

    
    // 1D physical array test

    ArrayType v1(3, 0., 1.), v2(3, 0., 1.);
    
    operation( v1, v2, [](ArrayType& v1, ArrayType& v2){
                for (unsigned i = 0; i < std::min(v1.size(), v2.size()); i++)
                {
                    v1(i) = v2(i) = Complex{static_cast<double>(i)+1, static_cast<double>(i)-1};
                }
        } );

    operation( v1, v2, [](ArrayType& v1, ArrayType& v2){ std::cout << v1 + v2 << '\n' << std::endl;});
    
    operation( v1, v2, [](ArrayType& v1, ArrayType& v2){ std::cout << v1 - v2 << '\n' << std::endl;});
    
    operation( v1, v2, [](ArrayType& v1, ArrayType& v2){ std::cout << v1 * v2 << '\n' << std::endl;});
    
    operation( v1, v2, [](ArrayType& v1, ArrayType& v2){ std::cout << v1 / v2 << '\n' << std::endl;});
    
    operation( v1, v2, [](ArrayType& v1, ArrayType& v2){ v1 += v2;});
    
    operation( v1, v2, [](ArrayType& v1, ArrayType& v2){ v1 -= v2;});
    
    operation( v1, v2, [](ArrayType& v1, ArrayType& v2){ v1 *= v2;});
    
    operation( v1, v2, [](ArrayType& v1, ArrayType& v2){ v1 /= v2;});
    
    for (auto && a: v1)
    {
        std::cout << a << " ";
    }

    std::cout << '\n';

    // 2D physical array test
/*
    KTPhysicalArray< 2, complex > m1(5, -1., 2., 10, 15., 20.);
    KTPhysicalArray< 2, complex > m2(5, -1., 2., 10, 15., 20.);

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
    KTPhysicalArray< 2, complex > m3(m1);
    std::cout << m3 << std::endl;
    std::cout << m1 + m2 << std::endl;
    std::cout << m1 - m2 << std::endl;
    m1 /= m2;
    std::cout << m1 << std::endl;
*/
    return 0;
}
