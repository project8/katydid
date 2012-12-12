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
    /*!
     @class KTAxisProperties
     @author N. S. Oblath

     @brief Encapsulates the properties of n-dimensional binned axes.

     @details
     Provides the number of bins and axis ranges for n-dimensional axes.  This is intended to be combined
     with array- or vector-like storage classes.

     @note
     Dimensions are numbered on the interval [1, NDims].

     \tparam <NDims> {Number of dimensions}
    */

    template< size_t NDims >
    class KTAxisProperties
    {
        public:
            KTAxisProperties();
            KTAxisProperties(KTNBinsFunctor< NDims >* getNBinsFunc);
            KTAxisProperties(const KTAxisProperties< NDims >& orig);
            virtual ~KTAxisProperties();

        public:
            KTAxisProperties< NDims >& operator=(const KTAxisProperties< NDims >& orig);

            // dimensions
        public:
            size_t GetNDimensions() const;

            // physical characteristics
        public:
            bool empty() const;
            size_t size(size_t dim) const;
            size_t GetNBins(size_t dim) const;
            void SetNBinsFunc(const KTNBinsFunctor< NDims >* getNBinsFunc);

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
            const KTNBinsFunctor< NDims >* fGetNBinsFunc;
            Double_t fBinWidths[NDims];
            Double_t fRangeMin[NDims];
            Double_t fRangeMax[NDims];

            // bin characteristics
        public:
            Double_t GetBinLowEdge(size_t dim, size_t bin) const;
            Double_t GetBinCenter(size_t dim, size_t bin) const;

            // from physical value to bin number
        public:
            size_t FindBin(size_t dim, Double_t pos) const;

            // axis labels
        public:
            const std::string& GetLabel(size_t dim) const;
            void SetLabel(size_t dim, const std::string& label);

        private:
            std::string fLabels[NDims];

            //ClassDef(KTAxisProperties, 1)

    };

    template< size_t NDims >
    KTAxisProperties< NDims >::KTAxisProperties()
    {
        fGetNBinsFunc = new KTDefaultNBins< NDims >();
        size_t arrPos;
        for (size_t iDim=1; iDim <= NDims; iDim++)
        {
            arrPos = iDim - 1;
            fBinWidths[arrPos] = 1.;
            fRangeMin[arrPos] = 0.;
            fRangeMax[arrPos] = 1.;
            fLabels[arrPos] = std::string("");
        }
    }

    template< size_t NDims >
    KTAxisProperties< NDims >::KTAxisProperties(KTNBinsFunctor< NDims >* getNBinsFunc)
    {
        fGetNBinsFunc = getNBinsFunc;
        size_t arrPos;
        for (size_t iDim=1; iDim <= NDims; iDim++)
        {
            arrPos = iDim - 1;
            fBinWidths[arrPos] = 1.;
            fRangeMin[arrPos] = 0.;
            fRangeMax[arrPos] = 1.;
            fLabels[arrPos] = std::string("");
        }
    }

    template< size_t NDims >
    KTAxisProperties< NDims >::KTAxisProperties(const KTAxisProperties< NDims >& orig)
    {
        fGetNBinsFunc = orig.fGetNBinsFunc->Clone();
        size_t arrPos;
        for (size_t iDim=1; iDim <= NDims; iDim++)
        {
            arrPos = iDim - 1;
            fBinWidths[arrPos] = orig.GetBinWidth(iDim);
            fRangeMin[arrPos] = orig.GetRangeMin(iDim);
            fRangeMax[arrPos] = orig.GetRangeMax(iDim);
            fLabels[arrPos] = orig.GetLabel(iDim);
        }
    }

    template< size_t NDims >
    KTAxisProperties< NDims >::~KTAxisProperties()
    {
        delete fGetNBinsFunc;
    }

    template< size_t NDims >
    size_t KTAxisProperties< NDims >::GetNDimensions() const
    {
        return NDims;
    }

    template< size_t NDims >
    KTAxisProperties< NDims >& KTAxisProperties< NDims >::operator=(const KTAxisProperties< NDims >& orig)
    {
        fGetNBinsFunc = orig.fGetNBinsFunc->Clone();
        size_t arrPos;
        for (size_t iDim=1; iDim <= NDims; iDim++)
        {
            arrPos = iDim - 1;
            fBinWidths[arrPos] = orig.GetBinWidth(iDim);
            fRangeMin[arrPos] = orig.GetRangeMin(iDim);
            fRangeMax[arrPos] = orig.GetRangeMax(iDim);
            fLabels[arrPos] = orig.GetLabel(iDim);
        }
        return *this;
    }

    template< size_t NDims >
    bool KTAxisProperties< NDims >::empty() const
    {
        bool isEmpty = true;
        for (size_t iDim=1; iDim <= NDims; iDim++)
        {
            isEmpty = isEmpty && size(iDim) == 0;
        }
        return isEmpty;
    }

    template< size_t NDims >
    size_t KTAxisProperties< NDims >::size(size_t dim) const
    {
        return (*fGetNBinsFunc)(dim);
    }

    template< size_t NDims >
    size_t KTAxisProperties< NDims >::GetNBins(size_t dim) const
    {
        return (*fGetNBinsFunc)(dim);
    }

    template< size_t NDims >
    void KTAxisProperties< NDims >::SetNBinsFunc(const KTNBinsFunctor< NDims >* getNBinsFunc)
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
        fBinWidths[arrPos] = (fRangeMax[arrPos] - fRangeMin[arrPos]) / (Double_t)((*fGetNBinsFunc)(dim));
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
        fBinWidths[arrPos] = (fRangeMax[arrPos] - fRangeMin[arrPos]) / (Double_t)((*fGetNBinsFunc)(dim));
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
        fBinWidths[arrPos] = (fRangeMax[arrPos] - fRangeMin[arrPos]) / (Double_t)((*fGetNBinsFunc)(dim));
        return;
    }

    template< size_t NDims >
    void KTAxisProperties< NDims >::SetRange(const Double_t* mins, const Double_t* maxes)
    {
        for (Int_t arrPos=0; arrPos<NDims; arrPos++)
        {
            fRangeMin[arrPos] = mins[arrPos];
            fRangeMax[arrPos] = maxes[arrPos];
            fBinWidths[arrPos] = (fRangeMax[arrPos] - fRangeMin[arrPos]) / (Double_t)((*fGetNBinsFunc)(arrPos+1));
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
    size_t KTAxisProperties< NDims >::FindBin(size_t dim, Double_t pos) const
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
    /*!
     @class KTAxisProperties< 1 >
     @author N. S. Oblath

     @brief Specialization of KTAxisProperties; encapsulates the properties of 1-dimensional binned axes.

     @details
     Provides the number of bins and axis ranges for 1-dimensional axis.  This is intended to be combined
     with an array- or vector-like storage class.
    */

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
            bool empty() const;
            size_t size() const;
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
            size_t FindBin(Double_t pos) const;

            // axis label
        public:
            const std::string& GetLabel() const;
            void SetLabel(const std::string& label);

        private:
            std::string fLabel;

            //ClassDef(KTAxisProperties< 1 >, 1);

    };
} /* namespace Katydid */

#endif /* KTAXISPROPERTIES_HH_ */
