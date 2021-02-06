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

#include <boost/iterator/iterator_facade.hpp>

namespace Katydid
{

    //*******************************
    // 1D implementation
    //*******************************

    template<>
    class KTPhysicalArray< 1, std::complex<double> > : public KTAxisProperties< 1 >
    {
            
        public:
        
        using value_type = std::complex<double>;
        using array_type = Eigen::Array< value_type, Eigen::Dynamic, 1, Eigen::ColMajor >;
        
        //revisit when eigen 3.4 is released
        //eigen 3.4 will add proper iterator support
        using iterator = value_type*; //array_type::iterator;
        using const_iterator = const value_type*; //array_type::const_iterator;
        using reverse_iterator = std::reverse_iterator< value_type *>;
        using const_reverse_iterator = std::reverse_iterator< const value_type * >;

        public:
            KTPhysicalArray();
            explicit KTPhysicalArray(size_t nBins, double rangeMin=0., double rangeMax=1.);
            explicit KTPhysicalArray(value_type value, size_t nBins, double rangeMin=0., double rangeMax=1.);

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


            KTPhysicalArray< 1, value_type >& operator+=(const KTPhysicalArray< 1, value_type >& rhs);
            KTPhysicalArray< 1, value_type >& operator-=(const KTPhysicalArray< 1, value_type >& rhs);
            KTPhysicalArray< 1, value_type >& operator*=(const KTPhysicalArray< 1, value_type >& rhs);
            KTPhysicalArray< 1, value_type >& operator/=(const KTPhysicalArray< 1, value_type >& rhs);

            KTPhysicalArray< 1, value_type >& operator+=(const value_type& rhs);
            KTPhysicalArray< 1, value_type >& operator-=(const value_type& rhs);
            KTPhysicalArray< 1, value_type >& operator*=(const value_type& rhs);
            KTPhysicalArray< 1, value_type >& operator/=(const value_type& rhs);
            
            friend KTPhysicalArray< 1, value_type > operator%(KTPhysicalArray< 2, value_type >& lhs, KTPhysicalArray< 1, value_type >& rhs);

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
    
    
    inline const Eigen::Array< std::complex<double>, Eigen::Dynamic, 1, Eigen::ColMajor >& KTPhysicalArray< 1, std::complex<double> >::GetData() const
    {
        return fData;
    }

    inline Eigen::Array< std::complex<double>, Eigen::Dynamic, 1, Eigen::ColMajor >& KTPhysicalArray< 1, std::complex<double> >::GetData()
    {
        return fData;
    }

    inline const std::string& KTPhysicalArray< 1, std::complex<double> >::GetDataLabel() const
    {
        return fLabel;
    }

    inline void KTPhysicalArray< 1, std::complex<double> >::SetDataLabel(const std::string& label)
    {
        fLabel = label;
        return;
    }

    inline const std::complex<double>& KTPhysicalArray< 1, std::complex<double> >::operator()(unsigned i) const
    {
#ifndef NDEBUG
        if (i >= size())
        {
            std::stringstream msg;
            msg << "Out of bounds: " << i << " >= " << size();
            KTERROR(utillog_physarr, msg.str());
            throw std::out_of_range(msg.str());
        }
#endif
        return fData[i];
    }

    inline std::complex<double>& KTPhysicalArray< 1, std::complex<double> >::operator()(unsigned i)
    {
#ifndef NDEBUG
        if (i >= size())
        {
            std::stringstream msg;
            msg << "Out of bounds: " << i << " >= " << size();
            KTERROR(utillog_physarr, msg.str());
            throw std::out_of_range(msg.str());
        }
#endif
        return fData[i];
    }

    inline bool KTPhysicalArray< 1, std::complex<double> >::IsCompatibleWith(const KTPhysicalArray< 1, std::complex<double> >& rhs) const
    {
        //return (this->size() == rhs.size() && this->GetRangeMin() == rhs.GetRangeMin() && this->GetRangeMax() == GetRangeMax());
        return (this->size() == rhs.size());
    }

