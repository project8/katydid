/*
 * TestAxisProperties.cc
 *
 *  Created on: Dec 26, 2011
 *      Author: nsoblath
 */


#include "KTAxisProperties.hh"
#include "KTAxisProperties.hh"

#ifdef ROOT_FOUND
#include "TH1.h"
#include "TH2.h"
#endif

#include <cmath>
#include <iostream>

using namespace Katydid;
using namespace std;

int main()
{
    size_t nBins1 = 100;
    double rangeMin1 = 1.0;
    double rangeMax1 = 2.0;

    double testPosition1 = 1.82235;
    size_t testBin1 = 35;

    size_t nBins2 = 541;
    double rangeMin2 = -23;
    double rangeMax2 = 2039;

    double testPosition2 = 820;
    size_t testBin2 = 18;

    // Test the 1-D array
    cout << "One-dimensional test" << endl;

    KTNBinsInArray< 1, FixedSize >* nBinsFunctor = new KTNBinsInArray< 1, FixedSize >(nBins1);
    KTAxisProperties< 1 > array(rangeMin1, rangeMax1, nBinsFunctor);

    cout << "Axis properties setup with nbins = " << array.size() << ", range_min = " << array.GetRangeMin() << ", and range_max = " << array.GetRangeMax() << endl;
    cout << "The bin width is " << array.GetBinWidth() << endl;
    cout << "The test position (" << testPosition1 << ") is in bin " << array.FindBin(testPosition1) << endl;
    cout << "The test bin (" << testBin1 << ") has a low edge of " << array.GetBinLowEdge(testBin1) << " and a bin center of " << array.GetBinCenter(testBin1) << endl;

#ifdef ROOT_FOUND
    TH1I histCompare("histCompare", "histCompare", nBins1, rangeMin1, rangeMax1);
    // relative magnitude floating point comparisons
    double tolerance = 1.e-10;
    bool theyAgree =
            fabs(array.GetBinWidth() - histCompare.GetBinWidth(testBin1+1)) / histCompare.GetBinWidth(testBin1) < tolerance &&
            (int)array.FindBin(testPosition1) == histCompare.FindBin(testPosition1)-1 &&
            fabs(array.GetBinLowEdge(testBin1) - histCompare.GetBinLowEdge(testBin1+1)) / histCompare.GetBinLowEdge(testBin1) < tolerance &&
            fabs(array.GetBinCenter(testBin1) - histCompare.GetBinCenter(testBin1+1)) / histCompare.GetBinCenter(testBin1) < tolerance;

    if (! theyAgree)
    {
        cout << "The KTAxisProperties calculations disagree with TH1!" << endl;
        cout << "  " << histCompare.GetBinWidth(testBin1+1) << "  " << histCompare.FindBin(testPosition1) << "  " << histCompare.GetBinLowEdge(testBin1+1) << "  " << histCompare.GetBinCenter(testBin1+1) << endl;
        return -1;
    }

    cout << "The KTAxisProperties calculations agree with TH1" << endl;
#endif

    // Test a 2-D array
    cout << "Two-dimensional test" << endl;

    size_t nBinses [2] = {nBins1, nBins2};
    KTNBinsInArray< 2, FixedSize >* nBinsFunctor2 = new KTNBinsInArray< 2, FixedSize >(nBinses);
    KTAxisProperties< 2 > array2D;
    double rangeMins [2] = {rangeMin1, rangeMin2};
    double rangeMaxes [2] = {rangeMax1, rangeMax2};
    array2D.SetNBinsFunc(nBinsFunctor2);
    array2D.SetRangeMin(rangeMins);
    array2D.SetRangeMax(rangeMaxes);

    cout << "Axis properties setup with nbins (x,y) = (" << array2D.size(1) << "," << array2D.size(2) << "), range_min = (" << array2D.GetRangeMin(1) << "," << array2D.GetRangeMin(2) << "), and range_max = (" << array2D.GetRangeMax(1) << "," << array2D.GetRangeMax(2) << ")" << endl;
    cout << "The bin widths are " << array2D.GetBinWidth(1) << " and " << array2D.GetBinWidth(2) << endl;
    cout << "The test position 1 (" << testPosition1 << ") is in bin " << array2D.FindBin(1, testPosition1) << endl;
    cout << "The test position 2 (" << testPosition2 << ") is in bin " << array2D.FindBin(2, testPosition2) << endl;
    cout << "The test bin 1 (" << testBin1 << ") has a low edge of " << array2D.GetBinLowEdge(1, testBin1) << " and a bin center of " << array2D.GetBinCenter(1, testBin1) << endl;
    cout << "The test bin 2 (" << testBin2 << ") has a low edge of " << array2D.GetBinLowEdge(2, testBin2) << " and a bin center of " << array2D.GetBinCenter(2, testBin2) << endl;

#ifdef ROOT_FOUND
    TH2I hist2DCompare("hist2DCompare", "hist2DCompare", nBins1, rangeMin1, rangeMax1, nBins2, rangeMin2, rangeMax2);
    TAxis* xAxis = hist2DCompare.GetXaxis();
    TAxis* yAxis = hist2DCompare.GetYaxis();
    // relative magnitude floating point comparisons
    theyAgree =
            fabs(array2D.GetBinWidth(1) - xAxis->GetBinWidth(testBin1+1)) / xAxis->GetBinWidth(testBin1) < tolerance &&
            (int)array2D.FindBin(1, testPosition1) == xAxis->FindBin(testPosition1)-1 &&
            fabs(array2D.GetBinLowEdge(1, testBin1) - xAxis->GetBinLowEdge(testBin1+1)) / xAxis->GetBinLowEdge(testBin1) < tolerance &&
            fabs(array2D.GetBinCenter(1, testBin1) - xAxis->GetBinCenter(testBin1+1)) / xAxis->GetBinCenter(testBin1) < tolerance;

    if (! theyAgree)
    {
        cout << "The KTAxisProperties calculations disagree with the x-axis of TH2!" << endl;
        cout << "  " << xAxis->GetBinWidth(testBin1+1) << "  " << xAxis->FindBin(testPosition1) << "  " << xAxis->GetBinLowEdge(testBin1+1) << "  " << xAxis->GetBinCenter(testBin1+1) << endl;
        return -1;
    }

    theyAgree =
            fabs(array2D.GetBinWidth(2) - yAxis->GetBinWidth(testBin2+1)) / yAxis->GetBinWidth(testBin2) < tolerance &&
            (int)array2D.FindBin(2, testPosition2) == yAxis->FindBin(testPosition2)-1 &&
            fabs(array2D.GetBinLowEdge(2, testBin2) - yAxis->GetBinLowEdge(testBin2+1)) / yAxis->GetBinLowEdge(testBin2) < tolerance &&
            fabs(array2D.GetBinCenter(2, testBin2) - yAxis->GetBinCenter(testBin2+1)) / yAxis->GetBinCenter(testBin2) < tolerance;

    if (! theyAgree)
    {
        cout << "The KTAxisProperties calculations disagree with the y-axis of TH2!" << endl;
        cout << "  " << yAxis->GetBinWidth(testBin2+1) << "  " << yAxis->FindBin(testPosition2) << "  " << yAxis->GetBinLowEdge(testBin2+1) << "  " << yAxis->GetBinCenter(testBin2+1) << endl;
        return -1;
    }


    cout << "The KTAxisProperties calculations agree with TH2" << endl;
#endif

    return 0;
}
