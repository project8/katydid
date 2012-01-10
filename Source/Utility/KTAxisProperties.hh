/**
 @file KTAxisProperties.hh
 @brief Contains KTAxisProperties
 @details KTAxisProperties provides physical range and bin properties in multiple dimensions.  It's meant to accompany
 multi-dimensional data-storage objects (e.g. it provides the additional information to turn a vector into a histogram or a matrix into a 2-D histogram).
 @author: N. S. Oblath
 */

#ifndef KTAXISPROPERTIES_HH_
#define KTAXISPROPERTIES_HH_

#include "Rtypes.h"

#include <cmath>

namespace Katydid
{
    template< UInt_t NDims >
    class KTAxisProperties
    {
        public:
            enum Dimension
            {
                X=1,
                Y=2,
                Z=3
            };

        public:
            KTAxisProperties();
            KTAxisProperties(const KTAxisProperties< NDims >& orig);
            ~KTAxisProperties();

        public:
            KTAxisProperties< NDims >& operator=(const KTAxisProperties< NDims >& orig);

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

            // axis labels
        public:
            const std::string& GetLabel(UInt_t dim) const;
            void SetLabel(UInt_t dim, const std::string& label);

        private:
            std::string fLabels[NDims];

    };

    template< UInt_t NDims >
    KTAxisProperties< NDims >::KTAxisProperties()
    {
        for (UInt_t iDim=0; iDim < NDims; iDim++)
        {
            fNBins[iDim] = 1;
            fBinWidths[iDim] = 1.;
            fRangeMin[iDim] = 0.;
            fRangeMax[iDim] = 1.;
            fLabels[iDim];
        }
    }

    template< UInt_t NDims >
    KTAxisProperties< NDims >::KTAxisProperties(const KTAxisProperties< NDims >& orig)
    {
        for (UInt_t iDim=0; iDim < NDims; iDim++)
        {
            fNBins[iDim] = orig.GetNBins(iDim);
            fBinWidths[iDim] = orig.GetBinWidth(iDim);
            fRangeMin[iDim] = orig.GetRangeMin(iDim);
            fRangeMax[iDim] = orig.GetRangeMax(iDim);
            fLabels[iDim] = orig.GetLabel(iDim);
        }
    }

    template< UInt_t NDims >
    KTAxisProperties< NDims >::~KTAxisProperties()
    {
    }

    template< UInt_t NDims >
    KTAxisProperties< NDims >& KTAxisProperties< NDims >::operator=(const KTAxisProperties< NDims >& orig)
    {
        for (UInt_t iDim=0; iDim < NDims; iDim++)
        {
            fNBins[iDim] = orig.GetNBins(iDim);
            fBinWidths[iDim] = orig.GetBinWidth(iDim);
            fRangeMin[iDim] = orig.GetRangeMin(iDim);
            fRangeMax[iDim] = orig.GetRangeMax(iDim);
            fLabels[iDim] = orig.GetLabel(iDim);
        }
        return *this;
    }

    template< UInt_t NDims >
    UInt_t KTAxisProperties< NDims >::GetNBins(UInt_t dim) const
    {
        return fNBins[dim-1];
    }

    template< UInt_t NDims >
    void KTAxisProperties< NDims >::SetNBins(UInt_t dim, UInt_t nBins)
    {
        UInt_t arrPos = dim - 1;
        fNBins[arrPos] = nBins;
        fBinWidths[arrPos] = (fRangeMax[arrPos] - fRangeMin[arrPos]) / (Double_t)fNBins[arrPos];
        return;
    }

    template< UInt_t NDims >
    void KTAxisProperties< NDims >::SetNBins(const UInt_t* nBinses)
    {
        for (Int_t arrPos=0; arrPos<NDims; arrPos++)
        {
            fNBins[arrPos] = nBinses[arrPos];
            fBinWidths[arrPos] = (fRangeMax[arrPos] - fRangeMin[arrPos]) / (Double_t)fNBins[arrPos];
        }
        return;
    }

    template< UInt_t NDims >
    Double_t KTAxisProperties< NDims >::GetBinWidth(UInt_t dim) const
    {
        return fBinWidths[dim-1];
    }

    template< UInt_t NDims >
    Double_t KTAxisProperties< NDims >::GetRangeMin(UInt_t dim) const
    {
        return fRangeMin[dim-1];
    }

    template< UInt_t NDims >
    Double_t KTAxisProperties< NDims >::GetRangeMax(UInt_t dim) const
    {
        return fRangeMax[dim-1];
    }

