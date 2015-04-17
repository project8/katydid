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
#endif

#include <list>
#include <cstddef>

namespace Katydid
{
    template< size_t, typename T>
    class KTPhysicalArray;

    class KTSpline
    {
        public:
            typedef KTPhysicalArray< 1, double > Implementation;

        private:
            typedef std::list< Implementation* > ImplementationCache;

        public:
            KTSpline();
            KTSpline(double* xVals, double* yVals, unsigned nVals);
            KTSpline(const KTSpline& orig);
            virtual ~KTSpline();

            KTSpline& operator=(const KTSpline& rhs);

            double Evaluate(double xValue);
            double Evaluate(double xValue) const;

            Implementation* Implement(unsigned nBins, double xMin, double xMax) const;

            double GetXMin() const;
            void SetXMin(double min);

            double GetXMax() const;
            void SetXMax(double max);

#ifdef ROOT_FOUND
            TSpline3* GetSpline();
#endif

        private:
#ifdef ROOT_FOUND
            TSpline3 fSpline;
#endif

            double fXMin;
            double fXMax;

        public:
            /// Adds a new spline implementation to the cache. If a matching implementation already exists in the cache, the older implementation is deleted.  Ownership of the new implementation is taken by the cache.
            void AddToCache(Implementation* imp) const;
            /// Retrieves a matching implementation from the cache; returns NULL if one does not exist. The matching implementation is removed from the cache and ownership is transferred to the caller.
            Implementation* GetFromCache(unsigned nBins, double xMin, double xMax) const;

            void ClearCache() const;

        private:
            mutable ImplementationCache fCache;

    };

    inline double KTSpline::GetXMin() const
    {
        return fXMin;
    }

    inline void KTSpline::SetXMin(double min)
    {
        fXMin = min;
        return;
    }

    inline double KTSpline::GetXMax() const
    {
        return fXMax;
    }

    inline void KTSpline::SetXMax(double max)
    {
        fXMax = max;
        return;
    }

#ifdef ROOT_FOUND
    inline TSpline3* KTSpline::GetSpline()
    {
        return &fSpline;
    }
#endif

} /* namespace Katydid */
#endif /* KTSPLINE_HH_ */
