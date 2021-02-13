/*
 * KTPhysicalArrayComplex.hh
 *
 *  Created on: Feb 05, 2021
 *      Author: F. Thomas
 */

#include "KTPhysicalArrayComplex.hh"

namespace Katydid
{
    
    using value_type = std::complex<double>;
    using array_type = Eigen::Array< value_type, Eigen::Dynamic, 1, Eigen::ColMajor >;
    using matrix_type = Eigen::Array< value_type, Eigen::Dynamic, Eigen::Dynamic, Eigen::ColMajor >;

    //*******************************
    // 1D implementation
    //*******************************

    KTPhysicalArray< 1, value_type >::KTPhysicalArray() :
            KTAxisProperties< 1 >(),
            fData(),
            fLabel()
    {
        SetNBinsFunc(new KTNBinsInArray< 1, FixedSize >(0));
    }

    KTPhysicalArray< 1, value_type >::KTPhysicalArray(size_t nBins, double rangeMin, double rangeMax) :
            KTAxisProperties< 1 >(rangeMin, rangeMax, new KTNBinsInArray< 1, FixedSize >(nBins)),
            fData(nBins),
            fLabel()
    {
    }

    KTPhysicalArray< 1, value_type >::KTPhysicalArray(value_type value, size_t nBins, double rangeMin, double rangeMax) :
            KTPhysicalArray< 1, value_type >(nBins, rangeMin, rangeMax)
    {
        fData.fill(value);
    }

    KTPhysicalArray< 1, value_type >::~KTPhysicalArray()
    {}
    
    
    //*******************************
    // 2D implementation
    //*******************************

    
    KTPhysicalArray< 2, std::complex<double> >::KTPhysicalArray() :
            KTAxisProperties< 2 >(),
            fData(),
            fLabel()
    {
        //Is that what you had to do in the pre-lambda era?
        //please let's work on the use of this function pointer
        //I also don't like the 'new' keyword either
        size_t (KTPhysicalArray< 2, std::complex<double> >::*sizeArray[2])() const = {&KTPhysicalArray< 2, std::complex<double> >::rows, &KTPhysicalArray< 2, std::complex<double> >::cols};
        SetNBinsFunc(new KTNBinsInArray< 2, KTPhysicalArray< 2, std::complex<double> > >(this, sizeArray));
        //std::cout << "You have created a 2-D physical array" << std::endl;
    }
    
    KTPhysicalArray< 2, std::complex<double> >::KTPhysicalArray(size_t xNBins, double xRangeMin, double xRangeMax, size_t yNBins, double yRangeMin, double yRangeMax) :
            KTAxisProperties< 2 >(),
            fData(xNBins, yNBins),
            fLabel()
    {

        size_t (KTPhysicalArray< 2, std::complex<double> >::*sizeArray[2])() const = {&KTPhysicalArray< 2, std::complex<double> >::rows, &KTPhysicalArray< 2, std::complex<double> >::cols};
        SetNBinsFunc(new KTNBinsInArray< 2, KTPhysicalArray< 2, std::complex<double> > >(this, sizeArray));
        SetRangeMin(1, xRangeMin);
        SetRangeMin(2, yRangeMin);
        SetRangeMax(1, xRangeMax);
        SetRangeMax(2, yRangeMax);

    }


    KTPhysicalArray< 2, std::complex<double> >::KTPhysicalArray(std::complex<double> value, size_t xNBins, double xRangeMin, double xRangeMax, size_t yNBins, double yRangeMin, double yRangeMax) :
            KTPhysicalArray(xNBins, xRangeMin, xRangeMax, yNBins, yRangeMin, yRangeMax)
    {
        fData.fill(value);
    }


    KTPhysicalArray< 2, std::complex<double> >::~KTPhysicalArray()
    {
    }

} /* namespace Katydid */