    inline KTPhysicalArray< 1, std::complex<double> >& KTPhysicalArray< 1, std::complex<double> >::operator+=(const KTPhysicalArray< 1, std::complex<double>>& rhs)
    {
        if (this->IsCompatibleWith(rhs)) 
        {
            fData += rhs.fData;
        }

        return *this;
    }

    inline KTPhysicalArray< 1, std::complex<double> >& KTPhysicalArray< 1, std::complex<double> >::operator-=(const KTPhysicalArray< 1, std::complex<double>>& rhs)
    {
        if (this->IsCompatibleWith(rhs)) 
        {
            fData -= rhs.fData;
        }
        
        return *this;
    }

    inline KTPhysicalArray< 1, std::complex<double> >& KTPhysicalArray< 1, std::complex<double> >::operator*=(const KTPhysicalArray< 1, std::complex<double>>& rhs)
    {
        if (this->IsCompatibleWith(rhs)) 
        {
            fData *= rhs.fData;
        }
        
        return *this;
    }

    inline KTPhysicalArray< 1, std::complex<double> >& KTPhysicalArray< 1, std::complex<double> >::operator/=(const KTPhysicalArray< 1, std::complex<double>>& rhs)
    {
        if (this->IsCompatibleWith(rhs)) 
        {
            fData /= rhs.fData;
        }
        
        return *this;
    }

    inline KTPhysicalArray< 1, std::complex<double> >& KTPhysicalArray< 1, std::complex<double> >::operator+=(const std::complex<double>& rhs)
    {
        fData += rhs;
        return *this;
    }

    inline KTPhysicalArray< 1, std::complex<double> >& KTPhysicalArray< 1, std::complex<double> >::operator-=(const std::complex<double>& rhs)
    {
        fData -= rhs;
        return *this;
    }

    inline KTPhysicalArray< 1, std::complex<double> >& KTPhysicalArray< 1, std::complex<double> >::operator*=(const std::complex<double>& rhs)
    {
        fData *= rhs;
        return *this;
    }

    inline KTPhysicalArray< 1, std::complex<double> >& KTPhysicalArray< 1, std::complex<double> >::operator/=(const std::complex<double>& rhs)
    {
        fData /= rhs;
        return *this;
    }
    
    //*************************
    // Operator implementations
    //*************************

    /// Add two 1-D KTPhysicalArrays; requires lhs.size() == rhs.size(); axis range set to that of lhs.
    inline KTPhysicalArray< 1, std::complex<double> > operator+(KTPhysicalArray< 1, std::complex<double> >& lhs, const KTPhysicalArray< 1, std::complex<double> >& rhs)
    {
        if (! lhs.IsCompatibleWith(rhs)) return KTPhysicalArray< 1, std::complex<double> >();

        lhs += rhs;
        return lhs;
    }

    /// Subtracts two 1-D KTPhysicalArrays; requires lhs.size() == rhs.size(); axis range set to that of lhs.
    inline KTPhysicalArray< 1, std::complex<double> > operator-(KTPhysicalArray< 1, std::complex<double> >& lhs, const KTPhysicalArray< 1, std::complex<double> >& rhs)
    {
        if (! lhs.IsCompatibleWith(rhs)) return KTPhysicalArray< 1, std::complex<double> >();

        lhs -= rhs;
        return lhs;
    }

    /// Multiplies two 1-D KTPhysicalArrays; requires lhs.size() == rhs.size(); axis range set to that of lhs.
    inline KTPhysicalArray< 1, std::complex<double> > operator*(KTPhysicalArray< 1, std::complex<double> >& lhs, const KTPhysicalArray< 1, std::complex<double> >& rhs)
    {
        if (! lhs.IsCompatibleWith(rhs)) return KTPhysicalArray< 1, std::complex<double> >();

        lhs *= rhs;
        return lhs;
    }

    /// Divides two 1-D KTPhysicalArrays; requires lhs.size() == rhs.size(); axis range set to that of lhs.
    inline KTPhysicalArray< 1, std::complex<double> > operator/(KTPhysicalArray< 1, std::complex<double> >& lhs, const KTPhysicalArray< 1, std::complex<double> >& rhs)
    {
        if (! lhs.IsCompatibleWith(rhs)) return KTPhysicalArray< 1, std::complex<double> >();

        lhs /= rhs;
        return lhs;
    }

