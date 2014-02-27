/*
 * KTDigitizerTests.cc
 *
 *  Created on: December 17, 2013
 *      Author: N. Oblath
 */

#include "KTDigitizerTests.hh"

#include "KTDigitizerTestData.hh"
#include "KTLogger.hh"
#include "KTNOFactory.hh"
#include "KTParam.hh"
#include "KTRawTimeSeries.hh"
#include "KTRawTimeSeriesData.hh"
#include "KTTimeSeriesFFTW.hh"
#include "KTTimeSeriesReal.hh"
#include "KTTimeSeriesData.hh"

#include <cmath>

using boost::shared_ptr;

namespace Katydid
{
    KTLOGGER(dtlog, "KTDigitizerTests");

    static KTNORegistrar< KTProcessor, KTDigitizerTests > sDigTestRegistrar("digitizer-tests");

    KTDigitizerTests::KTDigitizerTests(const std::string& name) :
            KTProcessor(name),
            fNDigitizerBits(8),
            fNDigitizerLevels(pow(2, fNDigitizerBits)),
            fTestBitOccupancy(true),
            fTestClipping(true),
            fTestLinearity(true),
            fBinsPerAverage(50),
            fRawTestFuncs(),
            fBitOccupancyTestID(0),
            fClippingTestID(0),
            fLinearityTestID(0),
            fDigTestSignal("dig-test", this),
            fDigTestRawSlot("raw-ts", this, &KTDigitizerTests::RunTests, &fDigTestSignal)
    {
        unsigned id = 0;

        fBitOccupancyTestID = ++id;
        fClippingTestID = ++id;
        fLinearityTestID = ++id;

        SetTestBitOccupancy(fTestBitOccupancy);
        SetTestClipping(fTestClipping);
        SetTestLinearity(fTestLinearity);
    }

    KTDigitizerTests::~KTDigitizerTests()
    {
    }

    bool KTDigitizerTests::Configure(const KTParamNode* node)
    {
        if (node == NULL) return false;

        fNDigitizerBits = node->GetValue< unsigned >("n-digitizer-bits", fNDigitizerBits);

        SetTestBitOccupancy(node->GetValue< bool >("test-bit-occupancy", fTestBitOccupancy));

        SetTestClipping(node->GetValue< bool >("test-clipping", fTestClipping));

        SetTestLinearity(node->GetValue< bool >("test-linearity", fTestLinearity));

        KTWARN(dtlog, "fTestLinearity is " << fTestLinearity);

        return true;
    }

    bool KTDigitizerTests::RunTests(KTRawTimeSeriesData& data)
    {
        unsigned nComponents = data.GetNComponents();
        KTWARN(dtlog, "Size of fRawTestFuncs = " << fRawTestFuncs.size());
        KTDigitizerTestData& dtData = data.Of< KTDigitizerTestData >().SetNComponents(nComponents);
        for (unsigned component = 0; component < nComponents; ++component)
        {
            const KTRawTimeSeries* ts = static_cast< const KTRawTimeSeries* >(data.GetTimeSeries(component));
            for (TestFuncs::const_iterator func_it = fRawTestFuncs.begin(); func_it != fRawTestFuncs.end(); ++func_it)
            {
                (this->*(func_it->second))(ts, dtData, component);
            }
        }
        return true;
    }

    bool KTDigitizerTests::BitOccupancyTest(const KTRawTimeSeries* ts, KTDigitizerTestData& testData, unsigned component)
    {
        KTDEBUG(dtlog, "Running Bit Occupancy test");
        testData.SetBitOccupancyFlag(true);
        size_t nBins = ts->size();
        for (size_t iBin = 0; iBin < nBins; ++iBin)
        {
            testData.AddBits((*ts)(iBin), component);
        }
        return true;
    }

