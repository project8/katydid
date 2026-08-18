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

namespace Katydid
{
    

    namespace KTMath
    {
        /* ************************* */
        /* * Fundamental constants * */
        /* * From ROOT's TMath     * */
        /* ************************* */

        constexpr double Pi()       { return 3.14159265358979323846; }
        constexpr double TwoPi()    { return 6.28318530717958623; }
        constexpr double PiOver2()  { return 1.57079632679489656; }
        constexpr double PiOver4()  { return 0.785398163397448279; }
        constexpr double InvPi()    { return 0.318309886183790691; }
        constexpr double RadToDeg() { return 57.2957795130823229; }
        constexpr double DegToRad() { return 1.74532925199432955e-02; }
        constexpr double Sqrt2()    { return 1.4142135623730950488016887242097; }

        /* ************************ */
        /* * Physical constants   * */
        /* ************************ */

        constexpr double BoltzmannConstant()       { return 1.380649e-23; }
        constexpr double ReducedPlanckConstant()   { return 1.054571817e-34; }

        /// Round to nearest integer. Rounds half integers to the nearest even integer.
        /// From ROOT's TMath
        /// This templated version should only be used for types that can be added to floating point numbers, and cast to int.
        template< typename T >
        int Nint(T x)
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