    std::ostream& operator<< (std::ostream& ostr, const KTPhysicalArray< 1, std::complex<double> >& rhs)
    {
        ostr << rhs.GetData();
        return ostr;
    }
    
    //*************************
    // Iterators
    //*************************

    inline KTPhysicalArray< 1, std::complex<double> >::const_iterator KTPhysicalArray< 1, std::complex<double> >::begin() const
    {
        return fData.data();
    }

    inline KTPhysicalArray< 1, std::complex<double> >::const_iterator KTPhysicalArray< 1, std::complex<double> >::end() const
    {
        return fData.data() + size();
    }

    inline KTPhysicalArray< 1, std::complex<double> >::iterator KTPhysicalArray< 1, std::complex<double> >::begin()
    {
        return fData.data();
    }

    inline KTPhysicalArray< 1, std::complex<double> >::iterator KTPhysicalArray< 1, std::complex<double> >::end()
    {
        return fData.data() + size();
    }
    
    inline KTPhysicalArray< 1, std::complex<double> >::const_reverse_iterator KTPhysicalArray< 1, std::complex<double> >::rbegin() const
    {
        //a reverse iterator points to the first element outside the container
        return std::reverse_iterator<const std::complex<double>* >(fData.data() + size());
    }

    inline KTPhysicalArray< 1, std::complex<double> >::const_reverse_iterator KTPhysicalArray< 1, std::complex<double> >::rend() const
    {
        return std::reverse_iterator<const std::complex<double>* >(fData.data());
    }

    inline KTPhysicalArray< 1, std::complex<double> >::reverse_iterator KTPhysicalArray< 1, std::complex<double> >::rbegin()
    {
        return std::reverse_iterator< std::complex<double>* >(fData.data() + size());
    }

    inline KTPhysicalArray< 1, std::complex<double> >::reverse_iterator KTPhysicalArray< 1, std::complex<double> >::rend()
    {
        return std::reverse_iterator< std::complex<double>* >(fData.data());
    }
    
    //*************************
    // Begin, end free functions for range based for-loop
    //*************************
    
    KTPhysicalArray< 1, std::complex<double> >::iterator begin(KTPhysicalArray< 1, std::complex<double> > & array)
    {
        return array.begin();
    }
    
    KTPhysicalArray< 1, std::complex<double> >::iterator end(KTPhysicalArray< 1, std::complex<double> > & array)
    {
        return array.end();
    }
    
    KTPhysicalArray< 1, std::complex<double> >::const_iterator begin(const KTPhysicalArray< 1, std::complex<double> > & array)
    {
        return array.begin();
    }
    
    KTPhysicalArray< 1, std::complex<double> >::const_iterator end(const KTPhysicalArray< 1, std::complex<double> > & array)
    {
        return array.end();
    }
    
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
            virtual ~KTPhysicalArray();

        public:
            const matrix_type& GetData() const;
            matrix_type& GetData();

            const std::string& GetDataLabel() const;
            void SetDataLabel(const std::string& label);

        protected:
            matrix_type fData;
            std::string fLabel;
            
            size_t cols() const;
            size_t rows() const;

        public:
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
            
            friend KTPhysicalArray< 1, value_type > operator%(KTPhysicalArray< 2, value_type >& lhs, KTPhysicalArray< 1, value_type >& rhs);


            KTPhysicalArray< 2, value_type >& operator+=(const value_type& rhs);
            KTPhysicalArray< 2, value_type >& operator-=(const value_type& rhs);
            KTPhysicalArray< 2, value_type >& operator*=(const value_type& rhs);
            KTPhysicalArray< 2, value_type >& operator/=(const value_type& rhs);

        public:
            const_row_iterator begin1() const;
            const_col_iterator begin2() const;
            const_row_iterator end1() const;
            const_col_iterator end2() const;
            row_iterator begin1();
            col_iterator begin2();
            row_iterator end1();
            col_iterator end2();
            
            const_reverse_row_iterator rbegin1() const;
            const_reverse_col_iterator rbegin2() const;
            const_reverse_row_iterator rend1() const;
            const_reverse_col_iterator rend2() const;
            reverse_row_iterator rbegin1();
            reverse_col_iterator rbegin2();
            reverse_row_iterator rend1();
            reverse_col_iterator rend2();