    bool KTDigitizerTests::ClippingTest(const KTRawTimeSeries* ts, KTDigitizerTestData& testData, unsigned component)
    {
        KTDEBUG(dtlog, "Running Clipping test");
        testData.SetClippingFlag(true);
        size_t nBins = ts->size();
        unsigned nClipTop = 0, nClipBottom = 0;
        unsigned nMultClipTop = 0, nMultClipBottom = 0;
        for (size_t iBin = 0; iBin < nBins; ++iBin) //Find all max/min
        {
            if ((*ts)(iBin) >= fNDigitizerLevels-1)
            {
                ++nClipTop;
            }
            if ((*ts)(iBin) <= 0)
            {
                ++nClipBottom;
            }
        }
        for (size_t iBin = 1; iBin < nBins-1; ++iBin) //Find all sequential max/min except last and first
        {
            if ((*ts)(iBin) >= fNDigitizerLevels-1 && ((*ts)(iBin+1) >= fNDigitizerLevels-1 || (*ts)(iBin-1) >= fNDigitizerLevels-1))
            {
                ++nMultClipTop;
            }
            if ((*ts)(iBin) <= 0 && ((*ts)(iBin+1) <= 0 || (*ts)(iBin-1) <= 0))
            {
                ++nMultClipBottom;
            }
        }
        if ((*ts)(0) >= fNDigitizerLevels-1 && (*ts)(1) >= fNDigitizerLevels-1) //Find if first bin is sequential max
        {
            ++nMultClipTop;
        }
        if ((*ts)(0) <= 0 && (*ts)(1) <= 0) //Find if first bin is sequential min
        {
            ++nMultClipBottom;
        }
        if ((*ts)(nBins-1) >= fNDigitizerLevels-1 && (*ts)(nBins-2) >= fNDigitizerLevels-1) //Find if last bin is sequential max
        {
            ++nMultClipTop;
        }
        if ((*ts)(nBins-1) <= 0 && (*ts)(nBins-2) <= 0) //Find if last bin is sequential min
        {
            ++nMultClipBottom;
        }

        /*
		///hello i'm finding pairs now//
	    for (size_t iBin = 1; iBin < nBins-2; ++iBin) //Find all sequential max/min pairs except last and first
        {
	        if ((*ts)(iBin) >= fNDigitizerLevels-1 && (*ts)(iBin+1) >= fNDigitizerLevels-1 && (*ts)(iBin+2) < fNDigitizerLevels-1 && (*ts)(iBin-1) < fNDigitizerLevels-1)
	        {
	            ++nMultClipTop;
	        }
            if ((*ts)(iBin) <= 0 && (*ts)(iBin+1) <= 0 && (*ts)(iBin+2) > 0 && (*ts)(iBin-1) > 0)
	        {
	            ++nMultClipBottom;
	        }
        }
	    if ((*ts)(0) >= fNDigitizerLevels-1 && (*ts)(1) >= fNDigitizerLevels-1 && (*ts)(2) < fNDigitizerLevels-1) //Find if first bin is sequential max pair
 	    {
	        ++nMultClipTop;
	    }
	    if ((*ts)(0)<=0 && (*ts)(1)<=0 && (*ts)(2)>0) //Find if first bin is sequential min pair
	    {
	        ++nMultClipBottom;
	    }
	    if ((*ts)(nBins-1) >= fNDigitizerLevels-1 && (*ts)(nBins-2) >= fNDigitizerLevels-1 && (*ts)(nBins-3)<fNDigitizerLevels-1) //Find if last bin is sequential max pair
 	    {
	        ++nMultClipTop;
	    }
	    if ((*ts)(nBins-1) <= 0 && (*ts)(nBins-2) <= 0 && (*ts)(nBins-3)>0) //Find if last bin is sequential min pair
	    {
	        ++nMultClipBottom;
	    }
	    if ((*ts)(nBins-2) >= fNDigitizerLevels-1 && (*ts)(nBins-3) >= fNDigitizerLevels-1 && (*ts)(nBins-4)<fNDigitizerLevels-1 && (*ts)(nBins-1)<fNDigitizerLevels-1) //Find if second to last bin is sequential max pair
 	    {
	        ++nMultClipTop;
	    }
	    if ((*ts)(nBins-2) <= 0 && (*ts)(nBins-3) <= 0 && (*ts)(nBins-2)>0 && (*ts)(nBins-1)>0) //Find if second to last bin is sequential min pair
	    {
	        ++nMultClipBottom;
	    }
		///okay i have found pairs now///
         */
        testData.SetClippingData(nClipTop, nClipBottom, nMultClipTop, nMultClipBottom, (double)nClipTop / (double)ts->size(), (double)nClipBottom / (double)ts->size(), (double)nMultClipTop/(double)nClipTop, (double)nMultClipBottom/(double)nClipBottom, component);
        return true;
    }

