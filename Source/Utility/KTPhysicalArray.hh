/*
 * KTPhysicalArray.hh
 *
 *  Created on: Jan 10, 2012
 *      Author: nsoblath
 */

#ifndef KTPHYSICALARRAY_HH_
#define KTPHYSICALARRAY_HH_

#include "KTAxisProperties.hh"

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/bind.hpp>

#include <iostream>

namespace Katydid
{
    //*******************************
    // general, dummy, implementation
    //*******************************

    template< size_t NDims, typename XDataType >
    class KTPhysicalArray : public boost::numeric::ublas::vector< XDataType >, public KTAxisProperties< NDims >
    {
        private:
            typedef boost::numeric::ublas::vector< XDataType > XVectorType;

        public:
            KTPhysicalArray();
            ~KTPhysicalArray();

        public:
            KTPhysicalArray< NDims, XDataType >& operator/=(const KTPhysicalArray< NDims, XDataType >& rhs);
    };

    template< size_t NDims, typename XDataType >
    KTPhysicalArray< NDims, XDataType >::KTPhysicalArray() :
            XVectorType(),
            KTAxisProperties< NDims >()
    {
        SetNBinsFunc(new KTNBinsInArray< NDims, XVectorType >(this, &XVectorType::size));
        std::cout << "Warning from KTPhysicalArray: " << NDims << " dimensional arrays are not supported." << std::endl;
        std::cout << "This is an instance of a dummy object." << std::endl;
    }

    template< size_t NDims, typename XDataType >
    KTPhysicalArray< NDims, XDataType >::~KTPhysicalArray()
    {
    }

    template< size_t NDims, typename XDataType >
    KTPhysicalArray< NDims, XDataType >& KTPhysicalArray< NDims, XDataType >::operator/=(const KTPhysicalArray< NDims, XDataType>& /*rhs*/)
    {
        return *this;
    }

    //*************************
    // 1-D array implementation
    //*************************

    template< typename XDataType >
    class KTPhysicalArray< 1, XDataType > : public boost::numeric::ublas::vector< XDataType >, public KTAxisProperties< 1 >
    {
        private:
            typedef boost::numeric::ublas::vector< XDataType > XVectorType;
            typedef KTNBinsInArray< 1, XVectorType > XNBinsFunctor;

        public:
            KTPhysicalArray();
            KTPhysicalArray(size_t nBins, Double_t rangeMin=0., Double_t rangeMax=1.);
            ~KTPhysicalArray();

        public:
            KTPhysicalArray< 1, XDataType >& operator/=(const KTPhysicalArray< 1, XDataType >& rhs);
    };

    template< typename XDataType >
    KTPhysicalArray< 1, XDataType >::KTPhysicalArray() :
            XVectorType(),
            KTAxisProperties< 1 >()
    {
        SetNBinsFunc(new KTNBinsInArray< 1, XVectorType >(this, &XVectorType::size));
        std::cout << "You have created a 1-D physical array" << std::endl;
    }

    template< typename XDataType >
    KTPhysicalArray< 1, XDataType >::KTPhysicalArray(size_t nBins, Double_t rangeMin, Double_t rangeMax) :
            XVectorType(nBins),
            KTAxisProperties< 1 >(rangeMin, rangeMax)
    {
        SetNBinsFunc(new KTNBinsInArray< 1, XVectorType >((XVectorType*)this, &XVectorType::size));
        //SetNBinsFunc(KTNBinsInArray< 1, XVectorType >(this, &XVectorType::size));
        //SetNBinsFunc(boost::bind(&KTNBinsInArray< 1, XVectorType >::operator(), boost::ref(fNBinsFunc)));
        std::cout << "You have created a 1-D physical array with " << nBins << " bins, going from " << rangeMin << " to " << rangeMax << std::endl;
    }

    template< typename XDataType >
    KTPhysicalArray< 1, XDataType >::~KTPhysicalArray()
    {
    }

    template< typename XDataType >
    KTPhysicalArray< 1, XDataType >& KTPhysicalArray< 1, XDataType >::operator/=(const KTPhysicalArray< 1, XDataType>& rhs)
    {
        if (rhs.size() != this->size()) return *this;
        for (size_t iBin=0; iBin<this->size(); iBin++)
        {
            (*this)[iBin] /= rhs[iBin];
        }
        return *this;
    }

} /* namespace Katydid */
#endif /* KTPHYSICALARRAY_HH_ */