        public:
            value_type GetMaximumBin(unsigned& maxXBin, unsigned& maxYBin) const;
            value_type GetMinimumBin(unsigned& minXBin, unsigned& minYBin) const;
            /// Returns the pair (min value, max value)
            std::pair< value_type, value_type > GetMinMaxBin(unsigned& minXBin, unsigned& minYBin, unsigned& maxXBin, unsigned& maxYBin);
            
        private:
        
            struct MaxVisitor 
            {
        
                unsigned row;
                unsigned col;
                value_type max;
                
                void init( const value_type& value, Eigen::Index i, Eigen::Index j ) 
                {
                    max=value;
                    row=i;
                    col=j;
                };
                
                void operator()( const value_type& value, Eigen::Index i, Eigen::Index j ) 
                {
                    if(std::abs(value)>std::abs(max)) 
                    {
                        max=value;
                        row=i;
                        col=j;
                    }
                };
            };
            
            struct MinVisitor 
            {
        
                unsigned row;
                unsigned col;
                value_type min;
                
                void init( const value_type& value, Eigen::Index i, Eigen::Index j ) 
                {
                    min=value;
                    row=i;
                    col=j;
                };
                
                void operator()( const value_type& value, Eigen::Index i, Eigen::Index j ) 
                {
                    if(std::abs(value)<std::abs(min)) 
                    {
                        min=value;
                        row=i;
                        col=j;
                    }
                };
            };
            
            struct MinMaxVisitor 
            {
        
                unsigned rowMax;
                unsigned colMax;
                unsigned rowMin;
                unsigned colMin;
                value_type max;
                value_type min;
                
                void init( const value_type& value, Eigen::Index i, Eigen::Index j ) 
                {
                    max=value;
                    min=value;
                    rowMax=i;
                    colMax=j;
                    rowMin=i;
                    colMin=j;
                };
                
                void operator()( const value_type& value, Eigen::Index i, Eigen::Index j ) 
                {
                    if(std::abs(value)>std::abs(max)) 
                    {
                        max=value;
                        rowMax=i;
                        colMax=j;
                    }
                    if(std::abs(value)<std::abs(min))
                    {
                        min=value;
                        rowMin=i;
                        colMin=j;
                    }
                };
            };
    };
    
    size_t KTPhysicalArray< 2, std::complex<double> >::cols() const
    {
        return static_cast<size_t>(fData.cols());
    }
    
    size_t KTPhysicalArray< 2, std::complex<double> >::rows() const
    {
        return static_cast<size_t>(fData.rows());
    }


    inline const Eigen::Array< std::complex<double>, Eigen::Dynamic, Eigen::Dynamic, Eigen::ColMajor >& KTPhysicalArray< 2, std::complex<double> >::GetData() const
    {
        return fData;
    }


    inline typename Eigen::Array< std::complex<double>, Eigen::Dynamic, Eigen::Dynamic, Eigen::ColMajor >& KTPhysicalArray< 2, std::complex<double> >::GetData()
    {
        return fData;
    }


    inline const std::string& KTPhysicalArray< 2, std::complex<double> >::GetDataLabel() const
    {
        return fLabel;
    }


    inline void KTPhysicalArray< 2, std::complex<double> >::SetDataLabel(const std::string& label)
    {
        fLabel = label;
        return;
    }


    inline const std::complex<double>& KTPhysicalArray< 2, std::complex<double> >::operator()(unsigned i, unsigned j) const
    {
#ifdef Katydid_DEBUG
        if (i >= size(1))
        {
            std::stringstream msg;
            msg << "Out of bounds on axis 1: " << i << " >= " << size(1);
            KTERROR(utillog_physarr, msg.str());
            throw std::out_of_range(msg.str());
        }
        if (j >= size(2))
        {
            std::stringstream msg;
            msg << "Out of bounds on axis 2: " << j << " >= " << size(2);
            KTERROR(utillog_physarr, msg.str());
            throw std::out_of_range(msg.str());
        }
#endif
        return fData(i,j);
    }


