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

namespace Katydid
{

    class KTSpline
    {
        public:
            KTSpline();
            KTSpline(Double_t* xVals, Double_t* yVals, UInt_t nVals);
            virtual ~KTSpline();

            Double_t Evaluate(Double_t xValue);

            Double_t GetXMin();
            Double_t GetXMax();

        private:

#ifdef ROOT_FOUND
            TSpline3 fSpline;

#else
            Double_t fXMin;
            Double_t fXMax;
#endif

    };

} /* namespace Katydid */
#endif /* KTSPLINE_HH_ */
