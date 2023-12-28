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

#include <cstring>
#include <sstream>
#include <stdexcept>

#ifdef USE_OPENMP
#include <omp.h>
#endif

namespace Katydid
{
    
    LOGGER(utillog_physarr, "KTPhysicalArray");

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

            const std::string& GetDataLabel() const;
            void SetDataLabel(const std::string& label);

        protected:
            array_type fData;
            std:: string fLabel;

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
        LWARN(utillog_physarr, NDims << "-dimensional arrays are not supported.\n"
                "This is an instance of a dummy object.");
    }

    template< size_t NDims, typename XDataType >
    KTPhysicalArray< NDims, XDataType >::KTPhysicalArray(const KTPhysicalArray< NDims, value_type >& /*orig*/) :
            KTAxisProperties< NDims >(),
            fData(1)
    {
        SetNBinsFunc(new KTNBinsInArray< NDims, array_type >(this, &array_type::size));
        LWARN(utillog_physarr, NDims << "-dimensional arrays are not supported.\n"
               "This is an instance of a dummy object.");
    }

    template< size_t NDims, typename XDataType >
    KTPhysicalArray< NDims, XDataType >::~KTPhysicalArray()
    {
    }

    template< size_t NDims, typename XDataType >
    inline const typename KTPhysicalArray< NDims, XDataType >::array_type& KTPhysicalArray< NDims, XDataType >::GetData() const
    {
        return fData;
    }

    template< size_t NDims, typename XDataType >
    inline typename KTPhysicalArray< NDims, XDataType >::array_type& KTPhysicalArray< NDims, XDataType >::GetData()
    {
        return fData;
    }

    template< size_t NDims, typename XDataType >
    inline const std::string& KTPhysicalArray< NDims, XDataType >::GetDataLabel() const
    {
        return fLabel;
    }

    template< size_t NDims, typename XDataType >
    inline void KTPhysicalArray< NDims, XDataType >::SetDataLabel(const std::string& label)
    {
        fLabel = label;
        return;
    }

    template< size_t NDims, typename XDataType >
    inline const typename KTPhysicalArray< NDims, XDataType >::value_type& KTPhysicalArray< NDims, XDataType >::operator()(unsigned /*i*/) const
    {
        return fData(0);
    }

    template< size_t NDims, typename XDataType >
    inline typename KTPhysicalArray< NDims, XDataType >::value_type& KTPhysicalArray< NDims, XDataType >::operator()(unsigned /*i*/)
    {
        return fData(0);
    }

    template< size_t NDims, typename XDataType >
    inline KTPhysicalArray< NDims, XDataType >& KTPhysicalArray< NDims, XDataType >::operator=(const KTPhysicalArray< NDims, value_type>& /*rhs*/)
    {
        return *this;
    }

    template< size_t NDims, typename XDataType >
    inline bool KTPhysicalArray< NDims, XDataType >::IsCompatibleWith(const KTPhysicalArray< NDims, value_type >& /*rhs*/) const
    {
        return false;
    }

    template< size_t NDims, typename XDataType >
    inline KTPhysicalArray< NDims, XDataType >& KTPhysicalArray< NDims, XDataType >::operator+=(const KTPhysicalArray< NDims, value_type>& /*rhs*/)
    {
        return *this;
    }

    template< size_t NDims, typename XDataType >
    inline KTPhysicalArray< NDims, XDataType >& KTPhysicalArray< NDims, XDataType >::operator-=(const KTPhysicalArray< NDims, value_type>& /*rhs*/)
    {
        return *this;
    }

    template< size_t NDims, typename XDataType >
    inline KTPhysicalArray< NDims, XDataType >& KTPhysicalArray< NDims, XDataType >::operator*=(const KTPhysicalArray< NDims, value_type>& /*rhs*/)
    {
        return *this;
    }

    template< size_t NDims, typename XDataType >
    inline KTPhysicalArray< NDims, XDataType >& KTPhysicalArray< NDims, XDataType >::operator/=(const KTPhysicalArray< NDims, value_type>& /*rhs*/)
    {
        return *this;
    }

    template< size_t NDims, typename XDataType >
    inline KTPhysicalArray< NDims, XDataType >& KTPhysicalArray< NDims, XDataType >::operator+=(const value_type& /*rhs*/)
    {
        return *this;
    }

    template< size_t NDims, typename XDataType >
    inline KTPhysicalArray< NDims, XDataType >& KTPhysicalArray< NDims, XDataType >::operator-=(const value_type& /*rhs*/)
    {
        return *this;
    }

    template< size_t NDims, typename XDataType >
    inline KTPhysicalArray< NDims, XDataType >& KTPhysicalArray< NDims, XDataType >::operator*=(const value_type& /*rhs*/)
    {
        return *this;
    }

    template< size_t NDims, typename XDataType >
    inline KTPhysicalArray< NDims, XDataType >& KTPhysicalArray< NDims, XDataType >::operator/=(const value_type& /*rhs*/)
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
            typedef XDataType* array_type;
            typedef XDataType* const_iterator;
            typedef XDataType* iterator;
            typedef XDataType* const_reverse_iterator;
            typedef XDataType* reverse_iterator;

        private:
            typedef KTNBinsInArray< 1, array_type > XNBinsFunctor;

        public:
            KTPhysicalArray();
            explicit KTPhysicalArray(size_t nBins, double rangeMin=0., double rangeMax=1.);
            explicit KTPhysicalArray(XDataType value, size_t nBins, double rangeMin=0., double rangeMax=1.);
            KTPhysicalArray(const KTPhysicalArray< 1, value_type >& orig);
            virtual ~KTPhysicalArray();

        public:
            const array_type& GetData() const;
            array_type& GetData();

            const std::string& GetDataLabel() const;
            void SetDataLabel(const std::string& label);

        protected:
            array_type fData;
            std::string fLabel;

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

    };

    template< typename XDataType >
    KTPhysicalArray< 1, XDataType >::KTPhysicalArray() :
            KTAxisProperties< 1 >(),
            fData(NULL),
            fLabel()
    {
        SetNBinsFunc(new KTNBinsInArray< 1, FixedSize >(0));
    }

    template< typename XDataType >
    KTPhysicalArray< 1, XDataType >::KTPhysicalArray(size_t nBins, double rangeMin, double rangeMax) :
            KTAxisProperties< 1 >(rangeMin, rangeMax),
            fData(NULL),
            fLabel()
    {
        SetNBinsFunc(new KTNBinsInArray< 1, FixedSize >(nBins));
        fData = new XDataType[ nBins ];
    }

    template< typename XDataType >
    KTPhysicalArray< 1, XDataType >::KTPhysicalArray(XDataType value, size_t nBins, double rangeMin, double rangeMax) :
            KTPhysicalArray< 1, XDataType >(nBins, rangeMin, rangeMax)
    {
        for (unsigned index = 0; index < nBins; ++index)
        {
            fData[index] = value;
        }
    }

    template< typename XDataType >
    KTPhysicalArray< 1, XDataType >::KTPhysicalArray(const KTPhysicalArray< 1, value_type >& orig) :
            KTAxisProperties< 1 >(orig),
            fData(orig.fData),
            fLabel(orig.fLabel)
    {
        SetNBinsFunc(new KTNBinsInArray< 1, FixedSize >(orig.size()));
        fData = new XDataType[ orig.size() ];
        memcpy( fData, orig.fData, orig.size() * sizeof( XDataType ) );
    }

    template< typename XDataType >
    KTPhysicalArray< 1, XDataType >::~KTPhysicalArray()
    {
        if (fData != NULL)
        {
            delete [] fData;
        }
    }

    template< typename XDataType >
    inline const typename KTPhysicalArray< 1, XDataType >::array_type& KTPhysicalArray< 1, XDataType >::GetData() const
    {
        return fData;
    }

    template< typename XDataType >
    inline typename KTPhysicalArray< 1, XDataType >::array_type& KTPhysicalArray< 1, XDataType >::GetData()
    {
        return fData;
    }

    template< typename XDataType >
    inline const std::string& KTPhysicalArray< 1, XDataType >::GetDataLabel() const
    {
        return fLabel;
    }

    template< typename XDataType >
    inline void KTPhysicalArray< 1, XDataType >::SetDataLabel(const std::string& label)
    {
        fLabel = label;
        return;
    }

    template< typename XDataType >
    inline const typename KTPhysicalArray< 1, XDataType >::value_type& KTPhysicalArray< 1, XDataType >::operator()(unsigned i) const
    {
#ifndef NDEBUG
        if (i >= size())
        {
            std::stringstream msg;
            msg << "Out of bounds: " << i << " >= " << size();
            LERROR(utillog_physarr, msg.str());
            throw std::out_of_range(msg.str());
        }
#endif
        return fData[i];
    }

    template< typename XDataType >
    inline typename KTPhysicalArray< 1, XDataType >::value_type& KTPhysicalArray< 1, XDataType >::operator()(unsigned i)
    {
#ifndef NDEBUG
        if (i >= size())
        {
            std::stringstream msg;
            msg << "Out of bounds: " << i << " >= " << size();
            LERROR(utillog_physarr, msg.str());
            throw std::out_of_range(msg.str());
        }
#endif
        return fData[i];
    }

    template< typename XDataType >
    inline bool KTPhysicalArray< 1, XDataType >::IsCompatibleWith(const KTPhysicalArray< 1, value_type >& rhs) const
    {
        //return (this->size() == rhs.size() && this->GetRangeMin() == rhs.GetRangeMin() && this->GetRangeMax() == GetRangeMax());
        return (this->size() == rhs.size());
    }

    template< typename XDataType >
    inline KTPhysicalArray< 1, XDataType >& KTPhysicalArray< 1, XDataType >::operator=(const KTPhysicalArray< 1, value_type>& rhs)
    {
        if( fData != NULL )
        {
            delete [] fData;
        }
        fLabel = rhs.fLabel;
        SetNBinsFunc(new KTNBinsInArray< 1, FixedSize >(rhs.size()));
        fData = new XDataType[ rhs.size() ];
        memcpy( fData, rhs.fData, rhs.size() * sizeof( XDataType ) );
        KTAxisProperties< 1 >::operator=(rhs);
        return *this;
    }

    template< typename XDataType >
    inline KTPhysicalArray< 1, XDataType >& KTPhysicalArray< 1, XDataType >::operator+=(const KTPhysicalArray< 1, value_type>& rhs)
    {
        if (! this->IsCompatibleWith(rhs)) return *this;
#pragma omp parallel for
        for (size_t iBin=0; iBin<size(); ++iBin)
        {
            fData[iBin] += rhs.fData[iBin];
        }
        return *this;
    }

    template< typename XDataType >
    inline KTPhysicalArray< 1, XDataType >& KTPhysicalArray< 1, XDataType >::operator-=(const KTPhysicalArray< 1, value_type>& rhs)
    {
        if (! this->IsCompatibleWith(rhs)) return *this;
#pragma omp parallel for
        for (size_t iBin=0; iBin<size(); ++iBin)
        {
            fData[iBin] -= rhs.fData[iBin];
        }
        return *this;
    }

    template< typename XDataType >
    inline KTPhysicalArray< 1, XDataType >& KTPhysicalArray< 1, XDataType >::operator*=(const KTPhysicalArray< 1, value_type>& rhs)
    {
        if (! this->IsCompatibleWith(rhs)) return *this;
#pragma omp parallel for
        for (size_t iBin=0; iBin<size(); ++iBin)
        {
            fData[iBin] *= rhs.fData[iBin];
        }
        return *this;
    }

    template< typename XDataType >
    inline KTPhysicalArray< 1, XDataType >& KTPhysicalArray< 1, XDataType >::operator/=(const KTPhysicalArray< 1, value_type>& rhs)
    {
        if (! this->IsCompatibleWith(rhs)) return *this;
#pragma omp parallel for
        for (size_t iBin=0; iBin<size(); ++iBin)
        {
            fData[iBin] /= rhs.fData[iBin];
        }
        return *this;
    }

    template< typename XDataType >
    inline KTPhysicalArray< 1, XDataType >& KTPhysicalArray< 1, XDataType >::operator+=(const value_type& rhs)
    {
#pragma omp parallel for
        for (size_t iBin=0; iBin<size(); ++iBin)
        {
            fData[iBin] += rhs;
        }
        return *this;
    }

    template< typename XDataType >
    inline KTPhysicalArray< 1, XDataType >& KTPhysicalArray< 1, XDataType >::operator-=(const value_type& rhs)
    {
#pragma omp parallel for
        for (size_t iBin=0; iBin<size(); ++iBin)
        {
            fData[iBin] -= rhs;
        }
        return *this;
    }

    template< typename XDataType >
    inline KTPhysicalArray< 1, XDataType >& KTPhysicalArray< 1, XDataType >::operator*=(const value_type& rhs)
    {
#pragma omp parallel for
        for (size_t iBin=0; iBin<size(); ++iBin)
        {
            fData[iBin] *= rhs;
        }
        return *this;
    }

    template< typename XDataType >
    inline KTPhysicalArray< 1, XDataType >& KTPhysicalArray< 1, XDataType >::operator/=(const XDataType& rhs)
    {
#pragma omp parallel for
        for (size_t iBin=0; iBin<size(); ++iBin)
        {
            fData[iBin] /= rhs;
        }
        return *this;
    }

    template< typename XDataType >
    inline typename KTPhysicalArray< 1, XDataType >::const_iterator KTPhysicalArray< 1, XDataType >::begin() const
    {
        return fData;
    }

    template< typename XDataType >
    inline typename KTPhysicalArray< 1, XDataType >::const_iterator KTPhysicalArray< 1, XDataType >::end() const
    {
        return fData + size();
    }

    template< typename XDataType >
    inline typename KTPhysicalArray< 1, XDataType >::iterator KTPhysicalArray< 1, XDataType >::begin()
    {
        return fData;
    }

    template< typename XDataType >
    inline typename KTPhysicalArray< 1, XDataType >::iterator KTPhysicalArray< 1, XDataType >::end()
    {
        return fData + size();
    }


    template< typename XDataType >
    inline typename KTPhysicalArray< 1, XDataType >::const_reverse_iterator KTPhysicalArray< 1, XDataType >::rbegin() const
    {
        return fData + size() - 1;
    }

    template< typename XDataType >
    inline typename KTPhysicalArray< 1, XDataType >::const_reverse_iterator KTPhysicalArray< 1, XDataType >::rend() const
    {
        return fData - 1;
    }

    template< typename XDataType >
    inline typename KTPhysicalArray< 1, XDataType >::reverse_iterator KTPhysicalArray< 1, XDataType >::rbegin()
    {
        return fData + size() - 1;
    }

    template< typename XDataType >
    inline typename KTPhysicalArray< 1, XDataType >::reverse_iterator KTPhysicalArray< 1, XDataType >::rend()
    {
        return fData - 1;
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
            KTPhysicalArray(size_t xNBins, double xRangeMin, double xRangeMax, size_t yNBins, double yRangeMin, double yRangeMax);
            KTPhysicalArray(XDataType value, size_t xNBins, double xRangeMin, double xRangeMax, size_t yNBins, double yRangeMin, double yRangeMax);
            KTPhysicalArray(const KTPhysicalArray< 2, value_type >& orig);
            virtual ~KTPhysicalArray();

        public:
            const matrix_type& GetData() const;
            matrix_type& GetData();

            const std::string& GetDataLabel() const;
            void SetDataLabel(const std::string& label);

        protected:
            matrix_type fData;
            std::string fLabel;

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

        public:
            value_type GetMaximumBin(unsigned& maxXBin, unsigned& maxYBin) const;
            value_type GetMinimumBin(unsigned& minXBin, unsigned& minYBin) const;
            /// Returns the pair (min value, max value)
            std::pair< value_type, value_type > GetMinMaxBin(unsigned& minXBin, unsigned& minYBin, unsigned& maxXBin, unsigned& maxYBin);
    };

    template< typename XDataType >
    KTPhysicalArray< 2, XDataType >::KTPhysicalArray() :
            KTAxisProperties< 2 >(),
            fData(),
            fLabel()
    {
        size_t (matrix_type::*sizeArray[2])() const = {&matrix_type::size1, &matrix_type::size2};
        SetNBinsFunc(new KTNBinsInArray< 2, matrix_type >(&fData, sizeArray));
        //std::cout << "You have created a 2-D physical array" << std::endl;
    }

    template< typename XDataType >
    KTPhysicalArray< 2, XDataType >::KTPhysicalArray(size_t xNBins, double xRangeMin, double xRangeMax, size_t yNBins, double yRangeMin, double yRangeMax) :
            KTAxisProperties< 2 >(),
            fData(xNBins, yNBins),
            fLabel()
    {
        size_t (matrix_type::*sizeArray[2])() const = {&matrix_type::size1, &matrix_type::size2};
        SetNBinsFunc(new KTNBinsInArray< 2, matrix_type >(&fData, sizeArray));
        //SetNBinsFunc(new KTNBinsInArray< 2, matrix_type >((matrix_type*)this, (size_t (matrix_type::*[])()){&matrix_type::size1, &matrix_type::size2}));
        SetRangeMin(1, xRangeMin);
        SetRangeMin(2, yRangeMin);
        SetRangeMax(1, xRangeMax);
        SetRangeMax(2, yRangeMax);
        //LDEBUG(utillog_physarr, "created matrix with (" << fData.size1() << ", " << fData.size2() << ")");
    }

    template< typename XDataType >
    KTPhysicalArray< 2, XDataType >::KTPhysicalArray(XDataType value, size_t xNBins, double xRangeMin, double xRangeMax, size_t yNBins, double yRangeMin, double yRangeMax) :
            KTPhysicalArray(xNBins, xRangeMin, xRangeMax, yNBins, yRangeMin, yRangeMax)
    {
        for (unsigned xIndex = 0; xIndex < xNBins; ++xIndex)
        {
            for (unsigned yIndex = 0; yIndex < yNBins; ++yIndex)
            {
                fData(xIndex, yIndex) = value;
            }
        }
    }

    template< typename XDataType >
    KTPhysicalArray< 2, XDataType >::KTPhysicalArray(const KTPhysicalArray< 2, value_type >& orig) :
            KTAxisProperties< 2 >(),
            fData(orig.fData),
            fLabel(orig.fLabel)
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
    inline const typename KTPhysicalArray< 2, XDataType >::matrix_type& KTPhysicalArray< 2, XDataType >::GetData() const
    {
        return fData;
    }

    template< typename XDataType >
    inline typename KTPhysicalArray< 2, XDataType >::matrix_type& KTPhysicalArray< 2, XDataType >::GetData()
    {
        return fData;
    }

    template< typename XDataType >
    inline const std::string& KTPhysicalArray< 2, XDataType >::GetDataLabel() const
    {
        return fLabel;
    }

    template< typename XDataType >
    inline void KTPhysicalArray< 2, XDataType >::SetDataLabel(const std::string& label)
    {
        fLabel = label;
        return;
    }

    template< typename XDataType >
    inline const typename KTPhysicalArray< 2, XDataType >::value_type& KTPhysicalArray< 2, XDataType >::operator()(unsigned i, unsigned j) const
    {
#ifdef Katydid_DEBUG
        if (i >= size(1))
        {
            std::stringstream msg;
            msg << "Out of bounds on axis 1: " << i << " >= " << size(1);
            LERROR(utillog_physarr, msg.str());
            throw std::out_of_range(msg.str());
        }
        if (j >= size(2))
        {
            std::stringstream msg;
            msg << "Out of bounds on axis 2: " << j << " >= " << size(2);
            LERROR(utillog_physarr, msg.str());
            throw std::out_of_range(msg.str());
        }
#endif
        return fData(i,j);
    }

    template< typename XDataType >
    inline typename KTPhysicalArray< 2, XDataType >::value_type& KTPhysicalArray< 2, XDataType >::operator()(unsigned i, unsigned j)
    {
#ifdef Katydid_DEBUG
        if (i >= size(1))
        {
            std::stringstream msg;
            msg << "Out of bounds on axis 1: " << i << " >= " << size(1);
            LERROR(utillog_physarr, msg.str());
            throw std::out_of_range(msg.str());
         }
        if (j >= size(2))
        {
            std::stringstream msg;
            msg << "Out of bounds on axis 2: " << j << " >= " << size(2);
            LERROR(utillog_physarr, msg.str());
            throw std::out_of_range(msg.str());
        }
#endif
        return fData(i,j);
    }

    template< typename XDataType >
    inline bool KTPhysicalArray< 2, XDataType >::IsCompatibleWith(const KTPhysicalArray< 2, value_type >& rhs) const
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
        fLabel = rhs.fLabel;
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
        for (size_t iBinX=0; iBinX<fData.size1(); ++iBinX)
        {
            for (size_t iBinY=0; iBinY<fData.size2(); ++iBinY)
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
        for (size_t iBinX=0; iBinX<fData.size1(); ++iBinX)
        {
            for (size_t iBinY=0; iBinY<fData.size2(); ++iBinY)
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
        for (size_t iBinX=0; iBinX<fData.size1(); ++iBinX)
        {
            for (size_t iBinY=0; iBinY<fData.size2(); ++iBinY)
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
        for (size_t iBinX=0; iBinX<fData.size1(); ++iBinX)
        {
            for (size_t iBinY=0; iBinY<fData.size2(); ++iBinY)
            {
                fData(iBinX, iBinY) /= rhs(iBinX, iBinY);
            }
        }
        return *this;
    }

    template< typename XDataType >
    KTPhysicalArray< 2, XDataType >& KTPhysicalArray< 2, XDataType >::operator+=(const value_type& rhs)
    {
        for (size_t iBinX=0; iBinX<fData.size1(); ++iBinX)
        {
            for (size_t iBinY=0; iBinY<fData.size2(); ++iBinY)
            {
                fData(iBinX, iBinY) += rhs;
            }
        }
        return *this;
    }

    template< typename XDataType >
    KTPhysicalArray< 2, XDataType >& KTPhysicalArray< 2, XDataType >::operator-=(const value_type& rhs)
    {
        for (size_t iBinX=0; iBinX<fData.size1(); ++iBinX)
        {
            for (size_t iBinY=0; iBinY<fData.size2(); ++iBinY)
            {
                fData(iBinX, iBinY) -= rhs;
            }
        }
        return *this;
    }

    template< typename XDataType >
    KTPhysicalArray< 2, XDataType >& KTPhysicalArray< 2, XDataType >::operator*=(const value_type& rhs)
    {
        for (size_t iBinX=0; iBinX<fData.size1(); ++iBinX)
        {
            for (size_t iBinY=0; iBinY<fData.size2(); ++iBinY)
            {
                fData(iBinX, iBinY) *= rhs;
            }
        }
        return *this;
    }

    template< typename XDataType >
    KTPhysicalArray< 2, XDataType >& KTPhysicalArray< 2, XDataType >::operator/=(const value_type& rhs)
    {
        for (size_t iBinX=0; iBinX<fData.size1(); ++iBinX)
        {
            for (size_t iBinY=0; iBinY<fData.size2(); ++iBinY)
            {
                fData(iBinX, iBinY) /= rhs;
            }
        }
        return *this;
    }

    template< typename XDataType >
    inline typename KTPhysicalArray< 2, XDataType >::const_iterator1 KTPhysicalArray< 2, XDataType >::begin1() const
    {
        return fData.begin1();
    }

    template< typename XDataType >
    inline typename KTPhysicalArray< 2, XDataType >::const_iterator2 KTPhysicalArray< 2, XDataType >::begin2() const
    {
        return fData.begin2();
    }

    template< typename XDataType >
    inline typename KTPhysicalArray< 2, XDataType >::const_iterator1 KTPhysicalArray< 2, XDataType >::end1() const
    {
        return fData.end1();
    }

    template< typename XDataType >
    inline typename KTPhysicalArray< 2, XDataType >::const_iterator2 KTPhysicalArray< 2, XDataType >::end2() const
    {
        return fData.end2();
    }

    template< typename XDataType >
    inline typename KTPhysicalArray< 2, XDataType >::iterator1 KTPhysicalArray< 2, XDataType >::begin1()
    {
        return fData.begin1();
    }

    template< typename XDataType >
    inline typename KTPhysicalArray< 2, XDataType >::iterator2 KTPhysicalArray< 2, XDataType >::begin2()
    {
        return fData.begin2();
    }

    template< typename XDataType >
    inline typename KTPhysicalArray< 2, XDataType >::iterator1 KTPhysicalArray< 2, XDataType >::end1()
    {
        return fData.end1();
    }

    template< typename XDataType >
    inline typename KTPhysicalArray< 2, XDataType >::iterator2 KTPhysicalArray< 2, XDataType >::end2()
    {
        return fData.end2();
    }


    template< typename XDataType >
    inline typename KTPhysicalArray< 2, XDataType >::const_reverse_iterator1 KTPhysicalArray< 2, XDataType >::rbegin1() const
    {
        return fData.rbegin1();
    }

    template< typename XDataType >
    inline typename KTPhysicalArray< 2, XDataType >::const_reverse_iterator2 KTPhysicalArray< 2, XDataType >::rbegin2() const
    {
        return fData.rbegin2();
    }

    template< typename XDataType >
    inline typename KTPhysicalArray< 2, XDataType >::const_reverse_iterator1 KTPhysicalArray< 2, XDataType >::rend1() const
    {
        return fData.rend1();
    }

    template< typename XDataType >
    inline typename KTPhysicalArray< 2, XDataType >::const_reverse_iterator2 KTPhysicalArray< 2, XDataType >::rend2() const
    {
        return fData.rend2();
    }

    template< typename XDataType >
    inline typename KTPhysicalArray< 2, XDataType >::reverse_iterator1 KTPhysicalArray< 2, XDataType >::rbegin1()
    {
        return fData.rbegin1();
    }

    template< typename XDataType >
    inline typename KTPhysicalArray< 2, XDataType >::reverse_iterator2 KTPhysicalArray< 2, XDataType >::rbegin2()
    {
        return fData.rbegin2();
    }

    template< typename XDataType >
    inline typename KTPhysicalArray< 2, XDataType >::reverse_iterator1 KTPhysicalArray< 2, XDataType >::rend1()
    {
        return fData.rend1();
    }

    template< typename XDataType >
    inline typename KTPhysicalArray< 2, XDataType >::reverse_iterator2 KTPhysicalArray< 2, XDataType >::rend2()
    {
        return fData.rend2();
    }

    template< typename XDataType >
    XDataType KTPhysicalArray< 2, XDataType >::GetMaximumBin(unsigned& maxXBin, unsigned& maxYBin) const
    {
        typename KTPhysicalArray< 2, XDataType >::const_iterator1 xBinIt = fData.begin1();
        typename KTPhysicalArray< 2, XDataType >::const_iterator2 yBinIt = std::max_element(xBinIt.begin(), xBinIt.end());
        maxXBin = 0;
        maxYBin = yBinIt.index2();
        double maxYValue = *yBinIt;
        double value;
        for (; xBinIt != fData.end1(); ++xBinIt)
        {
            yBinIt = std::max_element(xBinIt.begin(), xBinIt.end());
            value = *yBinIt;
            if (value > maxYValue)
            {
                maxXBin = xBinIt.index1();
                maxYBin = yBinIt.index2();
                maxYValue = value;
            }
        }
        return maxYValue;
    }

    template< typename XDataType >
    XDataType KTPhysicalArray< 2, XDataType >::GetMinimumBin(unsigned& minXBin, unsigned& minYBin) const
    {
        typename KTPhysicalArray< 2, XDataType >::const_iterator1 xBinIt = fData.begin1();
        typename KTPhysicalArray< 2, XDataType >::const_iterator2 yBinIt = std::min_element(xBinIt.begin(), xBinIt.end());
        minXBin = 0;
        minYBin = yBinIt.index2();
        double minYValue = *yBinIt;
        double value;
        for (; xBinIt != fData.end1(); ++xBinIt)
        {
            yBinIt = std::min_element(xBinIt.begin(), xBinIt.end());
            value = *yBinIt;
            if (value < minYValue)
            {
                minXBin = xBinIt.index1();
                minYBin = yBinIt.index2();
                minYValue = value;
            }
        }
        return minYValue;
    }

    template< typename XDataType >
    std::pair< XDataType, XDataType > KTPhysicalArray< 2, XDataType >::GetMinMaxBin(unsigned& minXBin, unsigned& minYBin, unsigned& maxXBin, unsigned& maxYBin)
    {
        typename KTPhysicalArray< 2, XDataType >::const_iterator1 xBinIt = fData.begin1();
        std::pair< typename KTPhysicalArray< 2, XDataType >::const_iterator2, typename KTPhysicalArray< 2, XDataType >::const_iterator2 > yBinIts = std::minmax_element(xBinIt.begin(), xBinIt.end());
        minXBin = 0;
        minYBin = yBinIts.first.index2();
        maxXBin = 0;
        maxYBin = yBinIts.second.index2();
        double minYValue = *yBinIts.first;
        double maxYValue = *yBinIts.second;
        double value;
        for (; xBinIt != fData.end1(); ++xBinIt)
        {
            yBinIts = std::minmax_element(xBinIt.begin(), xBinIt.end());
            value = *yBinIts.first;
            if (value < minYValue)
            {
                minXBin = xBinIt.index1();
                minYBin = yBinIts.first.index2();
                minYValue = value;
            }
            value = *yBinIts.second;
            if (value > maxYValue)
            {
                maxXBin = xBinIt.index1();
                maxYBin = yBinIts.second.index2();
                maxYValue = value;
            }
        }
        return std::make_pair(minYValue, maxYValue);
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