    template< UInt_t NDims >
    void KTAxisProperties< NDims >::GetRange(UInt_t dim, Double_t& min, Double_t& max) const
    {
        min = GetRangeMin(dim);
        max = GetRangeMax(dim);
        return;
    }

    template< UInt_t NDims >
    void KTAxisProperties< NDims >::SetRangeMin(UInt_t dim, Double_t min)
    {
        UInt_t arrPos = dim - 1;
        fRangeMin[arrPos] = min;
        fBinWidths[arrPos] = (fRangeMax[arrPos] - fRangeMin[arrPos]) / (Double_t)fNBins[arrPos];
        return;
    }

    template< UInt_t NDims >
    void KTAxisProperties< NDims >::SetRangeMin(const Double_t* mins)
    {
        for (Int_t arrPos=0; arrPos<NDims; arrPos++)
        {
            fRangeMin[arrPos] = mins[arrPos];
            fBinWidths[arrPos] = (fRangeMax[arrPos] - fRangeMin[arrPos]) / (Double_t)fNBins[arrPos];
        }
    }

    template< UInt_t NDims >
    void KTAxisProperties< NDims >::SetRangeMax(UInt_t dim, Double_t max)
    {
        UInt_t arrPos = dim - 1;
        fRangeMax[arrPos] = max;
        fBinWidths[arrPos] = (fRangeMax[arrPos] - fRangeMin[arrPos]) / (Double_t)fNBins[arrPos];
        return;
    }

    template< UInt_t NDims >
    void KTAxisProperties< NDims >::SetRangeMax(const Double_t* maxes)
    {
        for (Int_t arrPos=0; arrPos<NDims; arrPos++)
        {
            fRangeMax[arrPos] = maxes[arrPos];
            fBinWidths[arrPos] = (fRangeMax[arrPos] - fRangeMin[arrPos]) / (Double_t)fNBins[arrPos];
        }
    }

    template< UInt_t NDims >
    void KTAxisProperties< NDims >::SetRange(UInt_t dim, Double_t min, Double_t max)
    {
        UInt_t arrPos = dim - 1;
        fRangeMin[arrPos] = min;
        fRangeMax[arrPos] = max;
        fBinWidths[arrPos] = (fRangeMax[arrPos] - fRangeMin[arrPos]) / (Double_t)fNBins[arrPos];
        return;
    }

    template< UInt_t NDims >
    void KTAxisProperties< NDims >::SetRange(const Double_t* mins, const Double_t* maxes)
    {
        for (Int_t arrPos=0; arrPos<NDims; arrPos++)
        {
            fRangeMin[arrPos] = mins[arrPos];
            fRangeMax[arrPos] = maxes[arrPos];
            fBinWidths[arrPos] = (fRangeMax[arrPos] - fRangeMin[arrPos]) / (Double_t)fNBins[arrPos];
        }
    }

    template< UInt_t NDims >
    Double_t KTAxisProperties< NDims >::GetBinLowEdge(UInt_t dim, UInt_t bin) const
    {
        return fRangeMin[dim-1] + fBinWidths[dim-1] * (Double_t)bin;
    }

    template< UInt_t NDims >
    Double_t KTAxisProperties< NDims >::GetBinCenter(UInt_t dim, UInt_t bin) const
    {
        return fRangeMin[dim-1] + fBinWidths[dim-1] * ((Double_t)bin + 0.5);
    }

    template< UInt_t NDims >
    UInt_t KTAxisProperties< NDims >::FindBin(UInt_t dim, Double_t pos)
    {
        return (UInt_t)(floor((pos - fRangeMin[dim-1]) / fBinWidths[dim-1]));
    }

    template< UInt_t NDims >
    const std::string& KTAxisProperties< NDims >::GetLabel(UInt_t dim) const
    {
        return fLabels[dim-1];
    }

    template< UInt_t NDims >
    void KTAxisProperties< NDims >::SetLabel(UInt_t dim, const std::string& label)
    {
        fLabels[dim-1] = label;
        return;
    }


} /* namespace Katydid */


//********************************
// specialization for 1 dimension
//********************************

namespace Katydid
{
    template<>
    class KTAxisProperties< 1 >
    {
        public:
            enum Dimension
            {
                X=1,
                Y=2,
                Z=3
            };

        public:
            KTAxisProperties();
            KTAxisProperties(UInt_t nBins, Double_t rangeMin, Double_t rangeMax);
            KTAxisProperties(const KTAxisProperties< 1 >& orig);
            virtual ~KTAxisProperties();

