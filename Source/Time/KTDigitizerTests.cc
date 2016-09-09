
/*
 * KTDigitizerTests.cc
 *
 *  Created on: December 17, 2013
 *      Author: N. Oblath
 */

#include "KTDigitizerTests.hh"

#include "KTDigitizerTestData.hh"
#include "KTRawTimeSeries.hh"
#include "KTRawTimeSeriesData.hh"
#include "KTTimeSeriesFFTW.hh"
#include "KTTimeSeriesReal.hh"
#include "KTTimeSeriesData.hh"

#include <cmath>
#include <numeric>

using std::vector;

namespace Katydid
{
    KTLOGGER(dtlog, "KTDigitizerTests");

    KT_REGISTER_PROCESSOR(KTDigitizerTests, "digitizer-tests");

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

    bool KTDigitizerTests::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return false;

        fNDigitizerBits = node->get_value< unsigned >("n-digitizer-bits", fNDigitizerBits);

        SetTestBitOccupancy(node->get_value< bool >("test-bit-occupancy", fTestBitOccupancy));

        SetTestClipping(node->get_value< bool >("test-clipping", fTestClipping));

        SetTestLinearity(node->get_value< bool >("test-linearity", fTestLinearity));

	if (node->has("disable-component"))
	  {
	    fDisableComponents.push_back(node->get_value< unsigned >("disable-component"));
	    KTWARN(dtlog, "dc now has " << fDisableComponents.size()  << " components");
	    KTWARN(dtlog, fDisableComponents[0]);
	  }

