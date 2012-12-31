/*
 * KTSpline.cc
 *
 *  Created on: Dec 13, 2012
 *      Author: nsoblath
 */

#include "KTSpline.hh"

#include "KTPhysicalArray.hh"

#ifndef ROOT_FOUND
#include "KTLogger.hh"
#endif

namespace Katydid
{

#ifdef ROOT_FOUND

    KTSpline::KTSpline() :
            fSpline(),
            fCache()
    {
    }

    KTSpline::KTSpline(Double_t* xVals, Double_t* yVals, UInt_t nVals) :
            fSpline("spline", xVals, yVals, nVals),
            fCache()
    {
    }

    KTSpline::~KTSpline()
    {
        ClearCache();
    }

    Double_t KTSpline::Evaluate(Double_t xValue)
    {
        return fSpline.Eval(xValue);
    }

    Double_t KTSpline::Evaluate(Double_t xValue) const
    {
        return fSpline.Eval(xValue);
    }

    KTPhysicalArray< 1, Double_t >* KTSpline::Implement(UInt_t nBins, Double_t xMin, Double_t xMax) const
    {
        Implementation* imp = GetFromCache(nBins, xMin, xMax);
        if (imp != NULL) return imp;

        imp = new KTPhysicalArray< 1, Double_t >(nBins, xMin, xMax);
        for (UInt_t iBin=0; iBin < nBins; iBin++)
        {
            (*imp)(iBin) = Evaluate(imp->GetBinCenter(iBin));
        }
        return imp;
    }


    Double_t KTSpline::GetXMin()
    {
        return fSpline.GetXmin();
    }

    Double_t KTSpline::GetXMax()
    {
        return fSpline.GetXmax();
    }


#else

    KTLOGGER(splinelog, "katydid.utility");

    KTSpline::KTSpline() :
            fXMin(0.),
            fXMax(1.)
    {
        KTERROR(splinelog, "Non-ROOT version of KTSpline is not fully functional. Stop now, or else!!!");
    }

    KTSpline::KTSpline(Double_t* xVals, Double_t* yVals, UInt_t nVals) :
            fXMin(xVals[0]),
            fXMax(xVals[nVals-1])
    {
        KTERROR(splinelog, "Non-ROOT version of KTSpline is not fully functional. Stop now, or else!!!");
    }

    KTSpline::~KTSpline()
    {
    }

    Double_t KTSpline::Evaluate(Double_t xValue)
    {
      return 1.;
    }

    Double_t KTSpline::Evaluate(Double_t xValue) const
    {
        return 1.;
    }

    Double_t KTSpline::GetXMin()
    {
        return fXMin;
    }

    Double_t KTSpline::GetXMax()
    {
        return fXMax;
    }

#endif

    void KTSpline::AddToCache(Implementation* imp) const
    {
        Implementation* oldImp = GetFromCache(imp->size(), imp->GetRangeMin(), imp->GetRangeMax());
        if (oldImp != NULL) delete oldImp;

        fCache.push_front(imp);
        return;
    }

    KTSpline::Implementation* KTSpline::GetFromCache(UInt_t nBins, Double_t xMin, Double_t xMax) const
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
