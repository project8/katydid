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
    
    const Eigen::Array< std::complex<double>, Eigen::Dynamic, 1, Eigen::ColMajor >& KTPhysicalArray< 1, std::complex<double> >::GetData() const
    {
        return fData;
    }

    Eigen::Array< std::complex<double>, Eigen::Dynamic, 1, Eigen::ColMajor >& KTPhysicalArray< 1, std::complex<double> >::GetData()
    {
        return fData;
    }

    const std::string& KTPhysicalArray< 1, std::complex<double> >::GetDataLabel() const
    {
        return fLabel;
    }

    void KTPhysicalArray< 1, std::complex<double> >::SetDataLabel(const std::string& label)
    {
        fLabel = label;
        return;
    }

    const std::complex<double>& KTPhysicalArray< 1, std::complex<double> >::operator()(unsigned i) const
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

    std::complex<double>& KTPhysicalArray< 1, std::complex<double> >::operator()(unsigned i)
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

    bool KTPhysicalArray< 1, std::complex<double> >::IsCompatibleWith(const KTPhysicalArray< 1, std::complex<double> >& rhs) const
    {
        //return (this->size() == rhs.size() && this->GetRangeMin() == rhs.GetRangeMin() && this->GetRangeMax() == GetRangeMax());
        return (this->size() == rhs.size());
    }

    KTPhysicalArray< 1, std::complex<double> >& KTPhysicalArray< 1, std::complex<double> >::operator+=(const KTPhysicalArray< 1, std::complex<double>>& rhs)
    {
        if (this->IsCompatibleWith(rhs)) 
        {
            fData += rhs.fData;
        }

        return *this;
    }

    KTPhysicalArray< 1, std::complex<double> >& KTPhysicalArray< 1, std::complex<double> >::operator-=(const KTPhysicalArray< 1, std::complex<double>>& rhs)
    {
        if (this->IsCompatibleWith(rhs)) 
        {
            fData -= rhs.fData;
        }
        
        return *this;
    }

    KTPhysicalArray< 1, std::complex<double> >& KTPhysicalArray< 1, std::complex<double> >::operator*=(const KTPhysicalArray< 1, std::complex<double>>& rhs)
    {
        if (this->IsCompatibleWith(rhs)) 
        {
            fData *= rhs.fData;
        }
        
        return *this;
    }

    KTPhysicalArray< 1, std::complex<double> >& KTPhysicalArray< 1, std::complex<double> >::operator/=(const KTPhysicalArray< 1, std::complex<double>>& rhs)
    {
        if (this->IsCompatibleWith(rhs)) 
        {
            fData /= rhs.fData;
        }
        
        return *this;
    }

    KTPhysicalArray< 1, std::complex<double> >& KTPhysicalArray< 1, std::complex<double> >::operator+=(const std::complex<double>& rhs)
    {
        fData += rhs;
        return *this;
    }

    KTPhysicalArray< 1, std::complex<double> >& KTPhysicalArray< 1, std::complex<double> >::operator-=(const std::complex<double>& rhs)
    {
        fData -= rhs;
        return *this;
    }

    KTPhysicalArray< 1, std::complex<double> >& KTPhysicalArray< 1, std::complex<double> >::operator*=(const std::complex<double>& rhs)
    {
        fData *= rhs;
        return *this;
    }

    KTPhysicalArray< 1, std::complex<double> >& KTPhysicalArray< 1, std::complex<double> >::operator/=(const std::complex<double>& rhs)
    {
        fData /= rhs;
        return *this;
    }
    
    //*************************
    // Operator implementations
    //*************************

    /// Add two 1-D KTPhysicalArrays; requires lhs.size() == rhs.size(); axis range set to that of lhs.
    KTPhysicalArray< 1, std::complex<double> > operator+(KTPhysicalArray< 1, std::complex<double> > lhs, const KTPhysicalArray< 1, std::complex<double> >& rhs)
    {
        if (! lhs.IsCompatibleWith(rhs)) return KTPhysicalArray< 1, std::complex<double> >();

        lhs += rhs;
        return lhs;
    }

    /// Subtracts two 1-D KTPhysicalArrays; requires lhs.size() == rhs.size(); axis range set to that of lhs.
    KTPhysicalArray< 1, std::complex<double> > operator-(KTPhysicalArray< 1, std::complex<double> > lhs, const KTPhysicalArray< 1, std::complex<double> >& rhs)
    {
        if (! lhs.IsCompatibleWith(rhs)) return KTPhysicalArray< 1, std::complex<double> >();

        lhs -= rhs;
        return lhs;
    }

    /// Multiplies two 1-D KTPhysicalArrays; requires lhs.size() == rhs.size(); axis range set to that of lhs.
    KTPhysicalArray< 1, std::complex<double> > operator*(KTPhysicalArray< 1, std::complex<double> > lhs, const KTPhysicalArray< 1, std::complex<double> >& rhs)
    {
        if (! lhs.IsCompatibleWith(rhs)) return KTPhysicalArray< 1, std::complex<double> >();

        lhs *= rhs;
        return lhs;
    }

    /// Divides two 1-D KTPhysicalArrays; requires lhs.size() == rhs.size(); axis range set to that of lhs.
    KTPhysicalArray< 1, std::complex<double> > operator/(KTPhysicalArray< 1, std::complex<double> > lhs, const KTPhysicalArray< 1, std::complex<double> >& rhs)
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

    KTPhysicalArray< 1, std::complex<double> >::const_iterator KTPhysicalArray< 1, std::complex<double> >::begin() const
    {
        return fData.data();
    }

    KTPhysicalArray< 1, std::complex<double> >::const_iterator KTPhysicalArray< 1, std::complex<double> >::end() const
    {
        return fData.data() + size();
    }

    KTPhysicalArray< 1, std::complex<double> >::iterator KTPhysicalArray< 1, std::complex<double> >::begin()
    {
        return fData.data();
    }

    KTPhysicalArray< 1, std::complex<double> >::iterator KTPhysicalArray< 1, std::complex<double> >::end()
    {
        return fData.data() + size();
    }
    
    KTPhysicalArray< 1, std::complex<double> >::const_reverse_iterator KTPhysicalArray< 1, std::complex<double> >::rbegin() const
    {
        //a reverse iterator points to the first element outside the container
        return std::reverse_iterator<const std::complex<double>* >(fData.data() + size());
    }

    KTPhysicalArray< 1, std::complex<double> >::const_reverse_iterator KTPhysicalArray< 1, std::complex<double> >::rend() const
    {
        return std::reverse_iterator<const std::complex<double>* >(fData.data());
    }

    KTPhysicalArray< 1, std::complex<double> >::reverse_iterator KTPhysicalArray< 1, std::complex<double> >::rbegin()
    {
        return std::reverse_iterator< std::complex<double>* >(fData.data() + size());
    }

    KTPhysicalArray< 1, std::complex<double> >::reverse_iterator KTPhysicalArray< 1, std::complex<double> >::rend()
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
    
    size_t KTPhysicalArray< 2, std::complex<double> >::cols() const
    {
        return static_cast<size_t>(fData.cols());
    }
    
    size_t KTPhysicalArray< 2, std::complex<double> >::rows() const
    {
        return static_cast<size_t>(fData.rows());
    }


    const Eigen::Array< std::complex<double>, Eigen::Dynamic, Eigen::Dynamic, Eigen::ColMajor >& KTPhysicalArray< 2, std::complex<double> >::GetData() const
    {
        return fData;
    }


    typename Eigen::Array< std::complex<double>, Eigen::Dynamic, Eigen::Dynamic, Eigen::ColMajor >& KTPhysicalArray< 2, std::complex<double> >::GetData()
    {
        return fData;
    }


    const std::string& KTPhysicalArray< 2, std::complex<double> >::GetDataLabel() const
    {
        return fLabel;
    }


    void KTPhysicalArray< 2, std::complex<double> >::SetDataLabel(const std::string& label)
    {
        fLabel = label;
        return;
    }


    const std::complex<double>& KTPhysicalArray< 2, std::complex<double> >::operator()(unsigned i, unsigned j) const
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


    std::complex<double>& KTPhysicalArray< 2, std::complex<double> >::operator()(unsigned i, unsigned j)
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


    bool KTPhysicalArray< 2, std::complex<double> >::IsCompatibleWith(const KTPhysicalArray< 2, std::complex<double> >& rhs) const
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
            

    KTPhysicalArray< 2, std::complex<double> >::const_row_iterator KTPhysicalArray< 2, std::complex<double> >::begin2() const
    {
        return const_row_iterator{ fData.data(), 1 };
    }


    KTPhysicalArray< 2, std::complex<double> >::const_col_iterator KTPhysicalArray< 2, std::complex<double> >::begin1() const
    {
        return const_col_iterator{ fData.data(), static_cast<long long>(size(1)) };
    }


    KTPhysicalArray< 2, std::complex<double> >::const_row_iterator KTPhysicalArray< 2, std::complex<double> >::end2() const
    {
        return const_row_iterator{ fData.data() + size(1), 1 };
    }


    KTPhysicalArray< 2, std::complex<double> >::const_col_iterator KTPhysicalArray< 2, std::complex<double> >::end1() const
    {
        return const_col_iterator{ fData.data() + size(1)*size(2), static_cast<long long>(size(1)) };
    }


    KTPhysicalArray< 2, std::complex<double> >::row_iterator KTPhysicalArray< 2, std::complex<double> >::begin2()
    {
        return row_iterator{ fData.data(), 1 };
    }


    KTPhysicalArray< 2, std::complex<double> >::col_iterator KTPhysicalArray< 2, std::complex<double> >::begin1()
    {
        return col_iterator{ fData.data(), static_cast<long long>(size(1)) };
    }


    KTPhysicalArray< 2, std::complex<double> >::row_iterator KTPhysicalArray< 2, std::complex<double> >::end2()
    {
        return row_iterator{ fData.data() + size(1), 1 };
    }


    KTPhysicalArray< 2, std::complex<double> >::col_iterator KTPhysicalArray< 2, std::complex<double> >::end1()
    {
        return col_iterator{ fData.data() + size(1)*size(2), static_cast<long long>(size(1)) };
    }
    
    //*************************
    // Reverse iterators
    //*************************

    KTPhysicalArray< 2, std::complex<double> >::const_reverse_row_iterator KTPhysicalArray< 2, std::complex<double> >::rbegin2() const
    {
        return const_reverse_row_iterator{ fData.data() + size(1), -1 };
    }


    KTPhysicalArray< 2, std::complex<double> >::const_reverse_col_iterator KTPhysicalArray< 2, std::complex<double> >::rbegin1() const
    {
        return const_reverse_col_iterator{ fData.data() + size(1)*size(2), -static_cast<long long>(size(1)) };
    }


    KTPhysicalArray< 2, std::complex<double> >::const_reverse_row_iterator KTPhysicalArray< 2, std::complex<double> >::rend2() const
    {
        return const_reverse_row_iterator{ fData.data(), -1 };
    }


    KTPhysicalArray< 2, std::complex<double> >::const_reverse_col_iterator KTPhysicalArray< 2, std::complex<double> >::rend1() const
    {
        return const_reverse_col_iterator{ fData.data(), -static_cast<long long>(size(1)) };
    }


    KTPhysicalArray< 2, std::complex<double> >::reverse_row_iterator KTPhysicalArray< 2, std::complex<double> >::rbegin2()
    {
        return reverse_row_iterator{ fData.data() + size(1), -1 };
    }


    KTPhysicalArray< 2, std::complex<double> >::reverse_col_iterator KTPhysicalArray< 2, std::complex<double> >::rbegin1()
    {
        return reverse_col_iterator{ fData.data() + size(1)*size(2), -static_cast<long long>(size(1)) };
    }


    KTPhysicalArray< 2, std::complex<double> >::reverse_row_iterator KTPhysicalArray< 2, std::complex<double> >::rend2()
    {
        return reverse_row_iterator{ fData.data(), -1 };
    }


    KTPhysicalArray< 2, std::complex<double> >::reverse_col_iterator KTPhysicalArray< 2, std::complex<double> >::rend1()
    {
        return reverse_col_iterator{ fData.data(), -static_cast<long long>(size(1)) };
    }


    std::complex<double> KTPhysicalArray< 2, std::complex<double> >::GetMaximumBin(unsigned& maxXBin, unsigned& maxYBin) const
    {
        //return fData.maxCoeff(&maxXBin, &maxYBin); // can be used for real values
        
        //functor for finding the maximum
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
        
        MaxVisitor mv;
        fData.visit(mv);
        maxXBin = mv.row;
        maxYBin = mv.col;
        
        return mv.max;
    }


    std::complex<double> KTPhysicalArray< 2, std::complex<double> >::GetMinimumBin(unsigned& minXBin, unsigned& minYBin) const
    {
        //return fData.minCoeff(&minXBin, &minYBin); // can be used for real values
        
        //functor for finding the minimum
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
        
        MinVisitor mv;
        fData.visit(mv);
        minXBin = mv.row;
        minYBin = mv.col;
        
        return mv.min;
    }


    std::pair< std::complex<double>, std::complex<double> > KTPhysicalArray< 2, std::complex<double> >::GetMinMaxBin(unsigned& minXBin, unsigned& minYBin, unsigned& maxXBin, unsigned& maxYBin)
    { 
        //functor for finding minimum and maximum at the same time
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
        
        MinMaxVisitor mv;
        fData.visit(mv);
        minXBin = mv.rowMin;
        minYBin = mv.colMin;
        maxXBin = mv.rowMax;
        maxYBin = mv.colMax;
        
        return std::make_pair(mv.min, mv.max);
    }
    
    //*************************
    // Operators
    //*************************
    
    /// Add two 2-D KTPhysicalArrays; requires lhs.size() == rhs.size(); axis range set to that of lhs.

    KTPhysicalArray< 2, std::complex<double> > operator+(KTPhysicalArray< 2, std::complex<double> > lhs, const KTPhysicalArray< 2, std::complex<double> >& rhs)
    {
        if (! lhs.IsCompatibleWith(rhs)) return KTPhysicalArray< 2, std::complex<double> >();

        lhs += rhs;
        return lhs;
    }

    /// Subtracts two 2-D KTPhysicalArrays; requires lhs.size() == rhs.size(); axis range set to that of lhs.

    KTPhysicalArray< 2, std::complex<double> > operator-(KTPhysicalArray< 2, std::complex<double> > lhs, const KTPhysicalArray< 2, std::complex<double> >& rhs)
    {
        if (! lhs.IsCompatibleWith(rhs)) return KTPhysicalArray< 2, std::complex<double> >();

        lhs -= rhs;
        return lhs;
    }

    /// Multiplies two 2-D KTPhysicalArrays; requires lhs.size() == rhs.size(); axis range set to that of lhs.

    KTPhysicalArray< 2, std::complex<double> > operator*(KTPhysicalArray< 2, std::complex<double> > lhs, const KTPhysicalArray< 2, std::complex<double> >& rhs)
    {
        if (! lhs.IsCompatibleWith(rhs)) return KTPhysicalArray< 2, std::complex<double> >();

        lhs *= rhs;
        return lhs;
    }

    /// Divides two 2-D KTPhysicalArrays; requires lhs.size() == rhs.size(); axis range set to that of lhs.

    KTPhysicalArray< 2, std::complex<double> > operator/(KTPhysicalArray< 2, std::complex<double> > lhs, const KTPhysicalArray< 2, std::complex<double> >& rhs)
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
    
    KTPhysicalArray< 2, std::complex<double> >& KTPhysicalArray< 2, std::complex<double> >::operator%=(const KTPhysicalArray< 2, std::complex<double> >& rhs)
    {
        
        fData = fData.matrix()*rhs.fData.matrix();
        
        SetRangeMin(2, rhs.GetRangeMin(2));
        SetRangeMax(2, rhs.GetRangeMax(2));

        return *this;
    }
    
    KTPhysicalArray< 2, std::complex<double> > operator%(KTPhysicalArray< 2, std::complex<double> > lhs, const KTPhysicalArray< 2, std::complex<double> >& rhs)
    {

        lhs %= rhs;
        return lhs;
    }
    
    //matrix-vector-multiplication
    KTPhysicalArray< 1, std::complex<double> > operator%(const KTPhysicalArray< 2, std::complex<double> >& lhs, KTPhysicalArray< 1, std::complex<double> > rhs)
    {

        rhs.fData = lhs.fData.matrix()*rhs.fData.matrix(); 
        
        rhs.SetRangeMin(lhs.GetRangeMin(1));
        rhs.SetRangeMax(lhs.GetRangeMax(1));
        
        return rhs;
    }

} /* namespace Katydid */
