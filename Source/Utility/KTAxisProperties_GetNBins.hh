/**
 @file KTAxisProperties_GetNBins.hh
 @brief Contains KTNBinsFunctor, and KTNBinsInArray
 @details These are functors available for deriving classes from KTAxisProperties, to make it simple to create a functor for the GetNBins function.
 @author: N. S. Oblath
 */

#ifndef KTAXISPROPERTIES_GETNBINS_HH_
#define KTAXISPROPERTIES_GETNBINS_HH_

#include "Rtypes.h"

#include <cmath>
#include <iostream>

namespace Katydid
{
    template< size_t NDims >
    class KTNBinsFunctor
    {
        public:
            KTNBinsFunctor() {}
            virtual ~KTNBinsFunctor() {}
            virtual size_t operator()(size_t dim=1) const =0;
            virtual KTNBinsFunctor< NDims >* Clone() const = 0;
    };


    //*******************************
    // general case, for N dimensions
    //*******************************


    template< size_t NDims, typename XArrayType >
    class KTNBinsInArray : public KTNBinsFunctor< NDims >
    {
        private:
            XArrayType* fPtrToArray;
            size_t (XArrayType::*fFuncPtr)(size_t);

        public:
            KTNBinsInArray(XArrayType* ptrToArray, size_t (XArrayType::*funcGetNBins)(size_t dim))
            {
                fPtrToArray = ptrToArray;
                fFuncPtr = funcGetNBins;
            }
            virtual ~KTNBinsInArray() {}

            virtual size_t operator()(size_t dim=1) const
            {
                return (*fPtrToArray.*fFuncPtr)(dim);
            }

            virtual KTNBinsFunctor< NDims >* Clone() const
            {
                return new KTNBinsInArray< NDims, XArrayType >(fPtrToArray, fFuncPtr);
            }
    };

    // specialize for anything with fixed size
    struct FixedSize {};
    template< size_t NDims >
    class KTNBinsInArray< NDims, FixedSize > : public KTNBinsFunctor< NDims >
    {
        private:
            size_t* fNBins;

        public:
            KTNBinsInArray(const size_t* nBins)
            {
                fNBins = new size_t[NDims];
                for (size_t iDim=0; iDim<NDims; iDim++)
                {
                    fNBins[iDim] = nBins[iDim];
                }
            }
            virtual ~KTNBinsInArray() {}

            virtual size_t operator()(size_t dim=1) const
            {
                return fNBins[dim-1];
            }

            virtual KTNBinsFunctor< NDims >* Clone() const
            {
                return new KTNBinsInArray< NDims, FixedSize >(fNBins);
            }
    };

    //*******************************
    // default case, for N dimensions
    //*******************************

    // default type; always returns 1
    template< size_t NDims >
    class KTDefaultNBins : public KTNBinsFunctor< NDims >
    {
        public:
            KTDefaultNBins() {}
            virtual ~KTDefaultNBins() {}

            virtual size_t operator()(size_t dim=1) const
            {
                return 1;
            }

            virtual KTNBinsFunctor< NDims >* Clone() const
            {
                return new KTDefaultNBins();
            }
    };


    //*******************************
    // specialization for 1 dimension
    //*******************************

    template<>
    class KTNBinsFunctor< 1 >
    {
        public:
            KTNBinsFunctor() {}
            virtual ~KTNBinsFunctor() {}
            virtual size_t operator()() const =0;
            virtual KTNBinsFunctor< 1 >* Clone() const =0;
    };

    template< typename XArrayType >
    class KTNBinsInArray< 1, XArrayType > : public KTNBinsFunctor< 1 >
    {
        private:
            const XArrayType* fPtrToArray;
            size_t (XArrayType::*fFuncPtr)(void) const;

        public:
            KTNBinsInArray(const XArrayType* ptrToArray, size_t (XArrayType::*funcGetNBins)() const)
            {
                fPtrToArray = ptrToArray;
                fFuncPtr = funcGetNBins;
            }
            virtual ~KTNBinsInArray() {}

            virtual size_t operator()() const
            {
                return (*fPtrToArray.*fFuncPtr)();
            }

            virtual KTNBinsFunctor< 1 >* Clone() const
            {
                return new KTNBinsInArray< 1, XArrayType >(fPtrToArray, fFuncPtr);
            }
    };

    // specialize for fixed-size arrays
    template<>
    class KTNBinsInArray< 1, FixedSize > : public KTNBinsFunctor< 1 >
    {
        private:
            size_t fNBins;

        public:
            KTNBinsInArray(size_t nBins)
            {
                fNBins = nBins;
            }
            virtual ~KTNBinsInArray() {}

            virtual size_t operator()() const
            {
                return fNBins;
            }

            virtual KTNBinsFunctor< 1 >* Clone() const
            {
                return new KTNBinsInArray< 1, FixedSize >(fNBins);
            }
    };


    //******************************
    // default case, for 1 dimension
    //******************************

    // default type; always returns 1
    template<>
    class KTDefaultNBins< 1 > : public KTNBinsFunctor< 1 >
    {
        public:
            KTDefaultNBins() {}
            virtual ~KTDefaultNBins() {}

            virtual size_t operator()() const
            {
                return 1;
            }

            virtual KTNBinsFunctor< 1 >* Clone() const
            {
                return new KTDefaultNBins< 1 >();
            }
    };



} /* namespace Katydid */


#endif /* KTAXISPROPERTIES_HH_ */
