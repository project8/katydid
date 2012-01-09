/**
 @file KTPhysicalArray.hh
 @brief Contains KTPhysicalArray
 @details KTPhysicalArray provides physical range and bin properties in multiple dimensions.  It's meant to accompany
 multi-dimensional data-storage objects (e.g. it provides the additional information to turn a vector into a histogram or a matrix into a 2-D histogram).
 @author: N. S. Oblath
 */

#ifndef KTPHYSICALARRAY_HH_
#define KTPHYSICALARRAY_HH_

#include "Rtypes.h"

#include <cmath>

namespace Katydid
{
    template< UInt_t NDims >
    class KTPhysicalArray
    {
        public:
            enum Dimension
            {
                X=1,
                Y=2,
                Z=3
            };

        public:
            KTPhysicalArray();
            KTPhysicalArray(const KTPhysicalArray< NDims >& orig);
            ~KTPhysicalArray();

        public:
            KTPhysicalArray< NDims >& operator=(const KTPhysicalArray< NDims >& orig);

            // dimensions
        public:
            UInt_t GetNDimensions() const;

            // physical characteristics
        public:
            UInt_t GetNBins(UInt_t dim) const;
            void SetNBins(UInt_t dim, UInt_t nBins);
            void SetNBins(const UInt_t* nBinses);

            Double_t GetBinWidth(UInt_t dim) const;

            Double_t GetRangeMin(UInt_t dim) const;
            Double_t GetRangeMax(UInt_t dim) const;
            void GetRange(UInt_t dim, Double_t& min, Double_t& max) const;

            void SetRangeMin(UInt_t dim, Double_t min);
            void SetRangeMin(const Double_t* mins);

            void SetRangeMax(UInt_t dim, Double_t max);
            void SetRangeMax(const Double_t* maxes);

            void SetRange(UInt_t dim, Double_t min, Double_t max);
            void SetRange(const Double_t* mins, const Double_t* maxes);

        protected:
            UInt_t fNBins[NDims];
            Double_t fBinWidths[NDims];
            Double_t fRangeMin[NDims];
            Double_t fRangeMax[NDims];

            // bin characteristics
        public:
            Double_t GetBinLowEdge(UInt_t dim, UInt_t bin) const;
            Double_t GetBinCenter(UInt_t dim, UInt_t bin) const;

            // from physical value to bin number
        public:
            UInt_t FindBin(UInt_t dim, Double_t pos);

    };

    template< UInt_t NDims >
    KTPhysicalArray< NDims >::KTPhysicalArray()
    {
        for (UInt_t iDim=0; iDim < NDims; iDim++)
        {
            fNBins[iDim] = 1;
            fBinWidths[iDim] = 1.;
            fRangeMin[iDim] = 0.;
            fRangeMax[iDim] = 1.;
        }
    }

    template< UInt_t NDims >
    KTPhysicalArray< NDims >::KTPhysicalArray(const KTPhysicalArray< NDims >& orig)
    {
        for (UInt_t iDim=0; iDim < NDims; iDim++)
        {
            fNBins[iDim] = orig.GetNBins(iDim);
            fBinWidths[iDim] = orig.GetBinWidth(iDim);
            fRangeMin[iDim] = orig.GetRangeMin(iDim);
            fRangeMax[iDim] = orig.GetRangeMax(iDim);
        }
    }

    template< UInt_t NDims >
    KTPhysicalArray< NDims >::~KTPhysicalArray()
    {
    }

    template< UInt_t NDims >
    KTPhysicalArray< NDims >& KTPhysicalArray< NDims >::operator=(const KTPhysicalArray< NDims >& orig)
    {
        for (UInt_t iDim=0; iDim < NDims; iDim++)
        {
            fNBins[iDim] = orig.GetNBins(iDim);
            fBinWidths[iDim] = orig.GetBinWidth(iDim);
            fRangeMin[iDim] = orig.GetRangeMin(iDim);
            fRangeMax[iDim] = orig.GetRangeMax(iDim);
        }
        return *this;
    }

    template< UInt_t NDims >
    UInt_t KTPhysicalArray< NDims >::GetNBins(UInt_t dim) const
    {
        return fNBins[dim-1];
    }

    template< UInt_t NDims >
    void KTPhysicalArray< NDims >::SetNBins(UInt_t dim, UInt_t nBins)
    {
        UInt_t arrPos = dim - 1;
        fNBins[arrPos] = nBins;
        fBinWidths[arrPos] = (fRangeMax[arrPos] - fRangeMin[arrPos]) / (Double_t)fNBins[arrPos];
        return;
    }

