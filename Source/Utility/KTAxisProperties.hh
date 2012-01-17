/**
 @file KTAxisProperties.hh
 @brief Contains KTAxisProperties
 @details KTAxisProperties provides physical range and bin properties in multiple dimensions.  It's meant to accompany
 multi-dimensional data-storage objects (e.g. it provides the additional information to turn a vector into a histogram or a matrix into a 2-D histogram).
 @author: N. S. Oblath
 */

#ifndef KTAXISPROPERTIES_HH_
#define KTAXISPROPERTIES_HH_

#include "KTAxisProperties_GetNBins.hh"

#include "Rtypes.h"

#include <cmath>

namespace Katydid
{
    template< size_t NDims >
    class KTAxisProperties
    {
        public:
            KTAxisProperties();
            KTAxisProperties(KTNBinsFunctor< NDims >* getNBinsFunc);
            KTAxisProperties(const KTAxisProperties< NDims >& orig);
            ~KTAxisProperties();

        public:
            KTAxisProperties< NDims >& operator=(const KTAxisProperties< NDims >& orig);

            // dimensions
        public:
            size_t GetNDimensions() const;

            // physical characteristics
        public:
            size_t GetNBins(size_t dim) const;
            void SetNBinsFunc(KTNBinsFunctor< NDims >* getNBinsFunc);

            Double_t GetBinWidth(size_t dim) const;

            Double_t GetRangeMin(size_t dim) const;
            Double_t GetRangeMax(size_t dim) const;
            void GetRange(size_t dim, Double_t& min, Double_t& max) const;

            void SetRangeMin(size_t dim, Double_t min);
            void SetRangeMin(const Double_t* mins);

            void SetRangeMax(size_t dim, Double_t max);
            void SetRangeMax(const Double_t* maxes);

            void SetRange(size_t dim, Double_t min, Double_t max);
            void SetRange(const Double_t* mins, const Double_t* maxes);

        protected:
            KTNBinsFunctor< NDims >* fGetNBinsFunc;
            Double_t fBinWidths[NDims];
            Double_t fRangeMin[NDims];
            Double_t fRangeMax[NDims];

            // bin characteristics
        public:
            Double_t GetBinLowEdge(size_t dim, size_t bin) const;
            Double_t GetBinCenter(size_t dim, size_t bin) const;

            // from physical value to bin number
        public:
            size_t FindBin(size_t dim, Double_t pos);

            // axis labels
        public:
            const std::string& GetLabel(size_t dim) const;
            void SetLabel(size_t dim, const std::string& label);

        private:
            std::string fLabels[NDims];

    };

    template< size_t NDims >
    KTAxisProperties< NDims >::KTAxisProperties()
    {
        fGetNBinsFunc = new KTDefaultNBins< NDims >();
        for (size_t iDim=0; iDim < NDims; iDim++)
        {
            fBinWidths[iDim] = 1.;
            fRangeMin[iDim] = 0.;
            fRangeMax[iDim] = 1.;
            fLabels[iDim];
        }
    }

    template< size_t NDims >
    KTAxisProperties< NDims >::KTAxisProperties(KTNBinsFunctor< NDims >* getNBinsFunc)
    {
        fGetNBinsFunc = getNBinsFunc;
        for (size_t iDim=0; iDim < NDims; iDim++)
        {
            fBinWidths[iDim] = 1.;
            fRangeMin[iDim] = 0.;
            fRangeMax[iDim] = 1.;
            fLabels[iDim];
        }
    }

    template< size_t NDims >
    KTAxisProperties< NDims >::KTAxisProperties(const KTAxisProperties< NDims >& orig)
    {
        fGetNBinsFunc = orig.fGetNBinsFunc->Clone();
        for (size_t iDim=0; iDim < NDims; iDim++)
        {
            fBinWidths[iDim] = orig.GetBinWidth(iDim);
            fRangeMin[iDim] = orig.GetRangeMin(iDim);
            fRangeMax[iDim] = orig.GetRangeMax(iDim);
            fLabels[iDim] = orig.GetLabel(iDim);
        }
    }

    template< size_t NDims >
    KTAxisProperties< NDims >::~KTAxisProperties()
    {
        delete fGetNBinsFunc;
    }

    template< size_t NDims >
    KTAxisProperties< NDims >& KTAxisProperties< NDims >::operator=(const KTAxisProperties< NDims >& orig)
    {
        fGetNBinsFunc = orig.fGetNBinsFunc->Clone();
        for (size_t iDim=0; iDim < NDims; iDim++)
        {
            fBinWidths[iDim] = orig.GetBinWidth(iDim);
            fRangeMin[iDim] = orig.GetRangeMin(iDim);
            fRangeMax[iDim] = orig.GetRangeMax(iDim);
            fLabels[iDim] = orig.GetLabel(iDim);
        }
        return *this;
    }

