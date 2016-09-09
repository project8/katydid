/**
 @file KTAxisProperties_GetNBins.hh
 @brief Contains KTNBinsFunctor, and KTNBinsInArray
 @details These are functors available for deriving classes from KTAxisProperties, to make it simple to create a functor for the GetNBins function.
 @author: N. S. Oblath
 */

#ifndef KTAXISPROPERTIES_GETNBINS_HH_
#define KTAXISPROPERTIES_GETNBINS_HH_

#include "KTLogger.hh"

#include <cmath>

namespace Katydid
{
    
    KTLOGGER(utillog_getnbins, "KTAxisProperties_GetNBins");

    template< size_t NDims >
    class KTNBinsFunctor
    {
        public:
            KTNBinsFunctor() {}
            virtual ~KTNBinsFunctor() {}
            virtual size_t operator()(size_t dim=1) const =0;
            virtual KTNBinsFunctor< NDims >* Clone() const = 0;

            //ClassDef(KTNBinsFunctor, 1)
    };


    //*******************************
    // general case, for N dimensions
    //*******************************


    template< size_t NDims, typename XArrayType >
    class KTNBinsInArray : public KTNBinsFunctor< NDims >
    {
        public:
            typedef size_t (XArrayType::*FuncGetNBinsByDim)(size_t dim) const;
            typedef size_t (XArrayType::*FuncGetNBinsOneDim)() const;

        private:
            XArrayType* fPtrToArray;
            //size_t (XArrayType::*fFuncPtr)(size_t);
            size_t (KTNBinsInArray::*fNBinsFuncPtr)(size_t) const;

        private:
            enum GetNBinsMode
            {
                kSingleFunc,
                kMultipleFunc
            } fMode;
            FuncGetNBinsByDim fDirectNBinsPtr;
            FuncGetNBinsOneDim* fArrayOfGetNBinsPtrs;

            size_t GetNBinsByDimDirectly(size_t dim) const
            {
                return (fPtrToArray->*fDirectNBinsPtr)(dim-1);
            }
            size_t GetNBinsByDimWithArray(size_t dim) const
            {
                if (dim > NDims) return 0;
                return (fPtrToArray->*fArrayOfGetNBinsPtrs[dim-1])();
            }


        public:
            //KTNBinsInArray(XArrayType* ptrToArray, size_t (XArrayType::*funcGetNBins)(size_t dim))
            KTNBinsInArray(XArrayType* ptrToArray, FuncGetNBinsByDim funcGetNBins) :
                    fPtrToArray(NULL),
                    fNBinsFuncPtr(NULL),
                    fMode(kSingleFunc),
                    fDirectNBinsPtr(NULL),
                    fArrayOfGetNBinsPtrs(NULL)
            {
                fPtrToArray = ptrToArray;
                fNBinsFuncPtr = &KTNBinsInArray::GetNBinsByDimDirectly;
                fDirectNBinsPtr = funcGetNBins;
            }

            KTNBinsInArray(XArrayType* ptrToArray, FuncGetNBinsOneDim* funcGetNBinsArray) :
                    fPtrToArray(NULL),
                    fNBinsFuncPtr(NULL),
                    fMode(kMultipleFunc),
                    fDirectNBinsPtr(NULL),
                    fArrayOfGetNBinsPtrs(NULL)
            {
                //KTWARN(utillog_getnbins, "in constructor for funcgetnbinsonedim, " << NDims);
                fPtrToArray = ptrToArray;
                fNBinsFuncPtr = &KTNBinsInArray::GetNBinsByDimWithArray;
                fArrayOfGetNBinsPtrs = new FuncGetNBinsOneDim [NDims];
                for (size_t arrPos=0; arrPos<NDims; arrPos++)
                {
                    fArrayOfGetNBinsPtrs[arrPos] = funcGetNBinsArray[arrPos];
                }
                //KTWARN(utillog_getnbins, fArrayOfGetNBinsPtrs[0] << "  " << fArrayOfGetNBinsPtrs[1]);
                //KTWARN(utillog_getnbins, (fPtrToArray->*fArrayOfGetNBinsPtrs[0])() << "  " << (fPtrToArray->*fArrayOfGetNBinsPtrs[1])());
            }

            virtual ~KTNBinsInArray()
            {
                if (fMode == kMultipleFunc)
                    delete [] fArrayOfGetNBinsPtrs;
            }

            virtual size_t operator()(size_t dim=1) const
            {
                return (this->*fNBinsFuncPtr)(dim);
            }

            virtual KTNBinsFunctor< NDims >* Clone() const
            {
                if (fMode == kSingleFunc)
                {
                    return new KTNBinsInArray< NDims, XArrayType >(fPtrToArray, fDirectNBinsPtr);
                }
                else if (fMode == kMultipleFunc)
                {
                    return new KTNBinsInArray< NDims, XArrayType >(fPtrToArray, fArrayOfGetNBinsPtrs);
                }
                KTERROR(utillog_getnbins, "Error in KTNBinsFunctor< " << NDims << " >::Clone: unknown NBins mode");
                return NULL;
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
                for (size_t arrPos=0; arrPos<NDims; arrPos++)
                {
                    fNBins[arrPos] = nBins[arrPos];
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
                return dim;
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
                return (fPtrToArray->*fFuncPtr)();
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