    inline std::complex<double>& KTPhysicalArray< 2, std::complex<double> >::operator()(unsigned i, unsigned j)
    {
#ifdef Katydid_DEBUG
        if (i >= size(1))
        {
            std::stringstream msg;
            msg << "Out of bounds on axis 1: " << i << " >= " << size(1);
            KTERROR(utillog_physarr, msg.str());
            throw std::out_of_range(msg.str());
         }
        if (j >= size(2))
        {
            std::stringstream msg;
            msg << "Out of bounds on axis 2: " << j << " >= " << size(2);
            KTERROR(utillog_physarr, msg.str());
            throw std::out_of_range(msg.str());
        }
#endif
        return fData(i,j);
    }


    inline bool KTPhysicalArray< 2, std::complex<double> >::IsCompatibleWith(const KTPhysicalArray< 2, std::complex<double> >& rhs) const
    {
        //return (this->size(1) == rhs.size(1) && this->GetRangeMin(1) == rhs.GetRangeMin(1) && this->GetRangeMax(1) == GetRangeMax(1) &&
        //        this->size(2) == rhs.size(2) && this->GetRangeMin(2) == rhs.GetRangeMin(2) && this->GetRangeMax(2) == GetRangeMax(2));
        return (this->size(1) == rhs.size(1) &&
                this->size(2) == rhs.size(2));
    }


    KTPhysicalArray< 2, std::complex<double> >& KTPhysicalArray< 2, std::complex<double> >::operator+=(const KTPhysicalArray< 2, std::complex<double>>& rhs)
    {
        if ( this->IsCompatibleWith(rhs) )
        {
            fData += rhs.fData;
        }

        return *this;
    }


    KTPhysicalArray< 2, std::complex<double> >& KTPhysicalArray< 2, std::complex<double> >::operator-=(const KTPhysicalArray< 2, std::complex<double>>& rhs)
    {
        if ( this->IsCompatibleWith(rhs) )
        {
            fData -= rhs.fData;
        }

        return *this;
    }


    KTPhysicalArray< 2, std::complex<double> >& KTPhysicalArray< 2, std::complex<double> >::operator*=(const KTPhysicalArray< 2, std::complex<double>>& rhs)
    {
        if ( this->IsCompatibleWith(rhs) )
        {
            fData *= rhs.fData;
        }

        return *this;
    }


    KTPhysicalArray< 2, std::complex<double> >& KTPhysicalArray< 2, std::complex<double> >::operator/=(const KTPhysicalArray< 2, std::complex<double>>& rhs)
    {
        if ( this->IsCompatibleWith(rhs) )
        {
            fData /= rhs.fData;
        }

        return *this;
    }


    KTPhysicalArray< 2, std::complex<double> >& KTPhysicalArray< 2, std::complex<double> >::operator+=(const std::complex<double>& rhs)
    {
        fData += rhs;
        return *this;
    }


    KTPhysicalArray< 2, std::complex<double> >& KTPhysicalArray< 2, std::complex<double> >::operator-=(const std::complex<double>& rhs)
    {
        fData -= rhs;
        return *this;
    }


    KTPhysicalArray< 2, std::complex<double> >& KTPhysicalArray< 2, std::complex<double> >::operator*=(const std::complex<double>& rhs)
    {
        fData *= rhs;
        return *this;
    }


    KTPhysicalArray< 2, std::complex<double> >& KTPhysicalArray< 2, std::complex<double> >::operator/=(const std::complex<double>& rhs)
    {
        fData /= rhs;
        return *this;
    }
            

    inline KTPhysicalArray< 2, std::complex<double> >::const_row_iterator KTPhysicalArray< 2, std::complex<double> >::begin1() const
    {
        return const_row_iterator{ fData.data(), 1 };
    }


    inline KTPhysicalArray< 2, std::complex<double> >::const_col_iterator KTPhysicalArray< 2, std::complex<double> >::begin2() const
    {
        return const_col_iterator{ fData.data(), static_cast<long long>(size(1)) };
    }


    inline KTPhysicalArray< 2, std::complex<double> >::const_row_iterator KTPhysicalArray< 2, std::complex<double> >::end1() const
    {
        return const_row_iterator{ fData.data() + size(1), 1 };
    }


