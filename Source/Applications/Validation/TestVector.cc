/*
 * TestVector.cc
 *
 *  Created on: Jan 10, 2012
 *      Author: nsoblath
 */

#include "KTPhysicalArray.hh"

using namespace Katydid;
using namespace boost::numeric::ublas;

int main ()
{
    KTPhysicalArray< 1, double > v1 (3, 0., 1.), v2 (3, 0., 1.);

    for (unsigned i = 0; i < std::min (v1.size (), v2.size ()); ++ i)
    {
        v1 (i) = v2 (i) = i;
    }

    std::cout << v1 + v2 << std::endl;
    std::cout << v1 - v2 << std::endl;

    return 0;
}