    template< size_t NDims >
    size_t KTAxisProperties< NDims >::GetNBins(size_t dim) const
    {
        return (*fGetNBinsFunc)(dim);
    }

    template< size_t NDims >
    void KTAxisProperties< NDims >::SetNBinsFunc(KTNBinsFunctor< NDims >* getNBinsFunc)
    {
        delete fGetNBinsFunc;
        fGetNBinsFunc = getNBinsFunc;
        return;
    }

    template< size_t NDims >
    Double_t KTAxisProperties< NDims >::GetBinWidth(size_t dim) const
    {
        return fBinWidths[dim-1];
    }

    template< size_t NDims >
    Double_t KTAxisProperties< NDims >::GetRangeMin(size_t dim) const
    {
        return fRangeMin[dim-1];
    }

    template< size_t NDims >
    Double_t KTAxisProperties< NDims >::GetRangeMax(size_t dim) const
    {
        return fRangeMax[dim-1];
    }

    template< size_t NDims >
    void KTAxisProperties< NDims >::GetRange(size_t dim, Double_t& min, Double_t& max) const
    {
        min = GetRangeMin(dim);
        max = GetRangeMax(dim);
        return;
    }

    template< size_t NDims >
    void KTAxisProperties< NDims >::SetRangeMin(size_t dim, Double_t min)
    {
        size_t arrPos = dim - 1;
        fRangeMin[arrPos] = min;
        fBinWidths[arrPos] = (fRangeMax[arrPos] - fRangeMin[arrPos]) / (Double_t)fGetNBinsFunc(dim);
        return;
    }

    template< size_t NDims >
    void KTAxisProperties< NDims >::SetRangeMin(const Double_t* mins)
    {
        for (Int_t arrPos=0; arrPos<NDims; arrPos++)
        {
            fRangeMin[arrPos] = mins[arrPos];
            fBinWidths[arrPos] = (fRangeMax[arrPos] - fRangeMin[arrPos]) / (Double_t)((*fGetNBinsFunc)(arrPos+1));
        }
    }

    template< size_t NDims >
    void KTAxisProperties< NDims >::SetRangeMax(size_t dim, Double_t max)
    {
        size_t arrPos = dim - 1;
        fRangeMax[arrPos] = max;
        fBinWidths[arrPos] = (fRangeMax[arrPos] - fRangeMin[arrPos]) / (Double_t)fGetNBinsFunc(dim);
        return;
    }

    template< size_t NDims >
    void KTAxisProperties< NDims >::SetRangeMax(const Double_t* maxes)
    {
        for (Int_t arrPos=0; arrPos<NDims; arrPos++)
        {
            fRangeMax[arrPos] = maxes[arrPos];
            fBinWidths[arrPos] = (fRangeMax[arrPos] - fRangeMin[arrPos]) / (Double_t)((*fGetNBinsFunc)(arrPos+1));
        }
    }

    template< size_t NDims >
    void KTAxisProperties< NDims >::SetRange(size_t dim, Double_t min, Double_t max)
    {
        size_t arrPos = dim - 1;
        fRangeMin[arrPos] = min;
        fRangeMax[arrPos] = max;
        fBinWidths[arrPos] = (fRangeMax[arrPos] - fRangeMin[arrPos]) / (Double_t)fGetNBinsFunc(dim);
        return;
    }

    template< size_t NDims >
    void KTAxisProperties< NDims >::SetRange(const Double_t* mins, const Double_t* maxes)
    {
        for (Int_t arrPos=0; arrPos<NDims; arrPos++)
        {
            fRangeMin[arrPos] = mins[arrPos];
            fRangeMax[arrPos] = maxes[arrPos];
            fBinWidths[arrPos] = (fRangeMax[arrPos] - fRangeMin[arrPos]) / (Double_t)fGetNBinsFunc(arrPos+1);
        }
    }

    template< size_t NDims >
    Double_t KTAxisProperties< NDims >::GetBinLowEdge(size_t dim, size_t bin) const
    {
        return fRangeMin[dim-1] + fBinWidths[dim-1] * (Double_t)bin;
    }

    template< size_t NDims >
    Double_t KTAxisProperties< NDims >::GetBinCenter(size_t dim, size_t bin) const
    {
        return fRangeMin[dim-1] + fBinWidths[dim-1] * ((Double_t)bin + 0.5);
    }

    template< size_t NDims >
    size_t KTAxisProperties< NDims >::FindBin(size_t dim, Double_t pos)
    {
        return (size_t)(floor((pos - fRangeMin[dim-1]) / fBinWidths[dim-1]));
    }

    template< size_t NDims >
    const std::string& KTAxisProperties< NDims >::GetLabel(size_t dim) const
    {
        return fLabels[dim-1];
    }

