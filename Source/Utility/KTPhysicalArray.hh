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
    class KTPhysicalArray : public KTAxisProperties< NDims >
    {
        public:
            typedef XDataType value_type;
            //typedef const_iterator;
            //typedef iterator;
            typedef typename boost::numeric::ublas::vector< XDataType > array_type;

        public:
            KTPhysicalArray();
            KTPhysicalArray(const KTPhysicalArray< NDims, value_type >& orig);
            virtual ~KTPhysicalArray();

        public:
            const array_type& GetData() const;
            array_type& GetData();

        private:
            array_type fData;

        public:
            const value_type& operator()(unsigned i) const;
            value_type& operator()(unsigned i);

        public:
            bool IsCompatibleWith(const KTPhysicalArray< NDims, value_type >& rhs) const;

            KTPhysicalArray< NDims, XDataType >& operator=(const KTPhysicalArray< NDims, value_type >& rhs);

            KTPhysicalArray< NDims, XDataType >& operator+=(const KTPhysicalArray< NDims, value_type >& rhs);
            KTPhysicalArray< NDims, XDataType >& operator-=(const KTPhysicalArray< NDims, value_type >& rhs);
            KTPhysicalArray< NDims, XDataType >& operator*=(const KTPhysicalArray< NDims, value_type >& rhs);
            KTPhysicalArray< NDims, XDataType >& operator/=(const KTPhysicalArray< NDims, value_type >& rhs);

            KTPhysicalArray< NDims, XDataType >& operator+=(const value_type& rhs);
            KTPhysicalArray< NDims, XDataType >& operator-=(const value_type& rhs);
            KTPhysicalArray< NDims, XDataType >& operator*=(const value_type& rhs);
            KTPhysicalArray< NDims, XDataType >& operator/=(const value_type& rhs);

    };

    template< size_t NDims, typename XDataType >
    KTPhysicalArray< NDims, XDataType >::KTPhysicalArray() :
            KTAxisProperties< NDims >(),
            fData(1)
    {
        SetNBinsFunc(new KTNBinsInArray< NDims, array_type >(this, &array_type::size));
        KTWARN(utillog_physarr, NDims << "-dimensional arrays are not supported.\n"
                "This is an instance of a dummy object.");
    }

    template< size_t NDims, typename XDataType >
    KTPhysicalArray< NDims, XDataType >::KTPhysicalArray(const KTPhysicalArray< NDims, value_type >& orig) :
            KTAxisProperties< NDims >(),
            fData(1)
    {
        SetNBinsFunc(new KTNBinsInArray< NDims, array_type >(this, &array_type::size));
        KTWARN(utillog_physarr, NDims << "-dimensional arrays are not supported.\n"
               "This is an instance of a dummy object.");
    }

    template< size_t NDims, typename XDataType >
    KTPhysicalArray< NDims, XDataType >::~KTPhysicalArray()
    {
    }

    template< size_t NDims, typename XDataType >
    const typename KTPhysicalArray< NDims, XDataType >::array_type& KTPhysicalArray< NDims, XDataType >::GetData() const
    {
        return fData;
    }

    template< size_t NDims, typename XDataType >
    typename KTPhysicalArray< NDims, XDataType >::array_type& KTPhysicalArray< NDims, XDataType >::GetData()
    {
        return fData;
    }

    template< size_t NDims, typename XDataType >
    const typename KTPhysicalArray< NDims, XDataType >::value_type& KTPhysicalArray< NDims, XDataType >::operator()(unsigned /*i*/) const
    {
        return fData(0);
    }

    template< size_t NDims, typename XDataType >
    typename KTPhysicalArray< NDims, XDataType >::value_type& KTPhysicalArray< NDims, XDataType >::operator()(unsigned /*i*/)
    {
        return fData(0);
    }

    template< size_t NDims, typename XDataType >
    KTPhysicalArray< NDims, XDataType >& KTPhysicalArray< NDims, XDataType >::operator=(const KTPhysicalArray< NDims, value_type>& /*rhs*/)
    {
        return *this;
    }

    template< size_t NDims, typename XDataType >
    bool KTPhysicalArray< NDims, XDataType >::IsCompatibleWith(const KTPhysicalArray< NDims, value_type >& /*rhs*/) const
    {
        return false;
    }

    template< size_t NDims, typename XDataType >
    KTPhysicalArray< NDims, XDataType >& KTPhysicalArray< NDims, XDataType >::operator+=(const KTPhysicalArray< NDims, value_type>& /*rhs*/)
    {
        return *this;
    }

    template< size_t NDims, typename XDataType >
    KTPhysicalArray< NDims, XDataType >& KTPhysicalArray< NDims, XDataType >::operator-=(const KTPhysicalArray< NDims, value_type>& /*rhs*/)
    {
        return *this;
    }

    template< size_t NDims, typename XDataType >
    KTPhysicalArray< NDims, XDataType >& KTPhysicalArray< NDims, XDataType >::operator*=(const KTPhysicalArray< NDims, value_type>& /*rhs*/)
    {
        return *this;
    }

    template< size_t NDims, typename XDataType >
    KTPhysicalArray< NDims, XDataType >& KTPhysicalArray< NDims, XDataType >::operator/=(const KTPhysicalArray< NDims, value_type>& /*rhs*/)
    {
        return *this;
    }

    template< size_t NDims, typename XDataType >
    KTPhysicalArray< NDims, XDataType >& KTPhysicalArray< NDims, XDataType >::operator+=(const value_type& /*rhs*/)
    {
        return *this;
    }

    template< size_t NDims, typename XDataType >
    KTPhysicalArray< NDims, XDataType >& KTPhysicalArray< NDims, XDataType >::operator-=(const value_type& /*rhs*/)
    {
        return *this;
    }

    template< size_t NDims, typename XDataType >
    KTPhysicalArray< NDims, XDataType >& KTPhysicalArray< NDims, XDataType >::operator*=(const value_type& /*rhs*/)
    {
        return *this;
    }

    template< size_t NDims, typename XDataType >
    KTPhysicalArray< NDims, XDataType >& KTPhysicalArray< NDims, XDataType >::operator/=(const value_type& /*rhs*/)
    {
        return *this;
    }

    //*************************
    // 1-D array implementation
    //*************************

    template< typename XDataType >
    class KTPhysicalArray< 1, XDataType > : public KTAxisProperties< 1 >
    {
        public:
            typedef XDataType value_type;
            typedef typename boost::numeric::ublas::vector< XDataType > array_type;
            typedef typename array_type::const_iterator const_iterator;
            typedef typename array_type::iterator iterator;
            typedef typename array_type::const_reverse_iterator const_reverse_iterator;
            typedef typename array_type::reverse_iterator reverse_iterator;

        private:
            typedef KTNBinsInArray< 1, array_type > XNBinsFunctor;

        public:
            KTPhysicalArray();
            KTPhysicalArray(size_t nBins, Double_t rangeMin=0., Double_t rangeMax=1.);
            KTPhysicalArray(const KTPhysicalArray< 1, value_type >& orig);
            virtual ~KTPhysicalArray();

        public:
            const array_type& GetData() const;
            array_type& GetData();

        private:
            array_type fData;

        public:
            const value_type& operator()(unsigned i) const;
            value_type& operator()(unsigned i);

        public:
            bool IsCompatibleWith(const KTPhysicalArray< 1, value_type >& rhs) const;

            KTPhysicalArray< 1, XDataType >& operator=(const KTPhysicalArray< 1, value_type >& rhs);

            KTPhysicalArray< 1, XDataType >& operator+=(const KTPhysicalArray< 1, value_type >& rhs);
            KTPhysicalArray< 1, XDataType >& operator-=(const KTPhysicalArray< 1, value_type >& rhs);
            KTPhysicalArray< 1, XDataType >& operator*=(const KTPhysicalArray< 1, value_type >& rhs);
            KTPhysicalArray< 1, XDataType >& operator/=(const KTPhysicalArray< 1, value_type >& rhs);

            KTPhysicalArray< 1, XDataType >& operator+=(const value_type& rhs);
            KTPhysicalArray< 1, XDataType >& operator-=(const value_type& rhs);
            KTPhysicalArray< 1, XDataType >& operator*=(const value_type& rhs);
            KTPhysicalArray< 1, XDataType >& operator/=(const value_type& rhs);

        public:
            const_iterator begin() const;
            const_iterator end() const;
            iterator begin();
            iterator end();

            const_reverse_iterator rbegin() const;
            const_reverse_iterator rend() const;
            reverse_iterator rbegin();
            reverse_iterator rend();

        private:
            void SetNewNBinsFunc(); // called from constructor; do not make virtual

    };

    template< typename XDataType >
    KTPhysicalArray< 1, XDataType >::KTPhysicalArray() :
            KTAxisProperties< 1 >(),
            fData()
    {
        SetNBinsFunc(new KTNBinsInArray< 1, array_type >(&fData, &array_type::size));
        //std::cout << "You have created a 1-D physical array" << std::endl;
    }

    template< typename XDataType >
    KTPhysicalArray< 1, XDataType >::KTPhysicalArray(size_t nBins, Double_t rangeMin, Double_t rangeMax) :
            KTAxisProperties< 1 >(rangeMin, rangeMax),
            fData(nBins)
    {
        SetNewNBinsFunc();
        //SetNBinsFunc(KTNBinsInArray< 1, array_type >(this, &array_type::size));
        //SetNBinsFunc(boost::bind(&KTNBinsInArray< 1, array_type >::operator(), boost::ref(fNBinsFunc)));
        //std::cout << "You have created a 1-D physical array with " << nBins << " bins, going from " << rangeMin << " to " << rangeMax << "  binwidth: " << fBinWidth << std::endl;
    }

    template< typename XDataType >
    KTPhysicalArray< 1, XDataType >::KTPhysicalArray(const KTPhysicalArray< 1, value_type >& orig) :
            KTAxisProperties< 1 >(orig),
            fData(orig.fData)
    {
        SetNewNBinsFunc();
    }

    template< typename XDataType >
    KTPhysicalArray< 1, XDataType >::~KTPhysicalArray()
    {
    }

    template< typename XDataType >
    const typename KTPhysicalArray< 1, XDataType >::array_type& KTPhysicalArray< 1, XDataType >::GetData() const
    {
        return fData;
    }

    template< typename XDataType >
    typename KTPhysicalArray< 1, XDataType >::array_type& KTPhysicalArray< 1, XDataType >::GetData()
    {
        return fData;
    }

    template< typename XDataType >
    const typename KTPhysicalArray< 1, XDataType >::value_type& KTPhysicalArray< 1, XDataType >::operator()(unsigned i) const
    {
        return fData(i);
    }

    template< typename XDataType >
    typename KTPhysicalArray< 1, XDataType >::value_type& KTPhysicalArray< 1, XDataType >::operator()(unsigned i)
    {
        return fData(i);
    }

    template< typename XDataType >
    bool KTPhysicalArray< 1, XDataType >::IsCompatibleWith(const KTPhysicalArray< 1, value_type >& rhs) const
    {
        //return (this->size() == rhs.size() && this->GetRangeMin() == rhs.GetRangeMin() && this->GetRangeMax() == GetRangeMax());
        return (this->size() == rhs.size());
    }

    template< typename XDataType >
    KTPhysicalArray< 1, XDataType >& KTPhysicalArray< 1, XDataType >::operator=(const KTPhysicalArray< 1, value_type>& rhs)
    {
        fData = rhs.fData;
        KTAxisProperties< 1 >::operator=(rhs);
        SetNewNBinsFunc();
        return *this;
    }

    template< typename XDataType >
    KTPhysicalArray< 1, XDataType >& KTPhysicalArray< 1, XDataType >::operator+=(const KTPhysicalArray< 1, value_type>& rhs)
    {
        if (! this->IsCompatibleWith(rhs)) return *this;
        for (size_t iBin=0; iBin<fData.size(); iBin++)
        {
            fData(iBin) += rhs(iBin);
        }
        return *this;
    }

    template< typename XDataType >
    KTPhysicalArray< 1, XDataType >& KTPhysicalArray< 1, XDataType >::operator-=(const KTPhysicalArray< 1, value_type>& rhs)
    {
        if (! this->IsCompatibleWith(rhs)) return *this;
        for (size_t iBin=0; iBin<fData.size(); iBin++)
        {
            fData(iBin) -= rhs(iBin);
        }
        return *this;
    }

    template< typename XDataType >
    KTPhysicalArray< 1, XDataType >& KTPhysicalArray< 1, XDataType >::operator*=(const KTPhysicalArray< 1, value_type>& rhs)
    {
        if (! this->IsCompatibleWith(rhs)) return *this;
        for (size_t iBin=0; iBin<fData.size(); iBin++)
        {
            fData(iBin) *= rhs(iBin);
        }
        return *this;
    }

    template< typename XDataType >
    KTPhysicalArray< 1, XDataType >& KTPhysicalArray< 1, XDataType >::operator/=(const KTPhysicalArray< 1, value_type>& rhs)
    {
        if (! this->IsCompatibleWith(rhs)) return *this;
        for (size_t iBin=0; iBin<fData.size(); iBin++)
        {
            fData(iBin) /= rhs(iBin);
        }
        return *this;
    }

    template< typename XDataType >
    KTPhysicalArray< 1, XDataType >& KTPhysicalArray< 1, XDataType >::operator+=(const value_type& rhs)
    {
        for (size_t iBin=0; iBin<fData.size(); iBin++)
        {
            fData(iBin) += rhs;
        }
        return *this;
    }

    template< typename XDataType >
    KTPhysicalArray< 1, XDataType >& KTPhysicalArray< 1, XDataType >::operator-=(const value_type& rhs)
    {
        for (size_t iBin=0; iBin<fData.size(); iBin++)
        {
            fData(iBin) -= rhs;
        }
        return *this;
    }

    template< typename XDataType >
    KTPhysicalArray< 1, XDataType >& KTPhysicalArray< 1, XDataType >::operator*=(const value_type& rhs)
    {
        for (size_t iBin=0; iBin<fData.size(); iBin++)
        {
            fData(iBin) *= rhs;
        }
        return *this;
    }

    template< typename XDataType >
    KTPhysicalArray< 1, XDataType >& KTPhysicalArray< 1, XDataType >::operator/=(const XDataType& rhs)
    {
        for (size_t iBin=0; iBin<fData.size(); iBin++)
        {
            fData(iBin) /= rhs;
        }
        return *this;
    }

    template< typename XDataType >
    typename KTPhysicalArray< 1, XDataType >::const_iterator KTPhysicalArray< 1, XDataType >::begin() const
    {
        return fData.begin();
    }

    template< typename XDataType >
    typename KTPhysicalArray< 1, XDataType >::const_iterator KTPhysicalArray< 1, XDataType >::end() const
    {
        return fData.end();
    }

    template< typename XDataType >
    typename KTPhysicalArray< 1, XDataType >::iterator KTPhysicalArray< 1, XDataType >::begin()
    {
        return fData.begin();
    }

    template< typename XDataType >
    typename KTPhysicalArray< 1, XDataType >::iterator KTPhysicalArray< 1, XDataType >::end()
    {
        return fData.end();
    }


    template< typename XDataType >
    typename KTPhysicalArray< 1, XDataType >::const_reverse_iterator KTPhysicalArray< 1, XDataType >::rbegin() const
    {
        return fData.rbegin();
    }

    template< typename XDataType >
    typename KTPhysicalArray< 1, XDataType >::const_reverse_iterator KTPhysicalArray< 1, XDataType >::rend() const
    {
        return fData.rend();
    }

    template< typename XDataType >
    typename KTPhysicalArray< 1, XDataType >::reverse_iterator KTPhysicalArray< 1, XDataType >::rbegin()
    {
        return fData.rbegin();
    }

    template< typename XDataType >
    typename KTPhysicalArray< 1, XDataType >::reverse_iterator KTPhysicalArray< 1, XDataType >::rend()
    {
        return fData.rend();
    }

    template< typename XDataType >
    void KTPhysicalArray< 1, XDataType >::SetNewNBinsFunc()
    {
        SetNBinsFunc(new KTNBinsInArray< 1, array_type >(&fData, &array_type::size));
        return;
    }


    //*************************
    // 2-D array implementation
    //*************************

    template< typename XDataType >
    class KTPhysicalArray< 2, XDataType > : public KTAxisProperties< 2 >
    {
        public:
            typedef XDataType value_type;
            typedef typename boost::numeric::ublas::matrix< XDataType > matrix_type;
            typedef typename matrix_type::const_iterator1 const_iterator1;
            typedef typename matrix_type::iterator1 iterator1;
            typedef typename matrix_type::const_reverse_iterator1 const_reverse_iterator1;
            typedef typename matrix_type::reverse_iterator1 reverse_iterator1;
            typedef typename matrix_type::const_iterator2 const_iterator2;
            typedef typename matrix_type::iterator2 iterator2;
            typedef typename matrix_type::const_reverse_iterator2 const_reverse_iterator2;
            typedef typename matrix_type::reverse_iterator2 reverse_iterator2;

        private:
            typedef KTNBinsInArray< 2, matrix_type > XNBinsFunctor;

        public:
            KTPhysicalArray();
            KTPhysicalArray(size_t xNBins, Double_t xRangeMin, Double_t xRangeMax, size_t yNBins, Double_t yRangeMin, Double_t yRangeMax);
            KTPhysicalArray(const KTPhysicalArray< 2, value_type >& orig);
            virtual ~KTPhysicalArray();

        public:
            const matrix_type& GetData() const;
            matrix_type& GetData();

        private:
            matrix_type fData;

        public:
            const value_type& operator()(unsigned i, unsigned j) const;
            value_type& operator()(unsigned i, unsigned j);

        public:
            bool IsCompatibleWith(const KTPhysicalArray< 2, value_type >& rhs) const;

            KTPhysicalArray< 2, XDataType >& operator=(const KTPhysicalArray< 2, value_type >& rhs);

            KTPhysicalArray< 2, XDataType >& operator+=(const KTPhysicalArray< 2, value_type >& rhs);
            KTPhysicalArray< 2, XDataType >& operator-=(const KTPhysicalArray< 2, value_type >& rhs);
            KTPhysicalArray< 2, XDataType >& operator*=(const KTPhysicalArray< 2, value_type >& rhs);
            KTPhysicalArray< 2, XDataType >& operator/=(const KTPhysicalArray< 2, value_type >& rhs);

            KTPhysicalArray< 2, XDataType >& operator=(const value_type& rhs);
            KTPhysicalArray< 2, XDataType >& operator+=(const value_type& rhs);
            KTPhysicalArray< 2, XDataType >& operator-=(const value_type& rhs);
            KTPhysicalArray< 2, XDataType >& operator*=(const value_type& rhs);
            KTPhysicalArray< 2, XDataType >& operator/=(const value_type& rhs);

        public:
            const_iterator1 begin1() const;
            const_iterator2 begin2() const;
            const_iterator1 end1() const;
            const_iterator2 end2() const;
            iterator1 begin1();
            iterator2 begin2();
            iterator1 end1();
            iterator2 end2();

            const_reverse_iterator1 rbegin1() const;
            const_reverse_iterator2 rbegin2() const;
            const_reverse_iterator1 rend1() const;
            const_reverse_iterator2 rend2() const;
            reverse_iterator1 rbegin1();
            reverse_iterator2 rbegin2();
            reverse_iterator1 rend1();
            reverse_iterator2 rend2();
    };

    template< typename XDataType >
    KTPhysicalArray< 2, XDataType >::KTPhysicalArray() :
            KTAxisProperties< 2 >(),
            fData()
    {
        size_t (matrix_type::*sizeArray[2])() const = {&matrix_type::size1, &matrix_type::size2};
        SetNBinsFunc(new KTNBinsInArray< 2, matrix_type >(&fData, sizeArray));
        //std::cout << "You have created a 2-D physical array" << std::endl;
    }

    template< typename XDataType >
    KTPhysicalArray< 2, XDataType >::KTPhysicalArray(size_t xNBins, Double_t xRangeMin, Double_t xRangeMax, size_t yNBins, Double_t yRangeMin, Double_t yRangeMax) :
            KTAxisProperties< 2 >(),
            fData(xNBins, yNBins)
    {
        size_t (matrix_type::*sizeArray[2])() const = {&matrix_type::size1, &matrix_type::size2};
        SetNBinsFunc(new KTNBinsInArray< 2, matrix_type >(&fData, sizeArray));
        //SetNBinsFunc(new KTNBinsInArray< 2, matrix_type >((matrix_type*)this, (size_t (matrix_type::*[])()){&matrix_type::size1, &matrix_type::size2}));
        SetRangeMin(1, xRangeMin);
        SetRangeMin(2, yRangeMin);
        SetRangeMax(1, xRangeMax);
        SetRangeMax(2, yRangeMax);
        //std::cout << "You have created a 2-D physical array with " << nBins << " bins, going from " << rangeMin << " to " << rangeMax << "  binwidth: " << fBinWidth << std::endl;
    }

    template< typename XDataType >
    KTPhysicalArray< 2, XDataType >::KTPhysicalArray(const KTPhysicalArray< 2, value_type >& orig) :
            KTAxisProperties< 2 >(),
            fData(orig.fData)
    {
        size_t (matrix_type::*sizeArray[2])() const = {&matrix_type::size1, &matrix_type::size2};
        SetNBinsFunc(new KTNBinsInArray< 2, matrix_type >(&fData, sizeArray));
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
    const typename KTPhysicalArray< 2, XDataType >::matrix_type& KTPhysicalArray< 2, XDataType >::GetData() const
    {
        return fData;
    }

    template< typename XDataType >
    typename KTPhysicalArray< 2, XDataType >::matrix_type& KTPhysicalArray< 2, XDataType >::GetData()
    {
        return fData;
    }

    template< typename XDataType >
    const typename KTPhysicalArray< 2, XDataType >::value_type& KTPhysicalArray< 2, XDataType >::operator()(unsigned i, unsigned j) const
    {
        return fData(i,j);
    }

    template< typename XDataType >
    typename KTPhysicalArray< 2, XDataType >::value_type& KTPhysicalArray< 2, XDataType >::operator()(unsigned i, unsigned j)
    {
        return fData(i,j);
    }

    template< typename XDataType >
    bool KTPhysicalArray< 2, XDataType >::IsCompatibleWith(const KTPhysicalArray< 2, value_type >& rhs) const
    {
        //return (this->size(1) == rhs.size(1) && this->GetRangeMin(1) == rhs.GetRangeMin(1) && this->GetRangeMax(1) == GetRangeMax(1) &&
        //        this->size(2) == rhs.size(2) && this->GetRangeMin(2) == rhs.GetRangeMin(2) && this->GetRangeMax(2) == GetRangeMax(2));
        return (this->size(1) == rhs.size(1) &&
                this->size(2) == rhs.size(2));
    }

    template< typename XDataType >
    KTPhysicalArray< 2, XDataType >& KTPhysicalArray< 2, XDataType >::operator=(const KTPhysicalArray< 2, value_type>& rhs)
    {
        fData = rhs.fData;
        KTAxisProperties< 2 >::operator=(rhs);
        size_t (matrix_type::*sizeArray[2])() const = {&matrix_type::size1, &matrix_type::size2};
        SetNBinsFunc(new KTNBinsInArray< 2, matrix_type >((matrix_type*)this, sizeArray));
        SetRangeMin(1, rhs.GetRangeMin(1));
        SetRangeMin(2, rhs.GetRangeMin(2));
        SetRangeMax(1, rhs.GetRangeMax(1));
        SetRangeMax(2, rhs.GetRangeMax(2));
        return *this;
    }

    template< typename XDataType >
    KTPhysicalArray< 2, XDataType >& KTPhysicalArray< 2, XDataType >::operator+=(const KTPhysicalArray< 2, value_type>& rhs)
    {
        if (! this->IsCompatibleWith(rhs)) return *this;
        for (size_t iBinX=0; iBinX<fData.size1(); iBinX++)
        {
            for (size_t iBinY=0; iBinY<fData.size2(); iBinY++)
            {
                fData(iBinX, iBinY) += rhs(iBinX, iBinY);
            }
        }
        return *this;
    }

    template< typename XDataType >
    KTPhysicalArray< 2, XDataType >& KTPhysicalArray< 2, XDataType >::operator-=(const KTPhysicalArray< 2, value_type>& rhs)
    {
        if (! this->IsCompatibleWith(rhs)) return *this;
        for (size_t iBinX=0; iBinX<fData.size1(); iBinX++)
        {
            for (size_t iBinY=0; iBinY<fData.size2(); iBinY++)
            {
                fData(iBinX, iBinY) -= rhs(iBinX, iBinY);
            }
        }
        return *this;
    }

    template< typename XDataType >
    KTPhysicalArray< 2, XDataType >& KTPhysicalArray< 2, XDataType >::operator*=(const KTPhysicalArray< 2, value_type>& rhs)
    {
        if (! this->IsCompatibleWith(rhs)) return *this;
        for (size_t iBinX=0; iBinX<fData.size1(); iBinX++)
        {
            for (size_t iBinY=0; iBinY<fData.size2(); iBinY++)
            {
                fData(iBinX, iBinY) *= rhs(iBinX, iBinY);
            }
        }
        return *this;
    }

    template< typename XDataType >
    KTPhysicalArray< 2, XDataType >& KTPhysicalArray< 2, XDataType >::operator/=(const KTPhysicalArray< 2, value_type>& rhs)
    {
        if (! this->IsCompatibleWith(rhs)) return *this;
        for (size_t iBinX=0; iBinX<fData.size1(); iBinX++)
        {
            for (size_t iBinY=0; iBinY<fData.size2(); iBinY++)
            {
                fData(iBinX, iBinY) /= rhs(iBinX, iBinY);
            }
        }
        return *this;
    }

    template< typename XDataType >
    KTPhysicalArray< 2, XDataType >& KTPhysicalArray< 2, XDataType >::operator+=(const value_type& rhs)
    {
        for (size_t iBinX=0; iBinX<fData.size1(); iBinX++)
        {
            for (size_t iBinY=0; iBinY<fData.size2(); iBinY++)
            {
                fData(iBinX, iBinY) += rhs;
            }
        }
        return *this;
    }

    template< typename XDataType >
    KTPhysicalArray< 2, XDataType >& KTPhysicalArray< 2, XDataType >::operator-=(const value_type& rhs)
    {
        for (size_t iBinX=0; iBinX<fData.size1(); iBinX++)
        {
            for (size_t iBinY=0; iBinY<fData.size2(); iBinY++)
            {
                fData(iBinX, iBinY) -= rhs;
            }
        }
        return *this;
    }

    template< typename XDataType >
    KTPhysicalArray< 2, XDataType >& KTPhysicalArray< 2, XDataType >::operator*=(const value_type& rhs)
    {
        for (size_t iBinX=0; iBinX<fData.size1(); iBinX++)
        {
            for (size_t iBinY=0; iBinY<fData.size2(); iBinY++)
            {
                fData(iBinX, iBinY) *= rhs;
            }
        }
        return *this;
    }

    template< typename XDataType >
    KTPhysicalArray< 2, XDataType >& KTPhysicalArray< 2, XDataType >::operator/=(const value_type& rhs)
    {
        for (size_t iBinX=0; iBinX<fData.size1(); iBinX++)
        {
            for (size_t iBinY=0; iBinY<fData.size2(); iBinY++)
            {
                fData(iBinX, iBinY) /= rhs;
            }
        }
        return *this;
    }

    template< typename XDataType >
    typename KTPhysicalArray< 2, XDataType >::const_iterator1 KTPhysicalArray< 2, XDataType >::begin1() const
    {
        return fData.begin1();
    }

    template< typename XDataType >
    typename KTPhysicalArray< 2, XDataType >::const_iterator2 KTPhysicalArray< 2, XDataType >::begin2() const
    {
        return fData.begin2();
    }

    template< typename XDataType >
    typename KTPhysicalArray< 2, XDataType >::const_iterator1 KTPhysicalArray< 2, XDataType >::end1() const
    {
        return fData.end1();
    }

    template< typename XDataType >
    typename KTPhysicalArray< 2, XDataType >::const_iterator2 KTPhysicalArray< 2, XDataType >::end2() const
    {
        return fData.end2();
    }

    template< typename XDataType >
    typename KTPhysicalArray< 2, XDataType >::iterator1 KTPhysicalArray< 2, XDataType >::begin1()
    {
        return fData.begin1();
    }

    template< typename XDataType >
    typename KTPhysicalArray< 2, XDataType >::iterator2 KTPhysicalArray< 2, XDataType >::begin2()
    {
        return fData.begin2();
    }

    template< typename XDataType >
    typename KTPhysicalArray< 2, XDataType >::iterator1 KTPhysicalArray< 2, XDataType >::end1()
    {
        return fData.end1();
    }

    template< typename XDataType >
    typename KTPhysicalArray< 2, XDataType >::iterator2 KTPhysicalArray< 2, XDataType >::end2()
    {
        return fData.end2();
    }


    template< typename XDataType >
    typename KTPhysicalArray< 2, XDataType >::const_reverse_iterator1 KTPhysicalArray< 2, XDataType >::rbegin1() const
    {
        return fData.rbegin1();
    }

    template< typename XDataType >
    typename KTPhysicalArray< 2, XDataType >::const_reverse_iterator2 KTPhysicalArray< 2, XDataType >::rbegin2() const
    {
        return fData.rbegin2();
    }

    template< typename XDataType >
    typename KTPhysicalArray< 2, XDataType >::const_reverse_iterator1 KTPhysicalArray< 2, XDataType >::rend1() const
    {
        return fData.rend1();
    }

    template< typename XDataType >
    typename KTPhysicalArray< 2, XDataType >::const_reverse_iterator2 KTPhysicalArray< 2, XDataType >::rend2() const
    {
        return fData.rend2();
    }

    template< typename XDataType >
    typename KTPhysicalArray< 2, XDataType >::reverse_iterator1 KTPhysicalArray< 2, XDataType >::rbegin1()
    {
        return fData.rbegin1();
    }

    template< typename XDataType >
    typename KTPhysicalArray< 2, XDataType >::reverse_iterator2 KTPhysicalArray< 2, XDataType >::rbegin2()
    {
        return fData.rbegin2();
    }

    template< typename XDataType >
    typename KTPhysicalArray< 2, XDataType >::reverse_iterator1 KTPhysicalArray< 2, XDataType >::rend1()
    {
        return fData.rend1();
    }

    template< typename XDataType >
    typename KTPhysicalArray< 2, XDataType >::reverse_iterator2 KTPhysicalArray< 2, XDataType >::rend2()
    {
        return fData.rend2();
    }

    //*************************
    // Operator implementations
    //*************************

    /// Add two 1-D KTPhysicalArrays; requires lhs.size() == rhs.size(); axis range set to that of lhs.
    template< typename XDataType >
    KTPhysicalArray< 1, XDataType > operator+(const KTPhysicalArray< 1, XDataType >& lhs, const KTPhysicalArray< 1, XDataType >& rhs)
    {
        if (! lhs.IsCompatibleWith(rhs)) return KTPhysicalArray< 1, XDataType >();

        KTPhysicalArray< 1, XDataType > returnVal(lhs);
        returnVal += rhs;
        return returnVal;
    }

    /// Subtracts two 1-D KTPhysicalArrays; requires lhs.size() == rhs.size(); axis range set to that of lhs.
    template< typename XDataType >
    KTPhysicalArray< 1, XDataType > operator-(const KTPhysicalArray< 1, XDataType >& lhs, const KTPhysicalArray< 1, XDataType >& rhs)
    {
        if (! lhs.IsCompatibleWith(rhs)) return KTPhysicalArray< 1, XDataType >();

        KTPhysicalArray< 1, XDataType > returnVal(lhs);
        returnVal -= rhs;
        return returnVal;
    }

    /// Multiplies two 1-D KTPhysicalArrays; requires lhs.size() == rhs.size(); axis range set to that of lhs.
    template< typename XDataType >
    KTPhysicalArray< 1, XDataType > operator*(const KTPhysicalArray< 1, XDataType >& lhs, const KTPhysicalArray< 1, XDataType >& rhs)
    {
        if (! lhs.IsCompatibleWith(rhs)) return KTPhysicalArray< 1, XDataType >();

        KTPhysicalArray< 1, XDataType > returnVal(lhs);
        returnVal *= rhs;
        return returnVal;
    }

    /// Divides two 1-D KTPhysicalArrays; requires lhs.size() == rhs.size(); axis range set to that of lhs.
    template< typename XDataType >
    KTPhysicalArray< 1, XDataType > operator/(const KTPhysicalArray< 1, XDataType >& lhs, const KTPhysicalArray< 1, XDataType >& rhs)
    {
        if (! lhs.IsCompatibleWith(rhs)) return KTPhysicalArray< 1, XDataType >();

        KTPhysicalArray< 1, XDataType > returnVal(lhs);
        returnVal /= rhs;
        return returnVal;
    }

    template< typename XDataType >
    std::ostream& operator<< (std::ostream& ostr, const KTPhysicalArray< 1, XDataType >& rhs)
    {
        ostr << rhs.GetData();
        return ostr;
    }

    /// Add two 2-D KTPhysicalArrays; requires lhs.size() == rhs.size(); axis range set to that of lhs.
    template< typename XDataType >
    KTPhysicalArray< 2, XDataType > operator+(const KTPhysicalArray< 2, XDataType >& lhs, const KTPhysicalArray< 2, XDataType >& rhs)
    {
        if (! lhs.IsCompatibleWith(rhs)) return KTPhysicalArray< 2, XDataType >();

        KTPhysicalArray< 2, XDataType > returnVal(lhs);
        returnVal += rhs;
        return returnVal;
    }

    /// Subtracts two 2-D KTPhysicalArrays; requires lhs.size() == rhs.size(); axis range set to that of lhs.
    template< typename XDataType >
    KTPhysicalArray< 2, XDataType > operator-(const KTPhysicalArray< 2, XDataType >& lhs, const KTPhysicalArray< 2, XDataType >& rhs)
    {
        if (! lhs.IsCompatibleWith(rhs)) return KTPhysicalArray< 2, XDataType >();

        KTPhysicalArray< 2, XDataType > returnVal(lhs);
        returnVal -= rhs;
        return returnVal;
    }

    /// Multiplies two 2-D KTPhysicalArrays; requires lhs.size() == rhs.size(); axis range set to that of lhs.
    template< typename XDataType >
    KTPhysicalArray< 2, XDataType > operator*(const KTPhysicalArray< 2, XDataType >& lhs, const KTPhysicalArray< 2, XDataType >& rhs)
    {
        if (! lhs.IsCompatibleWith(rhs)) return KTPhysicalArray< 2, XDataType >();

        KTPhysicalArray< 2, XDataType > returnVal(lhs);
        returnVal *= rhs;
        return returnVal;
    }

    /// Divides two 2-D KTPhysicalArrays; requires lhs.size() == rhs.size(); axis range set to that of lhs.
    template< typename XDataType >
    KTPhysicalArray< 2, XDataType > operator/(const KTPhysicalArray< 2, XDataType >& lhs, const KTPhysicalArray< 2, XDataType >& rhs)
    {
        if (! lhs.IsCompatibleWith(rhs)) return KTPhysicalArray< 2, XDataType >();

        KTPhysicalArray< 2, XDataType > returnVal(lhs);
        returnVal /= rhs;
        return returnVal;
    }

    template< typename XDataType >
    std::ostream& operator<< (std::ostream& ostr, const KTPhysicalArray< 2, XDataType >& rhs)
    {
        ostr << rhs.GetData();
        return ostr;
    }


} /* namespace Katydid */
#endif /* KTPHYSICALARRAY_HH_ */
