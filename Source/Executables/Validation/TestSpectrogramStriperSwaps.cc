/*
 * TestSpectrogramStriperSwaps.cc
 *
 *  Created on: Aug 15, 2017
 *      Author: obla999
 */

#include "KTSpectrogramStriper.hh"

using namespace Katydid;


int main()
{
    KTSpectrogramStriper striper;

    striper.SetStripeSize(10);
    striper.SetStripeOverlap(3);

    striper.Configure(nullptr);

    return 0;
}
