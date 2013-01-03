/*
 * KTSpline.hh
 *
 *  Created on: Dec 13, 2012
 *      Author: nsoblath
 */

#ifndef KTSPLINE_HH_
#define KTSPLINE_HH_

#ifdef ROOT_FOUND
#include "TSpline.h"
#else
#include "Rtypes.h"
#endif

#include <list>

namespace Katydid
{
    template< size_t, typename T>
    class KTPhysicalArray;

    class KTSpline
    {
        public:
            typedef KTPhysicalArray< 1, Double_t > Implementation;

        private:
            typedef std::list< Implementation* > ImplementationCache;

        public:
            KTSpline();
            KTSpline(Double_t* xVals, Double_t* yVals, UInt_t nVals);
            virtual ~KTSpline();

            Double_t Evaluate(Double_t xValue);
            Double_t Evaluate(Double_t xValue) const;

            Implementation* Implement(UInt_t nBins, Double_t xMin, Double_t xMax) const;

            Double_t GetXMin();
            Double_t GetXMax();

        private:

#ifdef ROOT_FOUND
            TSpline3 fSpline;

#else
            Double_t fXMin;
            Double_t fXMax;
#endif

        public:
            /// Adds a new spline implementation to the cache. If a matching implementation already exists in the cache, the older implementation is deleted.  Ownership of the new implementation is taken by the cache.
            void AddToCache(Implementation* imp) const;
            /// Retrieves a matching implemtation from the cache; returns NULL if one does not exist. The matching implementation is removed from the cache and ownership is transferred to the caller.
            Implementation* GetFromCache(UInt_t nBins, Double_t xMin, Double_t xMax) const;

            void ClearCache() const;

        private:
            mutable ImplementationCache fCache;

    };

} /* namespace Katydid */
#endif /* KTSPLINE_HH_ */
