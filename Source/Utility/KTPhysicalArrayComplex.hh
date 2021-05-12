/*
 * KTPhysicalArrayComplex.hh
 *
 *  Created on: Feb 05, 2021
 *      Author: F. Thomas
 */

#ifndef KTPHYSICALARRAYCOMPLEX_HH_
#define KTPHYSICALARRAYCOMPLEX_HH_

#include "KTAxisProperties.hh"
#include "KTPhysicalArray.hh"

#include <eigen3/Eigen/Dense>
#include <complex>
#include <sstream>
#include <stdexcept>
#include <iterator>
#include <type_traits>

#include <boost/iterator/iterator_facade.hpp>

namespace Katydid
{
    
    //utility for the 2D iterators
    template < typename value_type >
    class SkipIterator: public boost::iterator_facade< SkipIterator<value_type>, value_type, boost::bidirectional_traversal_tag >
    {
        public:
            
            SkipIterator():
                position{ 0 },
                skip{ 0 }
            {}
            
            explicit SkipIterator(value_type* p, long long nRows): 
                position{ p },
                skip{ nRows }
            {}
            
            value_type* begin()
            {
                return position;
            }
            
            value_type* end()
            {
                return position + skip;
            }
        
        private:
            friend class boost::iterator_core_access;

            void increment() { position += skip; }
            void decrement() { position -= skip; }

            bool equal(SkipIterator< value_type > const& other) const
            {
                return this->position == other.position &&
                        this->skip == other.skip;
            }

            value_type& dereference() const { return *position; }
    
            value_type* position;
            long long skip;
    };

    //*******************************
    // 1D implementation
    //*******************************

    template<>
    class KTPhysicalArray< 1, std::complex<double> > : public KTAxisProperties< 1 >
    {
            
        public:
        
        using value_type = std::complex<double>;
        using array_type = Eigen::Array< value_type, Eigen::Dynamic, 1, Eigen::ColMajor >;
        
        //Maybe revisit when eigen 3.4 is released
        //eigen 3.4 will add native iterator support
        using iterator = value_type*; //array_type::iterator;
        using const_iterator = const value_type*; //array_type::const_iterator;
        using reverse_iterator = std::reverse_iterator< value_type *>;
        using const_reverse_iterator = std::reverse_iterator< const value_type * >;

        public:
            KTPhysicalArray();
            explicit KTPhysicalArray(size_t nBins, double rangeMin=0., double rangeMax=1.);
            explicit KTPhysicalArray(value_type value, size_t nBins, double rangeMin=0., double rangeMax=1.);

            virtual ~KTPhysicalArray() = default;
            KTPhysicalArray( KTPhysicalArray< 1, value_type> &&) = default;
            KTPhysicalArray<1, value_type>& operator=( KTPhysicalArray< 1, value_type> &&) = default;

            KTPhysicalArray( const KTPhysicalArray< 1, value_type>& ) = default;
            KTPhysicalArray<1, value_type>& operator=( const KTPhysicalArray< 1, value_type>& ) = default;

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


            KTPhysicalArray< 1, value_type >& operator+=(const KTPhysicalArray< 1, value_type >& rhs);
            KTPhysicalArray< 1, value_type >& operator-=(const KTPhysicalArray< 1, value_type >& rhs);
            KTPhysicalArray< 1, value_type >& operator*=(const KTPhysicalArray< 1, value_type >& rhs);
            KTPhysicalArray< 1, value_type >& operator/=(const KTPhysicalArray< 1, value_type >& rhs);

            KTPhysicalArray< 1, value_type >& operator+=(const value_type& rhs);
            KTPhysicalArray< 1, value_type >& operator-=(const value_type& rhs);
            KTPhysicalArray< 1, value_type >& operator*=(const value_type& rhs);
            KTPhysicalArray< 1, value_type >& operator/=(const value_type& rhs);
            
            friend KTPhysicalArray< 1, value_type > operator%(const KTPhysicalArray< 2, value_type >& lhs, KTPhysicalArray< 1, value_type > rhs);

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
    
    //*************************
    // Free function operators
    //*************************
    
    std::ostream& operator<< (std::ostream& ostr, const KTPhysicalArray< 1, std::complex<double> >& rhs);
    
    // this template enables usability of the arithmetic operators with all derived classes
    template<typename T>
    using IsKTPhysicalArray1D = typename std::enable_if<std::is_base_of<KTPhysicalArray< 1, std::complex<double> >, T>::value, T>::type;
    