    template< UInt_t NDims >
    void KTPhysicalArray< NDims >::SetNBins(const UInt_t* nBinses)
    {
        for (Int_t arrPos=0; arrPos<NDims; arrPos++)
        {
            fNBins[arrPos] = nBinses[arrPos];
            fBinWidths[arrPos] = (fRangeMax[arrPos] - fRangeMin[arrPos]) / (Double_t)fNBins[arrPos];
        }
        return;
    }

    template< UInt_t NDims >
    Double_t KTPhysicalArray< NDims >::GetBinWidth(UInt_t dim) const
    {
        return fBinWidths[dim-1];
    }

    template< UInt_t NDims >
    Double_t KTPhysicalArray< NDims >::GetRangeMin(UInt_t dim) const
    {
        return fRangeMin[dim-1];
    }

    template< UInt_t NDims >
    Double_t KTPhysicalArray< NDims >::GetRangeMax(UInt_t dim) const
    {
        return fRangeMax[dim-1];
    }

    template< UInt_t NDims >
    void KTPhysicalArray< NDims >::GetRange(UInt_t dim, Double_t& min, Double_t& max) const
    {
        min = GetRangeMin(dim);
        max = GetRangeMax(dim);
        return;
    }

    template< UInt_t NDims >
    void KTPhysicalArray< NDims >::SetRangeMin(UInt_t dim, Double_t min)
    {
        UInt_t arrPos = dim - 1;
        fRangeMin[arrPos] = min;
        fBinWidths[arrPos] = (fRangeMax[arrPos] - fRangeMin[arrPos]) / (Double_t)fNBins[arrPos];
        return;
    }

    template< UInt_t NDims >
    void KTPhysicalArray< NDims >::SetRangeMin(const Double_t* mins)
    {
        for (Int_t arrPos=0; arrPos<NDims; arrPos++)
        {
            fRangeMin[arrPos] = mins[arrPos];
            fBinWidths[arrPos] = (fRangeMax[arrPos] - fRangeMin[arrPos]) / (Double_t)fNBins[arrPos];
        }
    }

    template< UInt_t NDims >
    void KTPhysicalArray< NDims >::SetRangeMax(UInt_t dim, Double_t max)
    {
        UInt_t arrPos = dim - 1;
        fRangeMax[arrPos] = max;
        fBinWidths[arrPos] = (fRangeMax[arrPos] - fRangeMin[arrPos]) / (Double_t)fNBins[arrPos];
        return;
    }

    template< UInt_t NDims >
    void KTPhysicalArray< NDims >::SetRangeMax(const Double_t* maxes)
    {
        for (Int_t arrPos=0; arrPos<NDims; arrPos++)
        {
            fRangeMax[arrPos] = maxes[arrPos];
            fBinWidths[arrPos] = (fRangeMax[arrPos] - fRangeMin[arrPos]) / (Double_t)fNBins[arrPos];
        }
    }

    template< UInt_t NDims >
    void KTPhysicalArray< NDims >::SetRange(UInt_t dim, Double_t min, Double_t max)
    {
        UInt_t arrPos = dim - 1;
        fRangeMin[arrPos] = min;
        fRangeMax[arrPos] = max;
        fBinWidths[arrPos] = (fRangeMax[arrPos] - fRangeMin[arrPos]) / (Double_t)fNBins[arrPos];
        return;
    }

    template< UInt_t NDims >
    void KTPhysicalArray< NDims >::SetRange(const Double_t* mins, const Double_t* maxes)
    {
        for (Int_t arrPos=0; arrPos<NDims; arrPos++)
        {
            fRangeMin[arrPos] = mins[arrPos];
            fRangeMax[arrPos] = maxes[arrPos];
            fBinWidths[arrPos] = (fRangeMax[arrPos] - fRangeMin[arrPos]) / (Double_t)fNBins[arrPos];
        }
    }

    template< UInt_t NDims >
    Double_t KTPhysicalArray< NDims >::GetBinLowEdge(UInt_t dim, UInt_t bin) const
    {
        return fRangeMin[dim-1] + fBinWidths[dim-1] * (Double_t)bin;
    }

    template< UInt_t NDims >
    Double_t KTPhysicalArray< NDims >::GetBinCenter(UInt_t dim, UInt_t bin) const
    {
        return fRangeMin[dim-1] + fBinWidths[dim-1] * ((Double_t)bin + 0.5);
    }

    template< UInt_t NDims >
    UInt_t KTPhysicalArray< NDims >::FindBin(UInt_t dim, Double_t pos)
    {
        return (UInt_t)(floor((pos - fRangeMin[dim-1]) / fBinWidths[dim-1]));
    }

} /* namespace Katydid */


//********************************
// specialization for 1 dimension
//********************************

namespace Katydid
{
    template<>
    class KTPhysicalArray< 1 >
    {
        public:
            enum Dimension
            {
                X=1,
                Y=2,
                Z=3
            };