        return true;
    }

    bool KTDigitizerTests::RunTests(KTRawTimeSeriesData& data)
    {
        unsigned nComponents = data.GetNComponents();
        KTWARN(dtlog, "Size of fRawTestFuncs = " << fRawTestFuncs.size());
        KTDigitizerTestData& dtData = data.Of< KTDigitizerTestData >().SetNComponents(nComponents);
        for (unsigned component = 0; component < nComponents; ++component)
        {
	  bool skipComponent = false;
	  for (vector<unsigned>::const_iterator dcIt = fDisableComponents.begin(); dcIt != fDisableComponents.end(); ++dcIt)
	    {
	      KTWARN(dtlog, "checking " << *dcIt);
	      if (*dcIt == component)
		{
		  KTWARN(dtlog, "skipping " << component );
		  skipComponent = true;
		  break;
		}
	    }
	  if (skipComponent) continue;
	  KTWARN(dtlog, "analyzing " << component);

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
		/// If you want to find pairs of sequential clips, ///
		/// and comment out the above section.             ///
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
         */
        testData.SetClippingData(nClipTop, nClipBottom, nMultClipTop, nMultClipBottom, (double)nClipTop / (double)ts->size(), (double)nClipBottom / (double)ts->size(), (double)nMultClipTop/(double)nClipTop, (double)nMultClipBottom/(double)nClipBottom, component);
        return true;
    }

    bool KTDigitizerTests::LinearityTest(const KTRawTimeSeries* ts, KTDigitizerTestData& testData, unsigned component)
    {
        KTDEBUG(dtlog, "Running Linearity test");
        testData.SetLinearityFlag(true);
        size_t nBins = ts->size();
        int localMax = -1;
        int localMin = 300;
	int lastMax = -1;
	int lastMin = -1;
	int lastMaxEnd = -1;
	int lastMinEnd = -1;
	std::vector<int> localMaxStarts;
	std::vector<int> localMinStarts;
	std::vector<int> localMaxEnds;
	std::vector<int> localMinEnds;

	//find localMax and localMin
	for (size_t iBin = 1; iBin < nBins; ++iBin)
	  {
	    if ((int)((*ts)(iBin)) > localMax)
	      {
		localMax = (*ts)(iBin);
	      }
	    if ((int)((*ts)(iBin)) < localMin)
	      {
		localMin = (*ts)(iBin);
	      }
	  }
	//find where the maxes and mins are
	int countermax = 0;
	int countermin = 0;
	for (size_t iBin = 1; iBin < nBins; ++iBin) //find max and min starts
	  {
	    if ((*ts)(iBin) == localMax)	  
	      {
		//KTDEBUG(dtlog, iBin)
		if (lastMax < iBin-20)
		  {
		    localMaxStarts.push_back(iBin);
		    localMaxEnds.push_back(lastMax);
		  }
		lastMax = iBin;
	      }
	    if ((*ts)(iBin) == localMin)	 
	      {
		if (lastMin < iBin-20)
		  {
		    localMinStarts.push_back(iBin);
		    localMinEnds.push_back(lastMin);
		  }
		lastMin = iBin;
	      }  
	  }
	KTDEBUG(dtlog, "Max:"<<localMax<<", Min:"<<localMin);
	///////////////////////////
	/////////UPSLOPES//////////
	///////////////////////////
	// Upslope end/start shift
	int shiftStart = 0;
	int shiftEnd = 0;
	if (localMinEnds[0]<localMaxStarts[0] & localMinEnds[1]>localMaxStarts[0]) 
	  {
	    shiftStart = 0;
	    shiftEnd = 0;
	  }
	else if (localMinEnds[0]>localMaxStarts[0])
	  {
	    ++shiftEnd;
	    for(size_t i=0; localMinEnds[i]<localMaxStarts[0]; ++i)
	      {
		++shiftEnd;
	      }
	   }
	else if (localMinEnds[1]<localMaxStarts[0])
	   {
	     ++shiftStart;
	     for(size_t i=0; localMinEnds[1]>localMaxStarts[i]; ++i)
	       {
	         ++shiftStart;
	       }
	   }
	else
	   {
	     KTERROR(dtlog, "There is a shifting error.")
	   }
	  if (localMinEnds.size() != localMaxStarts.size())
	    {
	      KTERROR(dtlog, "Number of max and mins not equal.");
	      testData.SetLinearityData(0,0,0,0,0,0, component);
	      return true;
	    }
        //Linear Regression
	double fitStart = localMinEnds[0]; 
	double fitEnd = localMaxStarts[0];
	double sumXY = 0;
	double sumX = 0;
	double sumY = 0;
	double sumX2 = 0;
	double avgLinRegSlope = 0;
	double avgLinRegIntercept = 0;
 	std::vector<double> linRegIntercepts;
	for (int k = 0; k<localMinEnds.size()-shiftStart-shiftEnd; ++k)
	{
          fitStart = localMinEnds[k+shiftStart];
          fitEnd = localMaxStarts[k+shiftEnd];
          sumXY = 0;
          sumX = 0;
	  sumY = 0;
	  sumX2 = 0;
	  for (size_t iBin = fitStart; iBin <= fitEnd; ++iBin)
	    {
	      sumXY += (double)iBin * (double)((*ts)(iBin));
	      sumX += (double)iBin;
	      sumY += (double)((*ts)(iBin));
	      sumX2 += (double)iBin * iBin;
	    }
	  double N = fitEnd-fitStart+1;
	  double linRegSlope =  ((N * sumXY) - (sumX * sumY)) / ((N * sumX2) - (sumX * sumX));
	  double linRegIntercept = (sumY - (linRegSlope * sumX)) / (N);
	  linRegIntercepts.push_back(linRegIntercept);
          avgLinRegSlope = avgLinRegSlope + linRegSlope;
	  avgLinRegIntercept = avgLinRegIntercept + linRegIntercept;
	}
	  avgLinRegSlope = avgLinRegSlope/localMinEnds.size();
	  avgLinRegIntercept = avgLinRegIntercept/localMinEnds.size();	 
  	  std::vector<double> maxDiff;
	  //compare distance
	  for (int k = 0; k<localMinEnds.size()-shiftStart-shiftEnd; ++k)
	  {
	    fitStart = localMinEnds[k+shiftStart];
	    fitEnd = localMaxStarts[k+shiftEnd];
 
 	    double regBigDist = 0;
	    for (size_t iBin = fitStart; (double)iBin <= fitEnd; ++iBin)
	      {
		double regYDist = fabs((*ts)(iBin) -  avgLinRegSlope*(iBin-fitStart));
		if (regYDist > regBigDist)
		  {
		    regBigDist = regYDist;
		  }
	      }
	     maxDiff.push_back(regBigDist/255);
	  }

	  double maxDiffSum = std::accumulate(maxDiff.begin(), maxDiff.end(), 0.0);
	  double maxDiffAvg = maxDiffSum / maxDiff.size();

	  double maxDiffSquareSum = std::inner_product(maxDiff.begin(), maxDiff.end(), maxDiff.begin(), 0.0);
	  double maxDiffStdev = std::sqrt(maxDiffSquareSum / maxDiff.size() - maxDiffAvg * maxDiffAvg);
	  ///////////////////////////
	  ////////DOWNSLOPES/////////
	  ///////////////////////////
	  // Downslope start/end shift
	  int shiftStartD = 0;
	  int shiftEndD = 0;
	  if (localMaxEnds[0]<localMinStarts[0] & localMaxEnds[1]>localMinStarts[0]) 
	    {
	      shiftStartD = 0;
	      shiftEndD = 0;
	    }
	  else if (localMaxEnds[0]>localMinStarts[0])
	    {
	      ++shiftEndD;
	      for(size_t i=0; localMaxEnds[i]<localMinStarts[0]; ++i)
	        {
	          ++shiftEndD;
	        }
	    }
	  else if (localMaxEnds[1]<localMinStarts[0])
	    {
	      ++shiftStartD;
	      for(size_t i=0; localMaxEnds[1]>localMinStarts[i]; ++i)
	        {
	          ++shiftStartD;
	        }
            }
          else
            {
              KTERROR(dtlog, "There is a shifting error.")
            }
          //Linear Regression
	  double fitStartD = localMaxEnds[0]; 
	  double fitEndD = localMinStarts[0];
	  double sumXYD = 0;
	  double sumXD = 0;
	  double sumYD = 0;
	  double sumX2D = 0;
	  double avgLinRegSlopeD = 0;
	  double avgLinRegInterceptD = 0;
 	  std::vector<double> linRegInterceptsD;
	  KTDEBUG(dtlog, "localMaxEnds.size()= "<<localMaxEnds.size()<<", localMinStarts.size()="<<localMinStarts.size());
	  if (localMaxEnds.size() != localMinStarts.size())
	    {
	      KTERROR(dtlog, "Number of max and mins not equal.");
	      testData.SetLinearityData(0,0,0,0,0,0, component);
	      return true;
	    }
	  for (int k = 0; k<localMaxEnds.size()-shiftStartD-shiftEndD; ++k)
	  {
	    fitStartD = localMaxEnds[k+shiftStartD];
	    fitEndD = localMinStarts[k+shiftEndD];
	    sumXYD = 0;
	    sumXD = 0;
	    sumYD = 0;
	    sumX2D = 0;
	    for (size_t iBin = fitStartD; (double)iBin <= fitEndD; ++iBin)
	      {
		sumXYD += (double)iBin * (double)((*ts)(iBin));
		sumXD += (double)iBin;
		sumYD += (double)((*ts)(iBin));
		sumX2D += (double)iBin * iBin;
	      }
	    double N = fitEndD-fitStartD+1;
	    double linRegSlope =  ((N * sumXYD) - (sumXD * sumYD)) / ((N * sumX2D) - (sumXD * sumXD));
	    double linRegIntercept = (sumYD - (linRegSlope * sumXD)) / (N);
	    linRegInterceptsD.push_back(linRegIntercept);
            avgLinRegSlopeD = avgLinRegSlopeD + linRegSlope;
	    avgLinRegInterceptD = avgLinRegInterceptD + linRegIntercept;
	  }
	  avgLinRegSlopeD = avgLinRegSlopeD/localMaxEnds.size();
	  avgLinRegInterceptD = avgLinRegInterceptD/localMaxEnds.size();	 
  	   std::vector<double> maxDiffD;
	   //Compare distance
	   for (int k = 0; k<localMaxEnds.size()-shiftEndD-shiftStartD; ++k)
	  {
	    fitStartD = localMaxEnds[k+shiftStartD];
	    fitEndD = localMinStarts[k+shiftEndD];
 	    double regBigDist = 0;
	    for (size_t iBin = fitStartD; iBin <= fitEndD; ++iBin)
	      {
		double regYDist = fabs((*ts)(iBin) - ( avgLinRegSlopeD*(iBin-fitStartD)+localMax));
		if (regYDist > regBigDist)
		  {
		    regBigDist = regYDist;
		  }
	      }
	     maxDiffD.push_back(regBigDist/255);
	  }

	  double maxDiffSumD = std::accumulate(maxDiffD.begin(), maxDiffD.end(), 0.0);
	  double maxDiffAvgD = maxDiffSumD / maxDiffD.size();

	  double maxDiffSquareSumD = std::inner_product(maxDiffD.begin(), maxDiffD.end(), maxDiffD.begin(), 0.0);
	  double maxDiffStdevD = std::sqrt(maxDiffSquareSumD / maxDiffD.size() - maxDiffAvgD * maxDiffAvgD);
 
	 

        testData.SetLinearityData(maxDiffAvg, maxDiffStdev, avgLinRegSlope, maxDiffAvgD, maxDiffStdevD, avgLinRegSlopeD, component);

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