    template< size_t NDims >
    void KTAxisProperties< NDims >::SetLabel(size_t dim, const std::string& label)
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
            KTAxisProperties();
            KTAxisProperties(Double_t rangeMin, Double_t rangeMax, KTNBinsFunctor< 1 >* getNBinsFunc=NULL);
            KTAxisProperties(const KTAxisProperties< 1 >& orig);
            virtual ~KTAxisProperties();

        public:
            KTAxisProperties< 1 >& operator=(const KTAxisProperties< 1 >& orig);

            // dimensions
        public:
            size_t GetNDimensions() const;

            // physical characteristics
        public:
            size_t GetNBins() const;
            void SetNBinsFunc(KTNBinsFunctor< 1 >* getNBinsFunc);

            Double_t GetBinWidth() const;

            Double_t GetRangeMin() const;
            Double_t GetRangeMax() const;
            void GetRange(Double_t& min, Double_t& max) const;
            void SetRangeMin(Double_t min);
            void SetRangeMax(Double_t max);
            void SetRange(Double_t min, Double_t max);

        protected:
            KTNBinsFunctor< 1 >* fGetNBinsFunc;
            Double_t fBinWidth;
            Double_t fRangeMin;
            Double_t fRangeMax;

            // bin characteristics
        public:
            Double_t GetBinLowEdge(size_t bin) const;
            Double_t GetBinCenter(size_t bin) const;

            // from physical value to bin number
        public:
            size_t FindBin(Double_t pos);

            // axis label
        public:
            const std::string& GetLabel() const;
            void SetLabel(const std::string& label);

        private:
            std::string fLabel;

    };

    KTAxisProperties< 1 >::KTAxisProperties() :
            fGetNBinsFunc(new KTDefaultNBins< 1 >()),
            fBinWidth(1.),
            fRangeMin(0.),
            fRangeMax(1.),
            fLabel()
    {
    }

    KTAxisProperties< 1 >::KTAxisProperties(Double_t rangeMin, Double_t rangeMax, KTNBinsFunctor< 1 >* getNBinsFunc) :
            fGetNBinsFunc(getNBinsFunc),
            fBinWidth(1.),
            fRangeMin(rangeMin),
            fRangeMax(rangeMax),
            fLabel()
    {
        if (fGetNBinsFunc == NULL) fGetNBinsFunc = new KTDefaultNBins< 1 >();
        fBinWidth = (rangeMax - rangeMin) / (Double_t)(*fGetNBinsFunc)();
    }

    KTAxisProperties< 1 >::KTAxisProperties(const KTAxisProperties< 1 >& orig)
    {
        fGetNBinsFunc = orig.fGetNBinsFunc->Clone();
        fBinWidth = orig.GetBinWidth();
        fRangeMin = orig.GetRangeMin();
        fRangeMax = orig.GetRangeMax();
        fLabel = orig.GetLabel();
    }

    KTAxisProperties< 1 >::~KTAxisProperties()
    {
        delete fGetNBinsFunc;
    }

    KTAxisProperties< 1 >& KTAxisProperties< 1 >::operator=(const KTAxisProperties< 1 >& orig)
    {
        fGetNBinsFunc = orig.fGetNBinsFunc->Clone();
        fBinWidth = orig.GetBinWidth();
        fRangeMin = orig.GetRangeMin();
        fRangeMax = orig.GetRangeMax();
        fLabel = orig.GetLabel();
        return *this;
    }

    size_t KTAxisProperties< 1 >::GetNBins() const
    {
        return (*fGetNBinsFunc)();
    }

    void KTAxisProperties< 1 >::SetNBinsFunc(KTNBinsFunctor< 1 >* getNBinsFunc)
    {
        delete fGetNBinsFunc;
        fGetNBinsFunc = getNBinsFunc;
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
        fBinWidth = (fRangeMax - fRangeMin) / (Double_t)(*fGetNBinsFunc)();
        return;
    }

    void KTAxisProperties< 1 >::SetRangeMax(Double_t max)
    {
        fRangeMax = max;
        fBinWidth = (fRangeMax - fRangeMin) / (Double_t)(*fGetNBinsFunc)();
        return;
    }

    void KTAxisProperties< 1 >::SetRange(Double_t min, Double_t max)
    {
        SetRangeMin(min);
        SetRangeMax(max);
        fBinWidth = (fRangeMax - fRangeMin) / (Double_t)(*fGetNBinsFunc)();
        return;
    }

    Double_t KTAxisProperties< 1 >::GetBinLowEdge(size_t bin) const
    {
        return fRangeMin + fBinWidth * (Double_t)bin;
    }

    Double_t KTAxisProperties< 1 >::GetBinCenter(size_t bin) const
    {
        return fRangeMin + fBinWidth * ((Double_t)bin + 0.5);
    }

    size_t KTAxisProperties< 1 >::FindBin(Double_t pos)
    {
        return (size_t)(floor((pos - fRangeMin) / fBinWidth));
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
