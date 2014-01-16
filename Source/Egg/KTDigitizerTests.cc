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
        for (size_t iBin = 0; iBin < nBins; ++iBin)
        {
            if ((*ts)(iBin) >= fNDigitizerLevels) ++nClipTop;
            if ((*ts)(iBin) <= 0) ++nClipBottom;
        }
        testData.SetClippingData(nClipTop, nClipBottom, (double)nClipTop / (double)ts->size(), (double)nClipBottom / (double)ts->size(), component);
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
     	KTDEBUG(dtlog, fitstart);
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
	for (size_t iBin = fitstart+1; iBin < nBins; ++iBin)
	  {
	    if ((*ts)(iBin)<=(*ts)(iBin-1))
	      {
		fitend=iBin-1;
		break;
	      }
	  }
	//error finding fitend
	if (fitend==-1)
	  {
	    fitend=nBins-1;
	  }
	KTDEBUG(dtlog, "fitstart is " << fitstart);
	KTDEBUG(dtlog, "fitend is " << fitend);
	//linreg
	KTDEBUG(dtlog, "Slope of the fitted line  is " << (((*ts)(fitend)-(*ts)(fitstart))/(fitend-fitstart)));
	double totalsqdist = 0;
	for (size_t iBin = fitstart; iBin < fitend; ++iBin)
	    {
	      double ydist = ((*ts)(iBin))-((iBin-fitstart)*(((*ts)(fitend)-(*ts)(fitstart))/(fitend-fitstart))); //since hight of fitted line is slope * number of bins
		totalsqdist = totalsqdist + ydist*ydist;
		}
	  double avgsqdist = totalsqdist / (fitend-fitstart+1);
	  KTDEBUG(dtlog, "Average of the y distances squared is " << totalsqdist);

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
