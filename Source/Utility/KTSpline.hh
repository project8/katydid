/*
 * KTSpline.hh
 *
 *  Created on: Dec 13, 2012
 *      Author: nsoblath
 */

#ifndef KTSPLINE_HH_
#define KTSPLINE_HH_

#include "KTPhysicalArray.hh"

#ifdef ROOT_FOUND
#include "TSpline.h"
#endif

#include <set>
#include <cstddef>
#include <memory>

namespace Katydid
{
    class KTSpline
    {
        public:
            class Implementation : public KTPhysicalArray< 1, double >
            {
                public:
                    Implementation(size_t nBins, double rangeMin=0., double rangeMax=1.) :
                        KTPhysicalArray< 1, double >(nBins, rangeMin, rangeMax),
                        fMean(0.)
                    {}
                    virtual ~Implementation() {}

                    double GetMean() const {return fMean;}
                    void SetMean(double mean) {fMean = mean;}

                private:
                    double fMean;
            };

        private:
            typedef std::set< std::shared_ptr< Implementation > > ImplementationCache;

        public:
            KTSpline();
            KTSpline(double* xVals, double* yVals, unsigned nVals);
            KTSpline(const KTSpline& orig);
            virtual ~KTSpline();

            KTSpline& operator=(const KTSpline& rhs);

            double Evaluate(double xValue);
            double Evaluate(double xValue) const;

            std::shared_ptr< Implementation > Implement(unsigned nBins, double xMin, double xMax) const;

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
            /// Retrieves a matching implementation from the cache; returns NULL if one does not exist. The matching implementation is removed from the cache and ownership is transferred to the caller.
            std::shared_ptr< Implementation > GetFromCache(unsigned nBins, double xMin, double xMax) const;

            void ClearCache() const;

        private:
            /// Adds a new spline implementation to the cache. If a matching implementation already exists in the cache, the older implementation is deleted.
            void AddToCache(std::shared_ptr< Implementation > imp) const;
            ImplementationCache::iterator FindInCache(unsigned nBins, double xMin, double XMax) const;

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
