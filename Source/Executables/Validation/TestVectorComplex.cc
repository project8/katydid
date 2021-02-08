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
using MatrixType = KTPhysicalArray< 2, Complex >;

template <typename ContainerType1, typename ContainerType2, typename f_type>
void operation(ContainerType1& v1, ContainerType2& v2, f_type f)
{
    f(v1, v2);
    std::cout << "v1: " << '\n' << v1 << '\n' << std::endl;
    std::cout << "v2: " << '\n' << v2 << '\n' << std::endl;
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

    operation( v1, v2, [](ArrayType& v1, ArrayType& v2){ std::cout << "Sum" << '\n' << v1 + v2 << '\n' << std::endl;});
    
    operation( v1, v2, [](ArrayType& v1, ArrayType& v2){ std::cout << "Diff" << '\n' << v1 - v2 << '\n' << std::endl;});
    
    operation( v1, v2, [](ArrayType& v1, ArrayType& v2){ std::cout << "Prod" << '\n' << v1 * v2 << '\n' << std::endl;});
    
    operation( v1, v2, [](ArrayType& v1, ArrayType& v2){ std::cout << "Frac" << '\n' << v1 / v2 << '\n' << std::endl;});
    
    operation( v1, v2, [](ArrayType& v1, ArrayType& v2){ std::cout << "+=" <<'\n'; v1 += v2;});
    
    operation( v1, v2, [](ArrayType& v1, ArrayType& v2){ std::cout << "-=" <<'\n'; v1 -= v2;});
    
    operation( v1, v2, [](ArrayType& v1, ArrayType& v2){ std::cout << "*=" <<'\n'; v1 *= v2;});
    
    operation( v1, v2, [](ArrayType& v1, ArrayType& v2){ std::cout << "/=" <<'\n'; v1 /= v2;});
    
    std::cout << "For-each-loop\n";
    for (auto && a: v1)
    {
        std::cout << a << " ";
    }

    std::cout << "\n\n";

    // 2D physical array test

    MatrixType m1(5, -1., 2., 10, 15., 20.);
    MatrixType m2(5, -1., 2., 10, 15., 20.);

    std::cout << m1.size(1) << "  " << m1.size(2) << std::endl;
    std::cout << m2.size(1) << "  " << m2.size(2) << std::endl;


    operation( m1, m2, [](MatrixType& m1, MatrixType& m2){
        for (unsigned i = 0; i < std::min (m1.size(1), m2.size(1)); i++)
        {
            std::cout << "i=" << i << "  m1.size1=" << m1.size(1) << std::endl;
            for (unsigned j = 0; j < std::min(m1.size(2), m2.size(2)); j++)
            {
                std::cout << "   j=" << j << "  m1.size2=" << m1.size(2) << std::endl;
                double id = i;
                double jd = j;
                m1(i, j) = m2(i, j) =Complex{id+jd+1, id-jd-1} ;
            }
        }});
    
    std::cout << '\n';
    
    operation( m1, m2, [](MatrixType& m1, MatrixType& m2){ std::cout << "Sum" << '\n' << m1 + m2 << '\n' << std::endl;});
                   
    operation( m1, m2, [](MatrixType& m1, MatrixType& m2){ std::cout << "Diff" << '\n' << m1 - m2 << '\n' << std::endl;});
                   
    operation( m1, m2, [](MatrixType& m1, MatrixType& m2){ std::cout << "Prod" << '\n' << m1 * m2 << '\n' << std::endl;});
                   
    operation( m1, m2, [](MatrixType& m1, MatrixType& m2){ std::cout << "Frac" << '\n' << m1 / m2 << '\n' << std::endl;});
                   
    operation( m1, m2, [](MatrixType& m1, MatrixType& m2){ std::cout << "+=" <<'\n'; m1 += m2;});
                   
    operation( m1, m2, [](MatrixType& m1, MatrixType& m2){ std::cout << "-=" <<'\n'; m1 -= m2;});
                   
    operation( m1, m2, [](MatrixType& m1, MatrixType& m2){ std::cout << "*=" <<'\n'; m1 *= m2;});
                   
    operation( m1, m2, [](MatrixType& m1, MatrixType& m2){ std::cout << "/=" <<'\n'; m1 /= m2;});


    MatrixType m3(m1);
    operation( m1, m3, [](MatrixType& m1, MatrixType& m3){ std::cout << "Sum copy" << '\n' << m1 + m3 << '\n' << std::endl;});
    
    std::cout << "Test iterators " << '\n';
    for(auto it1 = m1.begin1(); it1 != m1.end1(); ++it1) {
        std::cout << " outer " << std::endl;
        for(auto it2 = it1.begin(); it2 !=it1.end(); ++it2) {
          std::cout << *it2 << ' ';
        }
        std::cout << std::endl;
    }
    
    unsigned xMax, yMax, xMin, yMin;
    
    std::cout << '\n';
    std::cout << "Maximum: " << m1.GetMaximumBin(xMax, yMax) << " at (" << xMax << "," << yMax << ")\n";
    std::cout << "Minimum: " << m1.GetMinimumBin(xMin, yMin) << " at (" << xMin << "," << yMin << ")\n";
    auto minmax = m1.GetMinMaxBin(xMin, yMin, xMax, yMax);
    std::cout << "Min: " <<  minmax.first << " at (" << xMin << "," << yMin << ") Max: " << minmax.second << " at (" << xMax << "," << yMax << ")\n";

    MatrixType m4(5, -1., 2., 10, 15., 20.);
    MatrixType m5(10, -1., 2., 5, 15., 20.);
    ArrayType v3(5, 0., 1.);
    
    for (unsigned i = 0; i < v3.size(); i++)
    {
        v3(i) = Complex{static_cast<double>(i)+1, static_cast<double>(i)-1};
    }
    
    for (unsigned i = 0; i < m4.size(1); i++)
    {
        std::cout << "i=" << i << "  m1.size1=" << m4.size(1) << std::endl;
        for (unsigned j = 0; j < m4.size(2); j++)
        {
            std::cout << "   j=" << j << "  m1.size2=" << m4.size(2) << std::endl;
            double id = i;
            double jd = j;
            m4(i, j) = Complex{id+jd+1, id-jd-1} ;
            m5(j, i) = Complex{id+jd+1, id-jd-1} ;
        }
    }
    
    std::cout << v3 << '\n';
    std::cout << m4 << '\n';
    std::cout << m5 << '\n';
    
    operation( m4, m5, [](MatrixType& m4, MatrixType& m5){ std::cout << "Matmul" << '\n' << m4 % m5 << '\n' << std::endl;});
    operation( m4, m5, [](MatrixType& m4, MatrixType& m5){ std::cout << "%=" << '\n'; m4 %= m5;});
    
    std::cout << "Size product: " << m4.size(1) << " " << m4.size(2) << '\n' << std::endl;

    operation( m4, v3, [](MatrixType& m4, ArrayType& v3){ std::cout << "Mat-vec-mul" << '\n' << m4 % v3 << '\n' << std::endl;});

    return 0;
}