    inline KTPhysicalArray< 2, std::complex<double> >::const_col_iterator KTPhysicalArray< 2, std::complex<double> >::end2() const
    {
        return const_col_iterator{ fData.data() + size(1)*size(2), static_cast<long long>(size(1)) };
    }


    inline KTPhysicalArray< 2, std::complex<double> >::row_iterator KTPhysicalArray< 2, std::complex<double> >::begin1()
    {
        return row_iterator{ fData.data(), 1 };
    }


    inline KTPhysicalArray< 2, std::complex<double> >::col_iterator KTPhysicalArray< 2, std::complex<double> >::begin2()
    {
        return col_iterator{ fData.data(), static_cast<long long>(size(1)) };
    }


    inline KTPhysicalArray< 2, std::complex<double> >::row_iterator KTPhysicalArray< 2, std::complex<double> >::end1()
    {
        return row_iterator{ fData.data() + size(1), 1 };
    }


    inline KTPhysicalArray< 2, std::complex<double> >::col_iterator KTPhysicalArray< 2, std::complex<double> >::end2()
    {
        return col_iterator{ fData.data() + size(1)*size(2), static_cast<long long>(size(1)) };
    }
    
    //*************************
    // Reverse iterators
    //*************************

    inline KTPhysicalArray< 2, std::complex<double> >::const_reverse_row_iterator KTPhysicalArray< 2, std::complex<double> >::rbegin1() const
    {
        return const_reverse_row_iterator{ fData.data() + size(1), -1 };
    }


    inline KTPhysicalArray< 2, std::complex<double> >::const_reverse_col_iterator KTPhysicalArray< 2, std::complex<double> >::rbegin2() const
    {
        return const_reverse_col_iterator{ fData.data() + size(1)*size(2), -static_cast<long long>(size(1)) };
    }


    inline KTPhysicalArray< 2, std::complex<double> >::const_reverse_row_iterator KTPhysicalArray< 2, std::complex<double> >::rend1() const
    {
        return const_reverse_row_iterator{ fData.data(), -1 };
    }


    inline KTPhysicalArray< 2, std::complex<double> >::const_reverse_col_iterator KTPhysicalArray< 2, std::complex<double> >::rend2() const
    {
        return const_reverse_col_iterator{ fData.data(), -static_cast<long long>(size(1)) };
    }


    inline KTPhysicalArray< 2, std::complex<double> >::reverse_row_iterator KTPhysicalArray< 2, std::complex<double> >::rbegin1()
    {
        return reverse_row_iterator{ fData.data() + size(1), -1 };
    }


    inline KTPhysicalArray< 2, std::complex<double> >::reverse_col_iterator KTPhysicalArray< 2, std::complex<double> >::rbegin2()
    {
        return reverse_col_iterator{ fData.data() + size(1)*size(2), -static_cast<long long>(size(1)) };
    }


    inline KTPhysicalArray< 2, std::complex<double> >::reverse_row_iterator KTPhysicalArray< 2, std::complex<double> >::rend1()
    {
        return reverse_row_iterator{ fData.data(), -1 };
    }


    inline KTPhysicalArray< 2, std::complex<double> >::reverse_col_iterator KTPhysicalArray< 2, std::complex<double> >::rend2()
    {
        return reverse_col_iterator{ fData.data(), -static_cast<long long>(size(1)) };
    }


    std::complex<double> KTPhysicalArray< 2, std::complex<double> >::GetMaximumBin(unsigned& maxXBin, unsigned& maxYBin) const
    {
        //return fData.maxCoeff(&maxXBin, &maxYBin); // can be used for real values
        
        MaxVisitor mv;
        fData.visit(mv);
        maxXBin = mv.row;
        maxYBin = mv.col;
        
        return mv.max;
    }


    std::complex<double> KTPhysicalArray< 2, std::complex<double> >::GetMinimumBin(unsigned& minXBin, unsigned& minYBin) const
    {
        //return fData.minCoeff(&minXBin, &minYBin); // can be used for real values
        
        MinVisitor mv;
        fData.visit(mv);
        minXBin = mv.row;
        minYBin = mv.col;
        
        return mv.min;
    }


