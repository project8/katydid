/*
 * KTMath.hh
 *
 *  Created on: Sep 7, 2012
 *      Author: nsoblath
 *
 *  Many, if not all, of these are currently taken from ROOT's TMath.
 */

#ifndef KTMATH_HH_
#define KTMATH_HH_

#include "Rtypes.h"

namespace Katydid
{

    namespace KTMath
    {
        /* ************************* */
        /* * Fundamental constants * */
        /* * From ROOT's TMath     * */
        /* ************************* */

        inline Double_t Pi()       { return 3.14159265358979323846; }
        inline Double_t TwoPi()    { return 2.0 * Pi(); }
        inline Double_t PiOver2()  { return Pi() / 2.0; }
        inline Double_t PiOver4()  { return Pi() / 4.0; }
        inline Double_t InvPi()    { return 1.0 / Pi(); }
        inline Double_t RadToDeg() { return 180.0 / Pi(); }
        inline Double_t DegToRad() { return Pi() / 180.0; }
        inline Double_t Sqrt2()    { return 1.4142135623730950488016887242097; }


        /// Round to nearest integer. Rounds half integers to the nearest even integer.
        /// From ROOT's TMath
        /// This templated version should only be used for types that can be added to floating point numbers, and cast to int.
        template< typename T >
        Int_t Nint(T x)
        {

           int i;
           if (x >= 0) {
              i = int(x + 0.5);
              if (x + 0.5 == T(i) && (i & 1)) i--;
           } else {
              i = int(x - 0.5);
              if (x - 0.5 == T(i) && (i & 1)) i++;

           }
           return i;
        }

    }


} /* namespace Katydid */
#endif /* KTMATH_HH_ */
