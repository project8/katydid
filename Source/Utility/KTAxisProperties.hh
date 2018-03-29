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

#include <cmath>
#include <sys/types.h>

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

            double GetBinWidth(size_t dim) const;

            double GetRangeMin(size_t dim) const;
            double GetRangeMax(size_t dim) const;
            void GetRange(size_t dim, double& min, double& max) const;

            void SetRangeMin(size_t dim, double min);
            void SetRangeMin(const double* mins);

            void SetRangeMax(size_t dim, double max);
            void SetRangeMax(const double* maxes);

            void SetRange(size_t dim, double min, double max);
            void SetRange(const double* mins, const double* maxes);

        protected:
            const KTNBinsFunctor< NDims >* fGetNBinsFunc;
            double fBinWidths[NDims];
            double fRangeMin[NDims];
            double fRangeMax[NDims];

            // bin characteristics
        public:
            double GetBinLowEdge(size_t dim, size_t bin) const;
            double GetBinCenter(size_t dim, size_t bin) const;

            // from physical value to bin number, bounded by range (i.e. will return 0 or the last bin if pos is out of range)
        public:
            ssize_t FindBin(size_t dim, double pos) const;

            // axis labels
        public:
            const std::string& GetAxisLabel(size_t dim) const;
            void SetAxisLabel(size_t dim, const std::string& label);

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
            fLabels[arrPos] = orig.GetAxisLabel(iDim);
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
            fLabels[arrPos] = orig.GetAxisLabel(iDim);
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
    double KTAxisProperties< NDims >::GetBinWidth(size_t dim) const
    {
        return fBinWidths[dim-1];
    }

    template< size_t NDims >
    double KTAxisProperties< NDims >::GetRangeMin(size_t dim) const
    {
        return fRangeMin[dim-1];
    }

    template< size_t NDims >
    double KTAxisProperties< NDims >::GetRangeMax(size_t dim) const
    {
        return fRangeMax[dim-1];
    }

    template< size_t NDims >
    void KTAxisProperties< NDims >::GetRange(size_t dim, double& min, double& max) const
    {
        min = GetRangeMin(dim);
        max = GetRangeMax(dim);
        return;
    }

    template< size_t NDims >
    void KTAxisProperties< NDims >::SetRangeMin(size_t dim, double min)
    {
        size_t arrPos = dim - 1;
        fRangeMin[arrPos] = min;
        fBinWidths[arrPos] = (fRangeMax[arrPos] - fRangeMin[arrPos]) / (double)((*fGetNBinsFunc)(dim));
        return;
    }

    template< size_t NDims >
    void KTAxisProperties< NDims >::SetRangeMin(const double* mins)
    {
        for (unsigned arrPos=0; arrPos<NDims; arrPos++)
        {
            fRangeMin[arrPos] = mins[arrPos];
            fBinWidths[arrPos] = (fRangeMax[arrPos] - fRangeMin[arrPos]) / (double)((*fGetNBinsFunc)(arrPos+1));
        }
    }

    template< size_t NDims >
    void KTAxisProperties< NDims >::SetRangeMax(size_t dim, double max)
    {
        size_t arrPos = dim - 1;
        fRangeMax[arrPos] = max;
        fBinWidths[arrPos] = (fRangeMax[arrPos] - fRangeMin[arrPos]) / (double)((*fGetNBinsFunc)(dim));
        return;
    }

    template< size_t NDims >
    void KTAxisProperties< NDims >::SetRangeMax(const double* maxes)
    {
        for (unsigned arrPos=0; arrPos<NDims; arrPos++)
        {
            fRangeMax[arrPos] = maxes[arrPos];
            fBinWidths[arrPos] = (fRangeMax[arrPos] - fRangeMin[arrPos]) / (double)((*fGetNBinsFunc)(arrPos+1));
        }
    }

    template< size_t NDims >
    void KTAxisProperties< NDims >::SetRange(size_t dim, double min, double max)
    {
        size_t arrPos = dim - 1;
        fRangeMin[arrPos] = min;
        fRangeMax[arrPos] = max;
        fBinWidths[arrPos] = (fRangeMax[arrPos] - fRangeMin[arrPos]) / (double)((*fGetNBinsFunc)(dim));
        return;
    }

    template< size_t NDims >
    void KTAxisProperties< NDims >::SetRange(const double* mins, const double* maxes)
    {
        for (int arrPos=0; arrPos<NDims; arrPos++)
        {
            fRangeMin[arrPos] = mins[arrPos];
            fRangeMax[arrPos] = maxes[arrPos];
            fBinWidths[arrPos] = (fRangeMax[arrPos] - fRangeMin[arrPos]) / (double)((*fGetNBinsFunc)(arrPos+1));
        }
    }

    template< size_t NDims >
    double KTAxisProperties< NDims >::GetBinLowEdge(size_t dim, size_t bin) const
    {
        return fRangeMin[dim-1] + fBinWidths[dim-1] * (double)bin;
    }

    template< size_t NDims >
    double KTAxisProperties< NDims >::GetBinCenter(size_t dim, size_t bin) const
    {
        return fRangeMin[dim-1] + fBinWidths[dim-1] * ((double)bin + 0.5);
    }

    template< size_t NDims >
    ssize_t KTAxisProperties< NDims >::FindBin(size_t dim, double pos) const
    {
        return pos < fRangeMin[dim-1] ? 0 :
                pos >= fRangeMax[dim-1] ? size(dim-1) - 1 :
                        (ssize_t)(floor((pos - fRangeMin[dim-1]) / fBinWidths[dim-1]));
    }

    template< size_t NDims >
    const std::string& KTAxisProperties< NDims >::GetAxisLabel(size_t dim) const
    {
        return fLabels[dim-1];
    }

    template< size_t NDims >
    void KTAxisProperties< NDims >::SetAxisLabel(size_t dim, const std::string& label)
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
            KTAxisProperties(double rangeMin, double rangeMax, KTNBinsFunctor< 1 >* getNBinsFunc=NULL);
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

            double GetBinWidth() const;

            double GetRangeMin() const;
            double GetRangeMax() const;
            void GetRange(double& min, double& max) const;
            void SetRangeMin(double min);
            void SetRangeMax(double max);
            void SetRange(double min, double max);

        protected:
            KTNBinsFunctor< 1 >* fGetNBinsFunc;
            double fBinWidth;
            double fRangeMin;
            double fRangeMax;

            // bin characteristics
        public:
            double GetBinLowEdge(size_t bin) const;
            double GetBinCenter(size_t bin) const;

            // from physical value to bin number
        public:
            ssize_t FindBin(double pos) const;

            // axis label
        public:
            const std::string& GetAxisLabel() const;
            void SetAxisLabel(const std::string& label);

        private:
            std::string fLabel;

            //ClassDef(KTAxisProperties< 1 >, 1);

    };
} /* namespace Katydid */

#endif /* KTAXISPROPERTIES_HH_ */