    std::pair< std::complex<double>, std::complex<double> > KTPhysicalArray< 2, std::complex<double> >::GetMinMaxBin(unsigned& minXBin, unsigned& minYBin, unsigned& maxXBin, unsigned& maxYBin)
    { 
        MinMaxVisitor mv;
        fData.visit(mv);
        minXBin = mv.rowMin;
        minYBin = mv.colMin;
        maxXBin = mv.rowMax;
        maxYBin = mv.colMax;
        
        return std::make_pair(mv.min, mv.max);
    }

    /// Add two 2-D KTPhysicalArrays; requires lhs.size() == rhs.size(); axis range set to that of lhs.

    KTPhysicalArray< 2, std::complex<double> > operator+(KTPhysicalArray< 2, std::complex<double> >& lhs, const KTPhysicalArray< 2, std::complex<double> >& rhs)
    {
        if (! lhs.IsCompatibleWith(rhs)) return KTPhysicalArray< 2, std::complex<double> >();

        lhs += rhs;
        return lhs;
    }

    /// Subtracts two 2-D KTPhysicalArrays; requires lhs.size() == rhs.size(); axis range set to that of lhs.

    KTPhysicalArray< 2, std::complex<double> > operator-(KTPhysicalArray< 2, std::complex<double> >& lhs, const KTPhysicalArray< 2, std::complex<double> >& rhs)
    {
        if (! lhs.IsCompatibleWith(rhs)) return KTPhysicalArray< 2, std::complex<double> >();

        lhs -= rhs;
        return lhs;
    }

    /// Multiplies two 2-D KTPhysicalArrays; requires lhs.size() == rhs.size(); axis range set to that of lhs.

    KTPhysicalArray< 2, std::complex<double> > operator*(KTPhysicalArray< 2, std::complex<double> >& lhs, const KTPhysicalArray< 2, std::complex<double> >& rhs)
    {
        if (! lhs.IsCompatibleWith(rhs)) return KTPhysicalArray< 2, std::complex<double> >();

        lhs *= rhs;
        return lhs;
    }

    /// Divides two 2-D KTPhysicalArrays; requires lhs.size() == rhs.size(); axis range set to that of lhs.

    KTPhysicalArray< 2, std::complex<double> > operator/(KTPhysicalArray< 2, std::complex<double> >& lhs, const KTPhysicalArray< 2, std::complex<double> >& rhs)
    {
        if (! lhs.IsCompatibleWith(rhs)) return KTPhysicalArray< 2, std::complex<double> >();

        lhs /= rhs;
        return lhs;
    }

    std::ostream& operator<< (std::ostream& ostr, const KTPhysicalArray< 2, std::complex<double> >& rhs)
    {
        ostr << rhs.GetData();
        return ostr;
    }
    
    
    //*************************
    // Matrix and Vector operations
    //*************************
    
    inline KTPhysicalArray< 2, std::complex<double> >& KTPhysicalArray< 2, std::complex<double> >::operator%=(const KTPhysicalArray< 2, std::complex<double> >& rhs)
    {
        
        fData = fData.matrix()*rhs.fData.matrix();
        
        SetRangeMin(2, rhs.GetRangeMin(2));
        SetRangeMax(2, rhs.GetRangeMax(2));

        return *this;
    }
    
    inline KTPhysicalArray< 2, std::complex<double> > operator%(KTPhysicalArray< 2, std::complex<double> >& lhs, const KTPhysicalArray< 2, std::complex<double> >& rhs)
    {

        lhs %= rhs;
        return lhs;
    }
    
    //matrix-vector-multiplication
    inline KTPhysicalArray< 1, std::complex<double> > operator%(KTPhysicalArray< 2, std::complex<double> >& lhs, KTPhysicalArray< 1, std::complex<double> >& rhs)
    {

        rhs.fData *= lhs.fData; 
        
        rhs.SetRangeMin(lhs.GetRangeMin(1));
        rhs.SetRangeMax(lhs.GetRangeMax(1));
        
        return rhs;
    }


} /* namespace Katydid */
#endif /* KTPHYSICALARRAYCOMPLEX_HH_ */
