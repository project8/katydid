/*
 * KTSpline.cc
 *
 *  Created on: Dec 13, 2012
 *      Author: nsoblath
 */

#include "KTSpline.hh"

#ifndef ROOT_FOUND
#include "KTLogger.hh"
#endif

namespace Katydid
{

#ifdef ROOT_FOUND

    KTSpline::KTSpline() :
            fSpline()
    {
    }

    KTSpline::KTSpline(Double_t* xVals, Double_t* yVals, UInt_t nVals) :
            fSpline("spline", xVals, yVals, nVals)
    {
    }

    KTSpline::~KTSpline()
    {
    }

    Double_t KTSpline::Evaluate(Double_t xValue)
    {
        return fSpline.Eval(xValue);
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

    Double_t KTSpline::GetXMin()
    {
        return fXMin;
    }

    Double_t KTSpline::GetXMax()
    {
        return fXMax;
    }

#endif

} /* namespace Katydid */
