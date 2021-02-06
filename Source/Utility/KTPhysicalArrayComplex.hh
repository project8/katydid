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

        public:
            const_iterator begin() const;
            const_iterator end() const;
            iterator begin();
            iterator end();

            /*const_reverse_iterator rbegin() const;
            const_reverse_iterator rend() const;
            reverse_iterator rbegin();
            reverse_iterator rend();*/

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

/*

    template< typename value_type >
    inline typename KTPhysicalArray< 1, value_type >::const_reverse_iterator KTPhysicalArray< 1, value_type >::rbegin() const
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
*/

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
            //eigen 3.4 will add proper iterator support
            using iterator = value_type*; //array_type::iterator;
            using const_iterator = const value_type*; //array_type::const_iterator;

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


            KTPhysicalArray< 2, value_type >& operator+=(const value_type& rhs);
            KTPhysicalArray< 2, value_type >& operator-=(const value_type& rhs);
            KTPhysicalArray< 2, value_type >& operator*=(const value_type& rhs);
            KTPhysicalArray< 2, value_type >& operator/=(const value_type& rhs);

        public:
            //~ const_iterator1 begin1() const;
            //~ const_iterator2 begin2() const;
            //~ const_iterator1 end1() const;
            //~ const_iterator2 end2() const;
            //~ iterator1 begin1();
            //~ iterator2 begin2();
            //~ iterator1 end1();
            //~ iterator2 end2();


          /*  
            const_reverse_iterator1 rbegin1() const;
            const_reverse_iterator2 rbegin2() const;
            const_reverse_iterator1 rend1() const;
            const_reverse_iterator2 rend2() const;
            reverse_iterator1 rbegin1();
            reverse_iterator2 rbegin2();
            reverse_iterator1 rend1();
            reverse_iterator2 rend2();  */

        public:
         //   value_type GetMaximumBin(unsigned& maxXBin, unsigned& maxYBin) const;
         //   value_type GetMinimumBin(unsigned& minXBin, unsigned& minYBin) const;
            /// Returns the pair (min value, max value)
         //   std::pair< value_type, value_type > GetMinMaxBin(unsigned& minXBin, unsigned& minYBin, unsigned& maxXBin, unsigned& maxYBin);
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

/*

    inline typename KTPhysicalArray< 2, std::complex<double> >::const_iterator1 KTPhysicalArray< 2, std::complex<double> >::begin1() const
    {
        return fData.begin1();
    }


    inline typename KTPhysicalArray< 2, std::complex<double> >::const_iterator2 KTPhysicalArray< 2, std::complex<double> >::begin2() const
    {
        return fData.begin2();
    }


    inline typename KTPhysicalArray< 2, std::complex<double> >::const_iterator1 KTPhysicalArray< 2, std::complex<double> >::end1() const
    {
        return fData.end1();
    }


    inline typename KTPhysicalArray< 2, std::complex<double> >::const_iterator2 KTPhysicalArray< 2, std::complex<double> >::end2() const
    {
        return fData.end2();
    }


    inline typename KTPhysicalArray< 2, std::complex<double> >::iterator1 KTPhysicalArray< 2, std::complex<double> >::begin1()
    {
        return fData.begin1();
    }


    inline typename KTPhysicalArray< 2, std::complex<double> >::iterator2 KTPhysicalArray< 2, std::complex<double> >::begin2()
    {
        return fData.begin2();
    }


    inline typename KTPhysicalArray< 2, std::complex<double> >::iterator1 KTPhysicalArray< 2, std::complex<double> >::end1()
    {
        return fData.end1();
    }


    inline typename KTPhysicalArray< 2, std::complex<double> >::iterator2 KTPhysicalArray< 2, std::complex<double> >::end2()
    {
        return fData.end2();
    }



    inline typename KTPhysicalArray< 2, std::complex<double> >::const_reverse_iterator1 KTPhysicalArray< 2, std::complex<double> >::rbegin1() const
    {
        return fData.rbegin1();
    }


    inline typename KTPhysicalArray< 2, std::complex<double> >::const_reverse_iterator2 KTPhysicalArray< 2, std::complex<double> >::rbegin2() const
    {
        return fData.rbegin2();
    }


    inline typename KTPhysicalArray< 2, std::complex<double> >::const_reverse_iterator1 KTPhysicalArray< 2, std::complex<double> >::rend1() const
    {
        return fData.rend1();
    }


    inline typename KTPhysicalArray< 2, std::complex<double> >::const_reverse_iterator2 KTPhysicalArray< 2, std::complex<double> >::rend2() const
    {
        return fData.rend2();
    }


    inline typename KTPhysicalArray< 2, std::complex<double> >::reverse_iterator1 KTPhysicalArray< 2, std::complex<double> >::rbegin1()
    {
        return fData.rbegin1();
    }


    inline typename KTPhysicalArray< 2, std::complex<double> >::reverse_iterator2 KTPhysicalArray< 2, std::complex<double> >::rbegin2()
    {
        return fData.rbegin2();
    }


    inline typename KTPhysicalArray< 2, std::complex<double> >::reverse_iterator1 KTPhysicalArray< 2, std::complex<double> >::rend1()
    {
        return fData.rend1();
    }


    inline typename KTPhysicalArray< 2, std::complex<double> >::reverse_iterator2 KTPhysicalArray< 2, std::complex<double> >::rend2()
    {
        return fData.rend2();
    }


    std::complex<double> KTPhysicalArray< 2, std::complex<double> >::GetMaximumBin(unsigned& maxXBin, unsigned& maxYBin) const
    {
        typename KTPhysicalArray< 2, std::complex<double> >::const_iterator1 xBinIt = fData.begin1();
        typename KTPhysicalArray< 2, std::complex<double> >::const_iterator2 yBinIt = std::max_element(xBinIt.begin(), xBinIt.end());
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


    std::complex<double> KTPhysicalArray< 2, std::complex<double> >::GetMinimumBin(unsigned& minXBin, unsigned& minYBin) const
    {
        typename KTPhysicalArray< 2, std::complex<double> >::const_iterator1 xBinIt = fData.begin1();
        typename KTPhysicalArray< 2, std::complex<double> >::const_iterator2 yBinIt = std::min_element(xBinIt.begin(), xBinIt.end());
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


    std::pair< std::complex<double>, std::complex<double> > KTPhysicalArray< 2, std::complex<double> >::GetMinMaxBin(unsigned& minXBin, unsigned& minYBin, unsigned& maxXBin, unsigned& maxYBin)
    {
        typename KTPhysicalArray< 2, std::complex<double> >::const_iterator1 xBinIt = fData.begin1();
        std::pair< typename KTPhysicalArray< 2, std::complex<double> >::const_iterator2, typename KTPhysicalArray< 2, std::complex<double> >::const_iterator2 > yBinIts = std::minmax_element(xBinIt.begin(), xBinIt.end());
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
    }*/

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


} /* namespace Katydid */
#endif /* KTPHYSICALARRAYCOMPLEX_HH_ */