    /// Add two 1-D KTPhysicalArrays; requires lhs.size() == rhs.size(); axis range set to that of lhs.
    template < typename T> 
    IsKTPhysicalArray1D<T> operator+(T lhs, const T& rhs)
    {
        if (! lhs.IsCompatibleWith(rhs)) return T();//KTPhysicalArray< 1, std::complex<double> >();

        lhs.KTPhysicalArray<1, std::complex<double> >::operator+=(rhs);
        return lhs;
    }
    
    /// Subtracts two 1-D KTPhysicalArrays; requires lhs.size() == rhs.size(); axis range set to that of lhs.
    template < typename T> 
    IsKTPhysicalArray1D<T> operator-(T lhs, const T& rhs)
    {
        if (! lhs.IsCompatibleWith(rhs)) return T();

        lhs.KTPhysicalArray<1, std::complex<double> >::operator-=(rhs);
        return lhs;
    }

    /// Multiplies two 1-D KTPhysicalArrays; requires lhs.size() == rhs.size(); axis range set to that of lhs.
    template < typename T> 
    IsKTPhysicalArray1D<T> operator*(T lhs, const T& rhs)
    {
        if (! lhs.IsCompatibleWith(rhs)) return T();

        lhs.KTPhysicalArray<1, std::complex<double> >::operator*=(rhs);
        return lhs;
    }

    /// Divides two 1-D KTPhysicalArrays; requires lhs.size() == rhs.size(); axis range set to that of lhs.
    template < typename T> 
    IsKTPhysicalArray1D<T> operator/(T lhs, const T& rhs)
    {
        if (! lhs.IsCompatibleWith(rhs)) return T();

        lhs.KTPhysicalArray<1, std::complex<double> >::operator/=(rhs);
        return lhs;
    }
    
    //*************************
    // 2-D array implementation
    //*************************

    template< >
    class KTPhysicalArray< 2, std::complex<double> > : public KTAxisProperties< 2 >
    {
        public:
            using value_type = std::complex<double>;
            using matrix_type = Eigen::Array< value_type, Eigen::Dynamic, Eigen::Dynamic, Eigen::ColMajor >;
            
            //revisit when eigen 3.4 is released
            //eigen 3.4 will add proper iterators
            using row_iterator = SkipIterator<value_type>;
            using const_row_iterator = SkipIterator< const value_type>;
            
            using col_iterator = SkipIterator<value_type>;
            using const_col_iterator = SkipIterator< const value_type>;
            
            using reverse_row_iterator = SkipIterator<value_type>;
            using const_reverse_row_iterator = SkipIterator< const value_type>;
            
            using reverse_col_iterator = SkipIterator<value_type>;
            using const_reverse_col_iterator = SkipIterator< const value_type>;

        private:
            using XNBinsFunctor = KTNBinsInArray< 2, matrix_type >;

        public:
            KTPhysicalArray();
            KTPhysicalArray(size_t xNBins, double xRangeMin, double xRangeMax, size_t yNBins, double yRangeMin, double yRangeMax);
            KTPhysicalArray(value_type value, size_t xNBins, double xRangeMin, double xRangeMax, size_t yNBins, double yRangeMin, double yRangeMax);
            virtual ~KTPhysicalArray() = default;

            KTPhysicalArray( KTPhysicalArray< 2, value_type> &&) = default;
            KTPhysicalArray<2, value_type>& operator=( KTPhysicalArray< 2, value_type> &&) = default;

            KTPhysicalArray( const KTPhysicalArray< 2, value_type>& ) = default;
            KTPhysicalArray<2, value_type>& operator=( const KTPhysicalArray< 2, value_type>& ) = default;

        public:
            const matrix_type& GetData() const;
            matrix_type& GetData();

            const std::string& GetDataLabel() const;
            void SetDataLabel(const std::string& label);

        protected:
            matrix_type fData;
            std::string fLabel;

        public:
            
            size_t cols() const;
            size_t rows() const;
            const value_type& operator()(unsigned i, unsigned j) const;
            value_type& operator()(unsigned i, unsigned j);

        public:
            bool IsCompatibleWith(const KTPhysicalArray< 2, value_type >& rhs) const;

            KTPhysicalArray< 2, value_type >& operator+=(const KTPhysicalArray< 2, value_type >& rhs);
            KTPhysicalArray< 2, value_type >& operator-=(const KTPhysicalArray< 2, value_type >& rhs);
            KTPhysicalArray< 2, value_type >& operator*=(const KTPhysicalArray< 2, value_type >& rhs);
            KTPhysicalArray< 2, value_type >& operator/=(const KTPhysicalArray< 2, value_type >& rhs);
            