        public:
            KTAxisProperties< 1 >& operator=(const KTAxisProperties< 1 >& orig);

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
            Double_t fBinWidth;
            Double_t fRangeMin;
            Double_t fRangeMax;

            // bin characteristics
        public:
            Double_t GetBinLowEdge(UInt_t bin) const;
            Double_t GetBinCenter(UInt_t bin) const;

            // from physical value to bin number
        public:
            UInt_t FindBin(Double_t pos);

            // axis label
        public:
            const std::string& GetLabel() const;
            void SetLabel(const std::string& label);

        private:
            std::string fLabel;

    };

    KTAxisProperties< 1 >::KTAxisProperties() :
            fNBins(1),
            fBinWidth(1.),
            fRangeMin(0.),
            fRangeMax(1.),
            fLabel()
    {
    }

    KTAxisProperties< 1 >::KTAxisProperties(UInt_t nBins, Double_t rangeMin, Double_t rangeMax) :
            fNBins(nBins),
            fBinWidth((rangeMax - rangeMin) / (Double_t)nBins),
            fRangeMin(rangeMin),
            fRangeMax(rangeMax),
            fLabel()
    {
    }

    KTAxisProperties< 1 >::KTAxisProperties(const KTAxisProperties< 1 >& orig)
    {
        fNBins = orig.GetNBins();
        fBinWidth = orig.GetBinWidth();
        fRangeMin = orig.GetRangeMin();
        fRangeMax = orig.GetRangeMax();
        fLabel = orig.GetLabel();
    }

    KTAxisProperties< 1 >::~KTAxisProperties()
    {
    }

    KTAxisProperties< 1 >& KTAxisProperties< 1 >::operator=(const KTAxisProperties< 1 >& orig)
    {
        fNBins = orig.GetNBins();
        fBinWidth = orig.GetBinWidth();
        fRangeMin = orig.GetRangeMin();
        fRangeMax = orig.GetRangeMax();
        fLabel = orig.GetLabel();
        return *this;
    }

    UInt_t KTAxisProperties< 1 >::GetNBins() const
    {
        return fNBins;
    }

    void KTAxisProperties< 1 >::SetNBins(UInt_t nBins)
    {
        fNBins = nBins;
        fBinWidth = (fRangeMax - fRangeMin) / (Double_t)fNBins;
        return;
    }

    Double_t KTAxisProperties< 1 >::GetBinWidth() const
    {
        return fBinWidth;
    }

    Double_t KTAxisProperties< 1 >::GetRangeMin() const
    {
        return fRangeMin;
    }

    Double_t KTAxisProperties< 1 >::GetRangeMax() const
    {
        return fRangeMax;
    }

    void KTAxisProperties< 1 >::GetRange(Double_t& min, Double_t& max) const
    {
        min = GetRangeMin();
        max = GetRangeMax();
        return;
    }

    void KTAxisProperties< 1 >::SetRangeMin(Double_t min)
    {
        fRangeMin = min;
        fBinWidth = (fRangeMax - fRangeMin) / (Double_t)fNBins;
        return;
    }

    void KTAxisProperties< 1 >::SetRangeMax(Double_t max)
    {
        fRangeMax = max;
        fBinWidth = (fRangeMax - fRangeMin) / (Double_t)fNBins;
        return;
    }

    void KTAxisProperties< 1 >::SetRange(Double_t min, Double_t max)
    {
        SetRangeMin(min);
        SetRangeMax(max);
        fBinWidth = (fRangeMax - fRangeMin) / (Double_t)fNBins;
        return;
    }

    Double_t KTAxisProperties< 1 >::GetBinLowEdge(UInt_t bin) const
    {
        return fRangeMin + fBinWidth * (Double_t)bin;
    }

    Double_t KTAxisProperties< 1 >::GetBinCenter(UInt_t bin) const
    {
        return fRangeMin + fBinWidth * ((Double_t)bin + 0.5);
    }

    UInt_t KTAxisProperties< 1 >::FindBin(Double_t pos)
    {
        return (UInt_t)(floor((pos - fRangeMin) / fBinWidth));
    }

    const std::string& KTAxisProperties< 1 >::GetLabel() const
    {
        return fLabel;
    }

    void KTAxisProperties< 1 >::SetLabel(const std::string& label)
    {
        fLabel = label;
        return;
    }

} /* namespace Katydid */


#endif /* KTAXISPROPERTIES_HH_ */
