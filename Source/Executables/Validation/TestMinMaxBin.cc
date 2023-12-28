/*
 * TestMinMaxBin.cc
 *
 *  Created on: Jul 23, 2014
 *      Author: nsoblath
 *
 *  Usage: > ./TestMinMaxBin
 *
 *  Purpose: Test GetMinimumBin and GetMaximumBin functions
 *
 *  Note: As of 7/25/14, the only function implemented (and therefore tested) is KTPhysicalArray<2, [type]>::GetMaximumBin()
 */

#include "logger.hh"
#include "KTPhysicalArray.hh"

#include <cstdlib>

using namespace Katydid;

LOGGER(testlog, "TestSmoothing");

int main()
{
    // initialize the rng
    srand(20493);

    unsigned nBins = 10;
    unsigned peakMinXBin = 2, peakMinYBin = 3;
    unsigned peakMaxXBin = 5, peakMaxYBin = 8;
    double peakMinValue = -100., peakMaxValue = 50.;

    LINFO(testlog, "Min Bin Test Parameters");
    LINFO(testlog, "Peak X bin: " << peakMinXBin);
    LINFO(testlog, "Peak Y bin: " << peakMinYBin << "  (not used in 1-D tests)\n");

    LINFO(testlog, "Max Bin Test Parameters");
    LINFO(testlog, "Peak X bin: " << peakMaxXBin);
    LINFO(testlog, "Peak Y bin: " << peakMaxYBin << "  (not used in 1-D tests)\n");

    //**************
    // 1-D Array
    //**************

    //**************
    // 2-D Array
    //**************
    LINFO(testlog, "Testing 2D GetMaximumBin");

    // Fill the array with values on [0, 10)
    KTPhysicalArray< 2, double > array2D(nBins, 0., 1., nBins, 0., 1.);
    for (unsigned iBin = 0; iBin < nBins; ++iBin)
    {
        for (unsigned iOtherBin = 0; iOtherBin < nBins; ++iOtherBin)
        {
            array2D(iBin, iOtherBin) = rand() & 10;
        }
    }
    array2D(peakMaxXBin, peakMaxYBin) = peakMaxValue;

    unsigned peakXBinFound, peakYBinFound;
    array2D.GetMaximumBin(peakXBinFound, peakYBinFound);
    LINFO(testlog, "Maximum bin found at: (" << peakXBinFound << ", " << peakYBinFound << ")");

    LINFO(testlog, "Min/Max bin test complete");

    return 0;
}