    bool KTDigitizerTests::LinearityTest(const KTRawTimeSeries* ts, KTDigitizerTestData& testData, unsigned component)
    {
        KTDEBUG(dtlog, "Running Linearity test");
        testData.SetLinearityFlag(true);
        size_t nBins = ts->size();
        int fitStart = -1;
        int fitEnd = -1;
        //find fitStart
        for (size_t iBin = 1; iBin < nBins; ++iBin)
        {
            if ((*ts)(iBin) != 0)
            {
                fitStart = iBin - 1;
                break;
            }
        }
        //error finding fitStart
        if (fitStart == -1)
        {
            KTERROR(dtlog, "Unable to find fit start");
            return false;
        }

        //find fitEnd
        double* whereHigh = new double[fBinsPerAverage];
        double oldAverage = 0;
        /*
        for (size_t iBin = fitStart+1; iBin < nBins; ++iBin) //testing
        {
            if ((*ts)(iBin) == 185)
            {
                break;
            }
        }
        */
        for (size_t iBin = fitStart+1; iBin < nBins; ++iBin)
        {
            whereHigh[iBin % fBinsPerAverage] = (*ts)(iBin);
            double total = 0;
            for (int i=0; i < fBinsPerAverage; ++i)
            {
                total = total + whereHigh[i];
            }
            double average = total / fBinsPerAverage;

            if (average < oldAverage)
            {
                fitEnd = iBin - fBinsPerAverage;
                break;
            }
            oldAverage = average;
        }
        //error finding fitEnd
        if (fitEnd == -1)
        {
            fitEnd = nBins-1;
        }

        delete [] whereHigh;

        double slope = double((*ts)(fitEnd) - (*ts)(fitStart)) / double(fitEnd - fitStart);

        //Linear Regression
        double sumXY = 0;
        double sumX = 0;
        double sumY = 0;
        double sumX2 = 0;
        for (size_t iBin = fitStart; iBin < fitEnd; ++iBin)
        {
            sumXY += (double)iBin * (double)((*ts)(iBin));
            sumX += iBin;
            sumY += (double)((*ts)(iBin));
            sumX2 += iBin * iBin;
        }

        double linRegSlope = ((fitEnd - fitStart+1) * sumXY - sumX * sumY) / ((fitEnd - fitStart+1) * sumX2 - sumX * sumX);
        double linRegIntercept = (sumY - linRegSlope * sumX) / (fitEnd - fitStart + 1);

        // Max difference from linreg line and chisquared
        double regBigDist = 0;
        double totalSqDist = 0;
        for (size_t iBin = fitStart; iBin < fitEnd; ++iBin)
        {
            double regYDist = (*ts)(iBin) - ((iBin-fitStart) * linRegSlope + linRegIntercept);
            totalSqDist = totalSqDist + regYDist*regYDist;
            if (regYDist > regBigDist)
            {
                regBigDist = regYDist;
            }
        }
        double avgSqDist = totalSqDist / (fitEnd - fitStart + 1);

        testData.SetLinearityData(regBigDist/256, avgSqDist, fitStart, fitEnd, linRegSlope, linRegIntercept, component);
        return true;
    }

    void KTDigitizerTests::SetTestBitOccupancy(bool flag)
    {
        fTestBitOccupancy = flag;
        if (flag)
        {
            fRawTestFuncs.insert(TestFuncs::value_type(fBitOccupancyTestID, &KTDigitizerTests::BitOccupancyTest));
        }
        else
        {
            fRawTestFuncs.erase(fBitOccupancyTestID);
        }
        return;
    }

    void KTDigitizerTests::SetTestClipping(bool flag)
    {
        fTestClipping = flag;
        if (flag)
        {
            fRawTestFuncs.insert(TestFuncs::value_type(fClippingTestID, &KTDigitizerTests::ClippingTest));
        }
        else
        {
            fRawTestFuncs.erase(fClippingTestID);
        }
        return;
    }

    void KTDigitizerTests::SetTestLinearity(bool flag)
    {
        fTestLinearity = flag;
        if (flag)
        {
            fRawTestFuncs.insert(TestFuncs::value_type(fLinearityTestID, &KTDigitizerTests::LinearityTest));
        }
        else
        {
            fRawTestFuncs.erase(fLinearityTestID);
        }
        return;
    }

} /* namespace Katydid */
