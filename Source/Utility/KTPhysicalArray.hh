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
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/bind.hpp>

namespace Katydid
{
    KTLOGGER(utillog_physarr, "katydid.utility");

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
            KTPhysicalArray(const KTPhysicalArray< NDims, XDataType >& orig);
            virtual ~KTPhysicalArray();

        public:
            KTPhysicalArray< NDims, XDataType >& operator=(const KTPhysicalArray< NDims, XDataType >& rhs);
            KTPhysicalArray< NDims, XDataType >& operator/=(const KTPhysicalArray< NDims, XDataType >& rhs);

    };

    template< size_t NDims, typename XDataType >
    KTPhysicalArray< NDims, XDataType >::KTPhysicalArray() :
            XVectorType(),
            KTAxisProperties< NDims >()
    {
        SetNBinsFunc(new KTNBinsInArray< NDims, XVectorType >(this, &XVectorType::size));
        KTWARN(utillog_physarr, NDims << "-dimensional arrays are not supported.\n"
                "This is an instance of a dummy object.");
    }

    template< size_t NDims, typename XDataType >
    KTPhysicalArray< NDims, XDataType >::KTPhysicalArray(const KTPhysicalArray< NDims, XDataType >& orig) :
            XVectorType(),
            KTAxisProperties< NDims >()
    {
        SetNBinsFunc(new KTNBinsInArray< NDims, XVectorType >(this, &XVectorType::size));
        KTWARN(utillog_physarr, NDims << "-dimensional arrays are not supported.\n"
               "This is an instance of a dummy object.");
    }

    template< size_t NDims, typename XDataType >
    KTPhysicalArray< NDims, XDataType >::~KTPhysicalArray()
    {
    }

    template< size_t NDims, typename XDataType >
    KTPhysicalArray< NDims, XDataType >& KTPhysicalArray< NDims, XDataType >::operator=(const KTPhysicalArray< NDims, XDataType>& /*rhs*/)
    {
        return *this;
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
            KTPhysicalArray(const KTPhysicalArray< 1, XDataType >& orig);
            virtual ~KTPhysicalArray();

        public:
            KTPhysicalArray< 1, XDataType >& operator=(const KTPhysicalArray< 1, XDataType >& rhs);
            KTPhysicalArray< 1, XDataType >& operator/=(const KTPhysicalArray< 1, XDataType >& rhs);

        private:
            void SetNewNBinsFunc(); // called from constructor; do not make virtual

    };

    template< typename XDataType >
    KTPhysicalArray< 1, XDataType >::KTPhysicalArray() :
            XVectorType(),
            KTAxisProperties< 1 >()
    {
        SetNBinsFunc(new KTNBinsInArray< 1, XVectorType >(this, &XVectorType::size));
        //std::cout << "You have created a 1-D physical array" << std::endl;
    }

    template< typename XDataType >
    KTPhysicalArray< 1, XDataType >::KTPhysicalArray(size_t nBins, Double_t rangeMin, Double_t rangeMax) :
            XVectorType(nBins),
            KTAxisProperties< 1 >(rangeMin, rangeMax)
    {
        SetNewNBinsFunc();
        //SetNBinsFunc(KTNBinsInArray< 1, XVectorType >(this, &XVectorType::size));
        //SetNBinsFunc(boost::bind(&KTNBinsInArray< 1, XVectorType >::operator(), boost::ref(fNBinsFunc)));
        //std::cout << "You have created a 1-D physical array with " << nBins << " bins, going from " << rangeMin << " to " << rangeMax << "  binwidth: " << fBinWidth << std::endl;
    }

    template< typename XDataType >
    KTPhysicalArray< 1, XDataType >::KTPhysicalArray(const KTPhysicalArray< 1, XDataType >& orig) :
            XVectorType(orig),
            KTAxisProperties< 1 >(orig)
    {
        SetNewNBinsFunc();
    }

    template< typename XDataType >
    KTPhysicalArray< 1, XDataType >::~KTPhysicalArray()
    {
    }

    template< typename XDataType >
    KTPhysicalArray< 1, XDataType >& KTPhysicalArray< 1, XDataType >::operator=(const KTPhysicalArray< 1, XDataType>& rhs)
    {
        XVectorType::operator=(rhs);
        KTAxisProperties< 1 >::operator=(rhs);
        SetNewNBinsFunc();
        return *this;
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

    template< typename XDataType >
    void KTPhysicalArray< 1, XDataType >::SetNewNBinsFunc()
    {
        SetNBinsFunc(new KTNBinsInArray< 1, XVectorType >(this, &XVectorType::size));
        return;
    }


    //*************************
    // 2-D array implementation
    //*************************

    template< typename XDataType >
    class KTPhysicalArray< 2, XDataType > : public boost::numeric::ublas::matrix< XDataType >, public KTAxisProperties< 2 >
    {
        private:
            typedef boost::numeric::ublas::matrix< XDataType > XMatrixType;
            typedef KTNBinsInArray< 2, XMatrixType > XNBinsFunctor;

        public:
            KTPhysicalArray();
            KTPhysicalArray(size_t xNBins, Double_t xRangeMin, Double_t xRangeMax, size_t yNBins, Double_t yRangeMin, Double_t yRangeMax);
            KTPhysicalArray(const KTPhysicalArray< 2, XDataType >& orig);
            virtual ~KTPhysicalArray();

        public:
            KTPhysicalArray< 2, XDataType >& operator=(const KTPhysicalArray< 2, XDataType >& rhs);
            KTPhysicalArray< 2, XDataType >& operator/=(const KTPhysicalArray< 2, XDataType >& rhs);

    };

    template< typename XDataType >
    KTPhysicalArray< 2, XDataType >::KTPhysicalArray() :
            XMatrixType(),
            KTAxisProperties< 2 >()
    {
        size_t (XMatrixType::*sizeArray[2])() const = {&XMatrixType::size1, &XMatrixType::size2};
        SetNBinsFunc(new KTNBinsInArray< 2, XMatrixType >((XMatrixType*)this, sizeArray));
        //std::cout << "You have created a 2-D physical array" << std::endl;
    }

    template< typename XDataType >
    KTPhysicalArray< 2, XDataType >::KTPhysicalArray(size_t xNBins, Double_t xRangeMin, Double_t xRangeMax, size_t yNBins, Double_t yRangeMin, Double_t yRangeMax) :
            XMatrixType(xNBins, yNBins),
            KTAxisProperties< 2 >()
    {
        size_t (XMatrixType::*sizeArray[2])() const = {&XMatrixType::size1, &XMatrixType::size2};
        SetNBinsFunc(new KTNBinsInArray< 2, XMatrixType >((XMatrixType*)this, sizeArray));
        //SetNBinsFunc(new KTNBinsInArray< 2, XMatrixType >((XMatrixType*)this, (size_t (XMatrixType::*[])()){&XMatrixType::size1, &XMatrixType::size2}));
        SetRangeMin(1, xRangeMin);
        SetRangeMin(2, yRangeMin);
        SetRangeMax(1, xRangeMax);
        SetRangeMax(2, yRangeMax);
        //std::cout << "You have created a 2-D physical array with " << nBins << " bins, going from " << rangeMin << " to " << rangeMax << "  binwidth: " << fBinWidth << std::endl;
    }

    template< typename XDataType >
    KTPhysicalArray< 2, XDataType >::KTPhysicalArray(const KTPhysicalArray< 2, XDataType >& orig) :
            KTPhysicalArray< 2, XDataType >::XMatrixType(orig.GetNBins(1), orig.GetNBins(2)),
            KTAxisProperties< 2 >()
    {
        size_t (XMatrixType::*sizeArray[2])() const = {&XMatrixType::size1, &XMatrixType::size2};
        SetNBinsFunc(new KTNBinsInArray< 2, XMatrixType >((XMatrixType*)this, sizeArray));
        SetRangeMin(1, orig.GetRangeMin(1));
        SetRangeMin(2, orig.GetRangeMin(2));
        SetRangeMax(1, orig.GetRangeMax(1));
        SetRangeMax(2, orig.GetRangeMax(2));
    }

    template< typename XDataType >
    KTPhysicalArray< 2, XDataType >::~KTPhysicalArray()
    {
    }

    template< typename XDataType >
    KTPhysicalArray< 2, XDataType >& KTPhysicalArray< 2, XDataType >::operator=(const KTPhysicalArray< 2, XDataType>& rhs)
    {
        XMatrixType::operator=(rhs);
        KTAxisProperties< 2 >::operator=(rhs);
        size_t (XMatrixType::*sizeArray[2])() const = {&XMatrixType::size1, &XMatrixType::size2};
        SetNBinsFunc(new KTNBinsInArray< 2, XMatrixType >((XMatrixType*)this, sizeArray));
        SetRangeMin(1, rhs.GetRangeMin(1));
        SetRangeMin(2, rhs.GetRangeMin(2));
        SetRangeMax(1, rhs.GetRangeMax(1));
        SetRangeMax(2, rhs.GetRangeMax(2));
        return *this;
    }

    template< typename XDataType >
    KTPhysicalArray< 2, XDataType >& KTPhysicalArray< 2, XDataType >::operator/=(const KTPhysicalArray< 2, XDataType>& rhs)
    {
        if (rhs.size1() != this->size1() || rhs.size2() != this->size2()) return *this;
        for (size_t iBinX=0; iBinX<this->size1(); iBinX++)
        {
            for (size_t iBinY=0; iBinY<this->size2(); iBinY++)
            {
                (*this)(iBinX, iBinY) /= rhs(iBinX, iBinY);
            }
        }
        return *this;
    }

} /* namespace Katydid */
#endif /* KTPHYSICALARRAY_HH_ */
