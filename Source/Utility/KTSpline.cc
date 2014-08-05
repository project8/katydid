/*
 * KTSpline.cc
 *
 *  Created on: Dec 13, 2012
 *      Author: nsoblath
 */

#include "KTSpline.hh"

#include "KTLogger.hh"
#include "KTPhysicalArray.hh"

KTLOGGER(splinelog, "KTSpline");

namespace Katydid
{

#ifdef ROOT_FOUND

    KTSpline::KTSpline() :
            fSpline(),
            fXMin(0.),
            fXMax(0.),
            fCache()
    {
    }

    KTSpline::KTSpline(double* xVals, double* yVals, unsigned nVals) :
            fSpline("spline", xVals, yVals, nVals),
            fXMin(xVals[0]),
            fXMax(xVals[nVals-1]),
            fCache()
    {
    }

    KTSpline::KTSpline(const KTSpline& orig) :
            fSpline(orig.fSpline),
            fXMin(orig.fXMin),
            fXMax(orig.fXMax),
            fCache()
    {}

    KTSpline::~KTSpline()
    {
        ClearCache();
    }

    KTSpline& KTSpline::operator=(const KTSpline& rhs)
    {
        fSpline = rhs.fSpline;
        fXMin = rhs.fXMin;
        fXMax = rhs.fXMax;
        fCache.clear();
        return *this;
    }

    double KTSpline::Evaluate(double xValue)
    {
        return fSpline.Eval(xValue);
    }

    double KTSpline::Evaluate(double xValue) const
    {
        return fSpline.Eval(xValue);
    }

    KTPhysicalArray< 1, double >* KTSpline::Implement(unsigned nBins, double xMin, double xMax) const
    {
        Implementation* imp = GetFromCache(nBins, xMin, xMax);
        if (imp != NULL) return imp;

        KTDEBUG(splinelog, "Creating new spline implementation for (" << nBins << ", " << xMin << ", " << xMax << ")");
        imp = new KTPhysicalArray< 1, double >(nBins, xMin, xMax);
        for (unsigned iBin=0; iBin < nBins; iBin++)
        {
            (*imp)(iBin) = Evaluate(imp->GetBinCenter(iBin));
        }
        return imp;
    }


#else

    KTSpline::KTSpline() :
            fXMin(0.),
            fXMax(0.)
    {
        KTERROR(splinelog, "Non-ROOT version of KTSpline is not fully functional. Stop now, or else!!!");
    }

    KTSpline::KTSpline(double* xVals, double* yVals, unsigned nVals) :
            fXMin(xVals[0]),
            fXMax(xVals[nVals-1])
    {
        KTERROR(splinelog, "Non-ROOT version of KTSpline is not fully functional. Stop now, or else!!!");
    }

    KTSpline::KTSpline(const KTSpline& orig) :
            fXMin(orig.fXMin),
            fXMax(orig.fXMax)
    {
    }

    KTSpline::~KTSpline()
    {
    }

    KTSpline& KTSpline::operator=(const KTSpline& rhs)
    {
        fXMin = rhs.fXMin;
        fXMax = rhs.fXMax;
        return *this;
    }

    double KTSpline::Evaluate(double xValue)
    {
      return 1.;
    }

    double KTSpline::Evaluate(double xValue) const
    {
        return 1.;
    }

#endif

    void KTSpline::AddToCache(Implementation* imp) const
    {
        Implementation* oldImp = GetFromCache(imp->size(), imp->GetRangeMin(), imp->GetRangeMax());
        if (oldImp != NULL) delete oldImp;

        fCache.push_front(imp);
        return;
    }

    KTSpline::Implementation* KTSpline::GetFromCache(unsigned nBins, double xMin, double xMax) const
    {
        for (ImplementationCache::iterator it = fCache.begin(); it != fCache.end(); it++)
        {
            if ((*it)->size() == nBins && (*it)->GetRangeMin() == xMin && (*it)->GetRangeMax() == xMax)
            {
                Implementation* imp = *it;
                fCache.erase(it);
                return imp;
            }
        }
        return NULL;
    }

    void KTSpline::ClearCache() const
    {
        while (! fCache.empty())
        {
            delete fCache.back();
            fCache.pop_back();
        }
    }


} /* namespace Katydid */
