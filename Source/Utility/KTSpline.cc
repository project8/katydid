/*
 * KTSpline.hh
 *
 *  Created on: Dec 13, 2012
 *      Author: nsoblath
 *
 *  Edited on: April 24, 2026
 *      Author: ehtkarim
 * 
 *   - The KTSpline class now implements its own natural cubic spline interpolation, and no longer depends on ROOT.
 *   - The old ROOT TSpline3-based implementation is removed.
 */

#include "KTSpline.hh"

#include "KTLogger.hh"
#include "KTPhysicalArray.hh"

#include <algorithm>

KTLOGGER(splinelog, "KTSpline");

namespace Katydid
{

    KTSpline::KTSpline() :
            fX(),
            fA(),
            fB(),
            fC(),
            fD(),
            fXMin(0.),
            fXMax(0.),
            fCache()
    {
    }

    KTSpline::KTSpline(double* xVals, double* yVals, unsigned nVals) :
            fX(),
            fA(),
            fB(),
            fC(),
            fD(),
            fXMin(0.),
            fXMax(0.),
            fCache()
    {
        if (xVals == NULL || yVals == NULL)
        {
            KTERROR(splinelog, "Cannot build a spline from null input arrays!");
            return;
        }

        if (nVals < 2)
        {
            KTERROR(splinelog, "At least two points are required to build a spline");
            return;
        }

        for (unsigned iVal = 1; iVal < nVals; ++iVal)
        {
            if (xVals[iVal] <= xVals[iVal-1])
            {
                KTERROR(splinelog, "Input x values must be strictly increasing!");
                return;
            }
        }

        fXMin = xVals[0];
        fXMax = xVals[nVals-1];

        fX.assign(xVals, xVals + nVals);
        fA.assign(yVals, yVals + nVals);
        fB.assign(nVals - 1, 0.0);
        fC.assign(nVals,     0.0);
        fD.assign(nVals - 1, 0.0);

        std::vector< double > h(nVals - 1, 0.0);
        for (unsigned iVal = 0; iVal < nVals - 1; ++iVal) h[iVal] = fX[iVal+1] - fX[iVal];

        std::vector< double > alpha(nVals, 0.0);
        for (unsigned iVal = 1; iVal < nVals - 1; ++iVal)
        {
            alpha[iVal] = 3.0/h[iVal]*(fA[iVal+1]-fA[iVal]) - 3.0/h[iVal-1]*(fA[iVal]-fA[iVal-1]);
        }

        std::vector< double > l(nVals, 0.0), mu(nVals, 0.0), z(nVals, 0.0);
        l[0] = 1.0;
        mu[0] = 0.0;
        z[0] = 0.0;

        for (unsigned iVal = 1; iVal < nVals - 1; ++iVal)
        {
            l[iVal] = 2.0*(fX[iVal+1]-fX[iVal-1]) - h[iVal-1]*mu[iVal-1];
            if (l[iVal] == 0.0)
            {
                KTERROR(splinelog, "Encountered singular spline system at i = " << iVal);
                fX.clear(); fA.clear(); fB.clear(); fC.clear(); fD.clear();
                fXMin = 0.; fXMax = 0.;
                return;
            }
            mu[iVal] = h[iVal]/l[iVal];
            z[iVal] = (alpha[iVal] - h[iVal-1]*z[iVal-1]) / l[iVal];
        }

        l[nVals-1] = 1.0;
        z[nVals-1] = 0.0;
        fC[nVals-1] = 0.0;

        for (int jVal = static_cast<int>(nVals) - 2; jVal >= 0; --jVal)
        {
            fC[jVal] = z[jVal] - mu[jVal]*fC[jVal+1];
            fB[jVal] = (fA[jVal+1]-fA[jVal])/h[jVal] - h[jVal]*(fC[jVal+1] + 2.0*fC[jVal])/3.0;
            fD[jVal] = (fC[jVal+1]-fC[jVal]) / (3.0*h[jVal]);
        }
    }

    KTSpline::KTSpline(const KTSpline& orig) :
            fX(orig.fX),
            fA(orig.fA),
            fB(orig.fB),
            fC(orig.fC),
            fD(orig.fD),
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
        if (this == &rhs) return *this;
        fX = rhs.fX;
        fA = rhs.fA;
        fB = rhs.fB;
        fC = rhs.fC;
        fD = rhs.fD;
        fXMin = rhs.fXMin;
        fXMax = rhs.fXMax;
        fCache.clear();
        return *this;
    }

    double KTSpline::Evaluate(double xValue)
    {
        return static_cast<const KTSpline*>(this)->Evaluate(xValue);
    }

    double KTSpline::Evaluate(double xValue) const
    {
        if (fX.empty()) return 0.0;

        if (xValue <= fX.front()) return fA.front();
        if (xValue >= fX.back())  return fA.back();

        std::vector< double >::const_iterator it = std::upper_bound(fX.begin(), fX.end(), xValue);
        size_t j = static_cast<size_t>(it - fX.begin() - 1);

        double dx = xValue - fX[j];
        return fA[j] + fB[j]*dx + fC[j]*dx*dx + fD[j]*dx*dx*dx;
    }

    std::shared_ptr< KTSpline::Implementation > KTSpline::Implement(unsigned nBins, double xMin, double xMax) const
    {
        std::shared_ptr< Implementation > imp = GetFromCache(nBins, xMin, xMax);
        if (imp != NULL) return imp;

        KTDEBUG(splinelog, "Creating new spline implementation for (" << nBins << ", " << xMin << ", " << xMax << ")");
        imp = std::make_shared< Implementation >(nBins, xMin, xMax);

        if (nBins == 0)
        {
            imp->SetMean(0.0);
            AddToCache(imp);
            return imp;
        }

        double mean = 0.;
        for (unsigned iBin=0; iBin < nBins; iBin++)
        {
            (*imp)(iBin) = Evaluate(imp->GetBinCenter(iBin));
            mean += (*imp)(iBin);
            //KTWARN(splinelog, (*imp)(iBin) << "  " << mean);
        }
        imp->SetMean(mean / (double)nBins);
        KTDEBUG(splinelog, "Calculated implementation mean: " << imp->GetMean());
        AddToCache(imp);
        return imp;
    }

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