            //matrix multiplication
            KTPhysicalArray< 2, value_type >& operator%=(const KTPhysicalArray< 2, value_type >& rhs);
            
            friend KTPhysicalArray< 1, value_type > operator%(const KTPhysicalArray< 2, value_type >& lhs, KTPhysicalArray< 1, value_type > rhs);

            KTPhysicalArray< 2, value_type >& operator+=(const value_type& rhs);
            KTPhysicalArray< 2, value_type >& operator-=(const value_type& rhs);
            KTPhysicalArray< 2, value_type >& operator*=(const value_type& rhs);
            KTPhysicalArray< 2, value_type >& operator/=(const value_type& rhs);

        public:
            const_col_iterator begin1() const;
            const_row_iterator begin2() const;
            const_col_iterator end1() const;
            const_row_iterator end2() const;
            col_iterator begin1();
            row_iterator begin2();
            col_iterator end1();
            row_iterator end2();
            
            const_reverse_col_iterator rbegin1() const;
            const_reverse_row_iterator rbegin2() const;
            const_reverse_col_iterator rend1() const;
            const_reverse_row_iterator rend2() const;
            reverse_col_iterator rbegin1();
            reverse_row_iterator rbegin2();
            reverse_col_iterator rend1();
            reverse_row_iterator rend2();

        public:
            value_type GetMaximumBin(unsigned& maxXBin, unsigned& maxYBin) const;
            value_type GetMinimumBin(unsigned& minXBin, unsigned& minYBin) const;
            /// Returns the pair (min value, max value)
            std::pair< value_type, value_type > GetMinMaxBin(unsigned& minXBin, unsigned& minYBin, unsigned& maxXBin, unsigned& maxYBin);
            
    };
    
    //*************************
    // Free function operators 2D
    //*************************
    
    std::ostream& operator<< (std::ostream& ostr, const KTPhysicalArray< 2, std::complex<double> >& rhs);
    
    template<typename T>
    using IsKTPhysicalArray2D = typename std::enable_if<std::is_base_of<KTPhysicalArray< 2, std::complex<double> >, T>::value, T>::type;
    
    /// Add two 2-D KTPhysicalArrays; requires lhs.size() == rhs.size(); axis range set to that of lhs.

    template < typename T> 
    IsKTPhysicalArray2D<T> operator+(T lhs, const T& rhs)
    {
        if (! lhs.IsCompatibleWith(rhs)) return T();

        lhs.KTPhysicalArray<2, std::complex<double> >::operator+=(rhs);
        return lhs;
    }

    /// Subtracts two 2-D KTPhysicalArrays; requires lhs.size() == rhs.size(); axis range set to that of lhs.

    template < typename T> 
    IsKTPhysicalArray2D<T> operator-(T lhs, const T& rhs)
    {
        if (! lhs.IsCompatibleWith(rhs)) return T();

        lhs.KTPhysicalArray<2, std::complex<double> >::operator-=(rhs);
        return lhs;
    }

    /// Multiplies two 2-D KTPhysicalArrays; requires lhs.size() == rhs.size(); axis range set to that of lhs.

    template < typename T> 
    IsKTPhysicalArray2D<T> operator*(T lhs, const T& rhs)
    {
        if (! lhs.IsCompatibleWith(rhs)) return T();

        lhs.KTPhysicalArray<2, std::complex<double> >::operator*=(rhs);
        return lhs;
    }

    /// Divides two 2-D KTPhysicalArrays; requires lhs.size() == rhs.size(); axis range set to that of lhs.
    template < typename T> 
    IsKTPhysicalArray2D<T> operator/(T lhs, const T& rhs)
    {
        if (! lhs.IsCompatibleWith(rhs)) return T();

        lhs.KTPhysicalArray<2, std::complex<double> >::operator/=(rhs);
        return lhs;
    }

    //*************************
    // Free function operators for matrix and vector operations
    //*************************
    
    KTPhysicalArray< 2, std::complex<double> > operator%(KTPhysicalArray< 2, std::complex<double> > lhs, const KTPhysicalArray< 2, std::complex<double> >& rhs);
    KTPhysicalArray< 1, std::complex<double> > operator%(const KTPhysicalArray< 2, std::complex<double> >& lhs, KTPhysicalArray< 1, std::complex<double> > rhs);
    
    
} /* namespace Katydid */
#endif /* KTPHYSICALARRAYCOMPLEX_HH_ */
