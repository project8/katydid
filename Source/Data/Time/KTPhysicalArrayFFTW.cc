/*
 * KTPhysicalArrayFFTW.cc
 *
 *  Created on: Oct 11, 2012
 *      Author: nsoblath
 */

#include "KTPhysicalArrayFFTW.hh"

#include <cstring>

using std::memcpy;

namespace Katydid
{


    KTPhysicalArray< 1, fftw_complex >::KTPhysicalArray() :
            KTAxisProperties< 1 >(),
            fData(NULL),
            fTempCache()
    {
        SetNBinsFunc(new KTNBinsInArray< 1, FixedSize >(0));
    }


    KTPhysicalArray< 1, fftw_complex >::KTPhysicalArray(size_t nBins, double rangeMin, double rangeMax) :
            KTAxisProperties< 1 >(rangeMin, rangeMax),
            fData(NULL),
            fTempCache()
    {
        SetNBinsFunc(new KTNBinsInArray< 1, FixedSize >(nBins));
        fData = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * nBins);
    }


    KTPhysicalArray< 1, fftw_complex >::KTPhysicalArray(const KTPhysicalArray< 1, fftw_complex >& orig) :
            KTAxisProperties< 1 >(orig),
            fData(NULL),
            fTempCache()
    {
        SetNBinsFunc(new KTNBinsInArray< 1, FixedSize >(orig.size()));
        fData = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * size());
        memcpy( fData, orig.fData, orig.size() * sizeof( fftw_complex ) );
    }


    KTPhysicalArray< 1, fftw_complex >::~KTPhysicalArray()
    {
        if (fData != NULL)
        {
            fftw_free(fData);
        }
    }


    const KTPhysicalArray< 1, fftw_complex >::array_type& KTPhysicalArray< 1, fftw_complex >::GetData() const
    {
        return fData;
    }


    KTPhysicalArray< 1, fftw_complex >::array_type& KTPhysicalArray< 1, fftw_complex >::GetData()
    {
        return fData;
    }


    const KTPhysicalArray< 1, fftw_complex >::value_type& KTPhysicalArray< 1, fftw_complex >::operator()(unsigned i) const
    {
        return fData[i];
    }


    KTPhysicalArray< 1, fftw_complex >::value_type& KTPhysicalArray< 1, fftw_complex >::operator()(unsigned i)
    {
        return fData[i];
    }


    bool KTPhysicalArray< 1, fftw_complex >::IsCompatibleWith(const KTPhysicalArray< 1, fftw_complex >& rhs) const
    {
        //return (this->size() == rhs.size() && this->GetRangeMin() == rhs.GetRangeMin() && this->GetRangeMax() == GetRangeMax());
        return (this->size() == rhs.size());
    }


    KTPhysicalArray< 1, fftw_complex >& KTPhysicalArray< 1, fftw_complex >::operator=(const KTPhysicalArray< 1, fftw_complex>& rhs)
    {
        if (fData != NULL)
        {
            fftw_free(fData);
        }
        SetNBinsFunc(new KTNBinsInArray< 1, FixedSize >(rhs.size()));
        fData = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * size());
        memcpy( fData, rhs.fData, rhs.size() * sizeof( fftw_complex ) );
        KTAxisProperties< 1 >::operator=(rhs);
        return *this;
    }


    KTPhysicalArray< 1, fftw_complex >& KTPhysicalArray< 1, fftw_complex >::operator+=(const KTPhysicalArray< 1, fftw_complex>& rhs)
    {
        if (! this->IsCompatibleWith(rhs)) return *this;
        for (size_t iBin=0; iBin<rhs.size(); ++iBin)
        {
            fData[iBin][0] = fData[iBin][0] + rhs.fData[iBin][0];
            fData[iBin][1] = fData[iBin][1] + rhs.fData[iBin][1];
        }
        return *this;
    }


    KTPhysicalArray< 1, fftw_complex >& KTPhysicalArray< 1, fftw_complex >::operator-=(const KTPhysicalArray< 1, fftw_complex>& rhs)
    {
        if (! this->IsCompatibleWith(rhs)) return *this;
        for (size_t iBin=0; iBin<size(); ++iBin)
        {
            fData[iBin][0] = fData[iBin][0] - rhs.fData[iBin][0];
            fData[iBin][1] = fData[iBin][1] - rhs.fData[iBin][1];
        }
        return *this;
    }


    KTPhysicalArray< 1, fftw_complex >& KTPhysicalArray< 1, fftw_complex >::operator*=(const KTPhysicalArray< 1, fftw_complex>& rhs)
    {
        if (! this->IsCompatibleWith(rhs)) return *this;
        for (size_t iBin=0; iBin<size(); ++iBin)
        {
            fTempCache[0] = fData[iBin][0] * rhs.fData[iBin][0] - fData[iBin][1] * rhs.fData[iBin][1];
            fTempCache[1] = fData[iBin][0] * rhs.fData[iBin][1] + fData[iBin][1] * rhs.fData[iBin][0];
            fData[iBin][0] = fTempCache[0];
            fData[iBin][1] = fTempCache[1];
        }
        return *this;
    }


    KTPhysicalArray< 1, fftw_complex >& KTPhysicalArray< 1, fftw_complex >::operator/=(const KTPhysicalArray< 1, fftw_complex>& rhs)
    {
        if (! this->IsCompatibleWith(rhs)) return *this;
        double abs, arg;
        for (size_t iBin=0; iBin<size(); ++iBin)
        {
            abs = std::sqrt((fData[iBin][0]*fData[iBin][0] + fData[iBin][1]*fData[iBin][1]) / (rhs.fData[iBin][0]*rhs.fData[iBin][0] + rhs.fData[iBin][1]*rhs.fData[iBin][1]));
            arg = std::atan2(fData[iBin][1], fData[iBin][0]) - std::atan2(rhs.fData[iBin][1], rhs.fData[iBin][0]);
            fTempCache[0] = abs * std::cos(arg);
            fTempCache[1] = abs * std::sin(arg);
            fData[iBin][0] = fTempCache[0];
            fData[iBin][1] = fTempCache[1];
        }
        return *this;
    }


    KTPhysicalArray< 1, fftw_complex >& KTPhysicalArray< 1, fftw_complex >::operator+=(const fftw_complex& rhs)
    {
        for (size_t iBin=0; iBin<size(); ++iBin)
        {
            fData[iBin][0] = fData[iBin][0] + rhs[0];
            fData[iBin][1] = fData[iBin][1] + rhs[1];
        }
        return *this;
    }


    KTPhysicalArray< 1, fftw_complex >& KTPhysicalArray< 1, fftw_complex >::operator-=(const fftw_complex& rhs)
    {
        for (size_t iBin=0; iBin<size(); ++iBin)
        {
            fData[iBin][0] = fData[iBin][0] - rhs[0];
            fData[iBin][1] = fData[iBin][1] - rhs[1];
        }
        return *this;
    }


    KTPhysicalArray< 1, fftw_complex >& KTPhysicalArray< 1, fftw_complex >::operator*=(const fftw_complex& rhs)
    {
        for (size_t iBin=0; iBin<size(); ++iBin)
        {
            fTempCache[0] = fData[iBin][0] * rhs[0] - fData[iBin][1] * rhs[1];
            fTempCache[1] = fData[iBin][0] * rhs[1] + fData[iBin][1] * rhs[0];
            fData[iBin][0] = fTempCache[0];
            fData[iBin][1] = fTempCache[1];
        }
        return *this;
    }


    KTPhysicalArray< 1, fftw_complex >& KTPhysicalArray< 1, fftw_complex >::operator/=(const fftw_complex& rhs)
    {
        double abs, arg;
        double rhsabs = rhs[0]*rhs[0] + rhs[1]*rhs[1];
        double rhsarg = std::atan2(rhs[1], rhs[0]);
        for (size_t iBin=0; iBin<size(); ++iBin)
        {
            abs = std::sqrt((fData[iBin][0]*fData[iBin][0] + fData[iBin][1]*fData[iBin][1]) / rhsabs);
            arg = std::atan2(fData[iBin][1], fData[iBin][0]) - rhsarg;
            fTempCache[0] = abs * std::cos(arg);
            fTempCache[1] = abs * std::sin(arg);
            fData[iBin][0] = fTempCache[0];
            fData[iBin][1] = fTempCache[1];
        }
        return *this;
    }


    KTPhysicalArray< 1, fftw_complex >& KTPhysicalArray< 1, fftw_complex >::operator*=(double rhs)
    {
        for (size_t iBin=0; iBin<size(); ++iBin)
        {
            fData[iBin][0] = fData[iBin][0] * rhs;
            fData[iBin][1] = fData[iBin][1] * rhs;
        }
        return *this;
    }



    KTPhysicalArray< 1, fftw_complex >::const_iterator KTPhysicalArray< 1, fftw_complex >::begin() const
    {
        return fData;
    }


    KTPhysicalArray< 1, fftw_complex >::const_iterator KTPhysicalArray< 1, fftw_complex >::end() const
    {
        return fData + size();
    }


    KTPhysicalArray< 1, fftw_complex >::iterator KTPhysicalArray< 1, fftw_complex >::begin()
    {
        return fData;
    }


    KTPhysicalArray< 1, fftw_complex >::iterator KTPhysicalArray< 1, fftw_complex >::end()
    {
        return fData + size();
    }



    KTPhysicalArray< 1, fftw_complex >::const_reverse_iterator KTPhysicalArray< 1, fftw_complex >::rbegin() const
    {
        return fData + size() - 1;
    }


    KTPhysicalArray< 1, fftw_complex >::const_reverse_iterator KTPhysicalArray< 1, fftw_complex >::rend() const
    {
        return fData - 1;
    }


    KTPhysicalArray< 1, fftw_complex >::reverse_iterator KTPhysicalArray< 1, fftw_complex >::rbegin()
    {
        return fData + size() - 1;
    }


    KTPhysicalArray< 1, fftw_complex >::reverse_iterator KTPhysicalArray< 1, fftw_complex >::rend()
    {
        return fData - 1;
    }




    //********************************************
    // Operator implementations for fftw_complex
    //********************************************

    std::ostream&
        operator<< (std::ostream& ostr, const fftw_complex& rhs)
    {
        ostr << "(" << rhs[0] << "," << rhs[1] << ")";
        return ostr;
    }


} /* namespace Katydid */
