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
#include "KTPStoreNode.hh"
#include "KTRawTimeSeries.hh"
#include "KTRawTimeSeriesData.hh"
#include "KTTimeSeriesFFTW.hh"
#include "KTTimeSeriesReal.hh"
#include "KTTimeSeriesData.hh"

#include <cmath>

using boost::shared_ptr;

namespace Katydid
{
    KTLOGGER(dtlog, "katydid.egg");

    static KTNORegistrar< KTProcessor, KTDigitizerTests > sDigTestRegistrar("digitizer-tests");

    KTDigitizerTests::KTDigitizerTests(const std::string& name) :
            KTProcessor(name),
            fNDigitizerBits(8),
            fNDigitizerLevels(pow(2, fNDigitizerBits)),
            fTestBitOccupancy(true),
            fTestClipping(true),
	    fTestLinearity(true),
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

    bool KTDigitizerTests::Configure(const KTPStoreNode* node)
    {
        if (node == NULL) return false;

        fNDigitizerBits = node->GetData< unsigned >("n-digitizer-bits", fNDigitizerBits);

        SetTestBitOccupancy(node->GetData< bool >("test-bit-occupancy", fTestBitOccupancy));

        SetTestClipping(node->GetData< bool >("test-clipping", fTestClipping));

	SetTestLinearity(node->GetData< bool >("test-linearity", fTestLinearity));

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
	for (size_t iBin = 0; iBin < nBins-1; ++iBin) //Find all sequential max/min except for the last bin
        {
	  if ((*ts)(iBin) >= fNDigitizerLevels-1 && (*ts)(iBin+1) >= fNDigitizerLevels-1)
	    {
	      ++nMultClipTop;
	    }
          if ((*ts)(iBin) <= 0 && (*ts)(iBin+1) <= 0)
	    {
	      ++nMultClipBottom;
	    }
        }
	if ((*ts)(nBins-1) >= fNDigitizerLevels-1 && (*ts)(nBins-2) >= fNDigitizerLevels-1) //Find if last bin is sequential max
	  {
	    ++nMultClipTop;
	  }
	if ((*ts)(nBins-1) <= 0 && (*ts)(nBins-2) <= 0) //Find if last bin is sequential min
	  {
	    ++nMultClipBottom;
	  }
        testData.SetClippingData(nClipTop, nClipBottom, nMultClipTop, nMultClipBottom, (double)nClipTop / (double)ts->size(), (double)nClipBottom / (double)ts->size(), (double)nMultClipTop/(double)nClipTop, (double)nMultClipBottom/(double)nClipBottom, component);
        return true;
    }

   bool KTDigitizerTests::LinearityTest(const KTRawTimeSeries* ts, KTDigitizerTestData& testData, unsigned component)
    {
        KTDEBUG(dtlog, "Running Linearity test");
        testData.SetLinearityFlag(true);
        size_t nBins = ts->size();
	int fitstart = -1;
	int fitend = -1;
	//find fitstart
        for (size_t iBin = 1; iBin < nBins; ++iBin)
        {
	  if ((*ts)(iBin)!=0)
	    {
	      fitstart=iBin-1;
	      break;
	    }
        }
	//error finding fitstart
	if (fitstart==-1)
	  {
	    KTERROR(dtlog, "Unable to find fitstart");
	    return false;
	  }
	//find fitend
	int bpa = 50; //bins per average
	double wherehigh[50]={};
	double oldaverage=0;
	     for (size_t iBin = fitstart+1; iBin < nBins; ++iBin) //testing 
		  {
		    if ((*ts)(iBin)==185)
		      {
			  break;
		      }
		  }
	for (size_t iBin = fitstart+1; iBin < nBins; ++iBin)
	  {
	    wherehigh[iBin % bpa]=(*ts)(iBin);
	    double total = 0;
	    for (int i=0; i<bpa; i++)
	      {
		total = total + wherehigh[i];
	      }
	    double average = total/bpa;
	    
	    if (average < (oldaverage))
	      {
		fitend = iBin-bpa;
		break;
	      }
	    oldaverage=average;
	  }
	//error finding fitend
	if (fitend==-1)
	  {
	    fitend=nBins-1;
	  }
  	double slope = double(((*ts)(fitend))-((*ts)(fitstart)))/double(fitend-fitstart);
	//Linear Regression
	 double sumXY = 0;
	 double sumX=0;
         double sumY=0;
         double sumX2=0;
	 for (size_t iBin = fitstart; iBin < fitend; ++iBin)
	    {
	      sumXY = sumXY + ((double)iBin * (double)((*ts)(iBin)));
	      sumX = sumX + iBin;
	      sumY = sumY + (double)((*ts)(iBin));
	      sumX2 = sumX2 + iBin*iBin;
	     }
	  // Max difference from linreg line and chisquared
	  double linregslope = ((fitend-fitstart+1)*sumXY-sumX*sumY)/((fitend-fitstart+1)*(sumX2)-sumX*sumX);
	  double linregintercept = (sumY - linregslope*sumX)/(fitend-fitstart+1);
	  double regbigdist = 0;
	  double totalsqdist = 0;
	  for (size_t iBin = fitstart; iBin < fitend; ++iBin)
	    {
	      double regydist = ((*ts)(iBin))-((iBin-fitstart)*(linregslope)+linregintercept); 
	      totalsqdist = totalsqdist + regydist*regydist;	     
	      if (regydist > regbigdist)
		{
		  regbigdist = regydist;
		    }
		}
	  double avgsqdist = totalsqdist / (fitend-fitstart+1);
	  testData.SetLinearityData(regbigdist/256, avgsqdist, fitstart, fitend, linregslope, linregintercept, component);
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