        public:
            KTPhysicalArray();
            KTPhysicalArray(UInt_t nBins, Double_t rangeMin, Double_t rangeMax);
            KTPhysicalArray(const KTPhysicalArray< 1 >& orig);
            virtual ~KTPhysicalArray();

        public:
            KTPhysicalArray< 1 >& operator=(const KTPhysicalArray< 1 >& orig);

            // dimensions
        public:
            UInt_t GetNDimensions() const;

            // physical characteristics
        public:
            UInt_t GetNBins() const;
            void SetNBins(UInt_t nBins);

            Double_t GetBinWidth() const;

            Double_t GetRangeMin() const;
            Double_t GetRangeMax() const;
            void GetRange(Double_t& min, Double_t& max) const;
            void SetRangeMin(Double_t min);
            void SetRangeMax(Double_t max);
            void SetRange(Double_t min, Double_t max);

        protected:
            UInt_t fNBins;
            Double_t fBinWidths;
            Double_t fRangeMin;
            Double_t fRangeMax;

            // bin characteristics
        public:
            Double_t GetBinLowEdge(UInt_t bin) const;
            Double_t GetBinCenter(UInt_t bin) const;

            // from physical value to bin number
        public:
            UInt_t FindBin(Double_t pos);

    };

    KTPhysicalArray< 1 >::KTPhysicalArray() :
            fNBins(1),
            fBinWidths(1.),
            fRangeMin(0.),
            fRangeMax(1.)
    {
    }

    KTPhysicalArray< 1 >::KTPhysicalArray(UInt_t nBins, Double_t rangeMin, Double_t rangeMax) :
            fNBins(nBins),
            fBinWidths((rangeMax - rangeMin) / (Double_t)nBins),
            fRangeMin(rangeMin),
            fRangeMax(rangeMax)
    {
    }

    KTPhysicalArray< 1 >::KTPhysicalArray(const KTPhysicalArray< 1 >& orig)
    {
        fNBins = orig.GetNBins();
        fBinWidths = orig.GetBinWidth();
        fRangeMin = orig.GetRangeMin();
        fRangeMax = orig.GetRangeMax();
    }

    KTPhysicalArray< 1 >::~KTPhysicalArray()
    {
    }

    KTPhysicalArray< 1 >& KTPhysicalArray< 1 >::operator=(const KTPhysicalArray< 1 >& orig)
    {
        fNBins = orig.GetNBins();
        fBinWidths = orig.GetBinWidth();
        fRangeMin = orig.GetRangeMin();
        fRangeMax = orig.GetRangeMax();
        return *this;
    }

    UInt_t KTPhysicalArray< 1 >::GetNBins() const
    {
        return fNBins;
    }

    void KTPhysicalArray< 1 >::SetNBins(UInt_t nBins)
    {
        fNBins = nBins;
        fBinWidths = (fRangeMax - fRangeMin) / (Double_t)fNBins;
        return;
    }

    Double_t KTPhysicalArray< 1 >::GetBinWidth() const
    {
        return fBinWidths;
    }

    Double_t KTPhysicalArray< 1 >::GetRangeMin() const
    {
        return fRangeMin;
    }

    Double_t KTPhysicalArray< 1 >::GetRangeMax() const
    {
        return fRangeMax;
    }

    void KTPhysicalArray< 1 >::GetRange(Double_t& min, Double_t& max) const
    {
        min = GetRangeMin();
        max = GetRangeMax();
        return;
    }

    void KTPhysicalArray< 1 >::SetRangeMin(Double_t min)
    {
        fRangeMin = min;
        fBinWidths = (fRangeMax - fRangeMin) / (Double_t)fNBins;
        return;
    }

    void KTPhysicalArray< 1 >::SetRangeMax(Double_t max)
    {
        fRangeMax = max;
        fBinWidths = (fRangeMax - fRangeMin) / (Double_t)fNBins;
        return;
    }

    void KTPhysicalArray< 1 >::SetRange(Double_t min, Double_t max)
    {
        SetRangeMin(min);
        SetRangeMax(max);
        fBinWidths = (fRangeMax - fRangeMin) / (Double_t)fNBins;
        return;
    }

    Double_t KTPhysicalArray< 1 >::GetBinLowEdge(UInt_t bin) const
    {
        return fRangeMin + fBinWidths * (Double_t)bin;
    }

    Double_t KTPhysicalArray< 1 >::GetBinCenter(UInt_t bin) const
    {
        return fRangeMin + fBinWidths * ((Double_t)bin + 0.5);
    }

    UInt_t KTPhysicalArray< 1 >::FindBin(Double_t pos)
    {
        return (UInt_t)(floor((pos - fRangeMin) / fBinWidths));
    }

} /* namespace Katydid */


#endif /* KTPHYSICALARRAY_HH_ */
