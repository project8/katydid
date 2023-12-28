/*
 * KTSpline.cc
 *
 *  Created on: Dec 13, 2012
 *      Author: nsoblath
 */

#include "KTSpline.hh"

#include "logger.hh"
#include "KTPhysicalArray.hh"

LOGGER(splinelog, "KTSpline");

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

    std::shared_ptr< KTSpline::Implementation > KTSpline::Implement(unsigned nBins, double xMin, double xMax) const
    {
        std::shared_ptr< Implementation > imp = GetFromCache(nBins, xMin, xMax);
        if (imp != NULL) return imp;

        LDEBUG(splinelog, "Creating new spline implementation for (" << nBins << ", " << xMin << ", " << xMax << ")");
        imp = std::make_shared< Implementation >(nBins, xMin, xMax);
        double mean = 0.;
        for (unsigned iBin=0; iBin < nBins; iBin++)
        {
            (*imp)(iBin) = Evaluate(imp->GetBinCenter(iBin));
            mean += (*imp)(iBin);
            //LWARN(splinelog, (*imp)(iBin) << "  " << mean);
        }
        imp->SetMean(mean / (double)nBins);
        LDEBUG(splinelog, "Calculated implementation mean: " << imp->GetMean());
        AddToCache(imp);
        return imp;
    }


#else

    KTSpline::KTSpline() :
            fXMin(0.),
            fXMax(0.)
    {
        LERROR(splinelog, "Non-ROOT version of KTSpline is not fully functional. Stop now, or else!!!");
    }

    KTSpline::KTSpline(double* xVals, double* yVals, unsigned nVals) :
            fXMin(xVals[0]),
            fXMax(xVals[nVals-1])
    {
        LERROR(splinelog, "Non-ROOT version of KTSpline is not fully functional. Stop now, or else!!!");
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

    KTPhysicalArray< 1, double >* KTSpline::Implement(unsigned nBins, double xMin, double xMax) const
    {
        return NULL;
    }

#endif

    void KTSpline::AddToCache(std::shared_ptr< Implementation > imp) const
    {
        ImplementationCache::iterator it = FindInCache(imp->size(), imp->GetRangeMin(), imp->GetRangeMax());
        if (it != fCache.end()) fCache.erase(it);

        fCache.insert(imp);
        return;
    }

    std::shared_ptr< KTSpline::Implementation > KTSpline::GetFromCache(unsigned nBins, double xMin, double xMax) const
    {
        ImplementationCache::iterator it = FindInCache(nBins, xMin, xMax);
        if (it != fCache.end())
        {
            std::shared_ptr< Implementation > imp = *it;
            return imp;
        }
        return std::shared_ptr< Implementation >();
    }

    void KTSpline::ClearCache() const
    {
        fCache.clear();
    }

    KTSpline::ImplementationCache::iterator KTSpline::FindInCache(unsigned nBins, double xMin, double xMax) const
    {
        for (ImplementationCache::iterator it = fCache.begin(); it != fCache.end(); it++)
        {
            if ((*it)->size() == nBins && (*it)->GetRangeMin() == xMin && (*it)->GetRangeMax() == xMax)
            {
                return it;
            }
        }
        return fCache.end();
    }
} /* namespace Katydid */
