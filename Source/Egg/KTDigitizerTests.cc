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
#include "KTTimeSeriesFFTW.hh"
#include "KTTimeSeriesReal.hh"
#include "KTTimeSeriesData.hh"

#include <cmath>

using boost::shared_ptr;

namespace Katydid
{
    KTLOGGER(dtlog, "katydid.egg");

    static KTDerivedNORegistrar< KTProcessor, KTDigitizerTests > sDigTestRegistrar("digitizer-tests");

    KTDigitizerTests::KTDigitizerTests(const std::string& name) :
            KTProcessor(name),
            fNDigitizerBits(8),
            fNDigitizerLevels(pow(2, fNDigitizerBits)),
            fTestBitOccupancy(true),
            fTestClipping(true),
            fFFTWTestFuncs(),
            fRealTestFuncs(),
            fBitOccupancyTestID(0),
            fClippingTestID(0),
            fDigTestSignal("dig-test", this),
            fDigTestRealSlot("ts-real", this, &KTDigitizerTests::RunTestsOnRealTS, &fDigTestSignal),
            fDigTestFFTWSlot("ts-fftw", this, &KTDigitizerTests::RunTestsOnFFTWTS, &fDigTestSignal)
    {
        unsigned id = 0;

        fBitOccupancyTestID = ++id;
        fClippingTestID = ++id;

        SetTestBitOccupancy(fTestBitOccupancy);
        SetTestClipping(fTestClipping);
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

        return true;
    }

    bool KTDigitizerTests::RunTestsOnRealTS(KTTimeSeriesData& data)
    {
        unsigned nComponents = data.GetNComponents();
        KTDigitizerTestData& dtData = data.Of< KTDigitizerTestData >().SetNComponents(nComponents);
        for (unsigned component = 0; component < nComponents; ++component)
        {
            const KTTimeSeriesReal* ts = static_cast< const KTTimeSeriesReal* >(data.GetTimeSeries(component));
            for (RealTestFuncs::const_iterator func_it = fRealTestFuncs.begin(); func_it != fRealTestFuncs.end(); ++func_it)
            {
                (this->*(func_it->second))(ts, dtData, component);
            }
        }
        return true;
    }

    bool KTDigitizerTests::RunTestsOnFFTWTS(KTTimeSeriesData& data)
    {
        unsigned nComponents = data.GetNComponents();
        KTDigitizerTestData& dtData = data.Of< KTDigitizerTestData >().SetNComponents(nComponents);
        for (unsigned component = 0; component < nComponents; ++component)
        {
            const KTTimeSeriesFFTW* ts = static_cast< const KTTimeSeriesFFTW* >(data.GetTimeSeries(component));
            for (FFTWTestFuncs::const_iterator func_it = fFFTWTestFuncs.begin(); func_it != fFFTWTestFuncs.end(); ++func_it)
            {
                (this->*(func_it->second))(ts, dtData, component);
            }
        }
        return true;
    }

    bool KTDigitizerTests::BitOccupancyTest(const KTTimeSeriesFFTW* ts, KTDigitizerTestData& testData, unsigned component)
    {
        return true;
    }
    bool KTDigitizerTests::BitOccupancyTest(const KTTimeSeriesReal* ts, KTDigitizerTestData& testData, unsigned component)
    {
        return true;
    }

    bool KTDigitizerTests::ClippingTest(const KTTimeSeriesFFTW* ts, KTDigitizerTestData& testData, unsigned component)
    {
        size_t nBins = ts->size();
        unsigned nClipTop = 0, nClipBottom = 0;
        for (size_t iBin = 0; iBin < nBins; ++iBin)
        {
            if ((*ts)(iBin)[0] >= fNDigitizerLevels) ++nClipTop;
            if ((*ts)(iBin)[0] <= 0) ++nClipBottom;
        }
        testData.SetClippingData(nClipTop, nClipBottom, (double)nClipTop / (double)ts->size(), (double)nClipBottom / (double)ts->size(), component);
        return true;
    }
    bool KTDigitizerTests::ClippingTest(const KTTimeSeriesReal* ts, KTDigitizerTestData& testData, unsigned component)
    {
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

    void KTDigitizerTests::SetTestBitOccupancy(bool flag)
    {
        fTestBitOccupancy = flag;
        if (flag)
        {
            fFFTWTestFuncs.insert(FFTWTestFuncs::value_type(fBitOccupancyTestID, &KTDigitizerTests::BitOccupancyTest));
            fRealTestFuncs.insert(RealTestFuncs::value_type(fBitOccupancyTestID, &KTDigitizerTests::BitOccupancyTest));
        }
        else
        {
            fFFTWTestFuncs.erase(fBitOccupancyTestID);
            fRealTestFuncs.erase(fBitOccupancyTestID);
        }
        return;
    }

    void KTDigitizerTests::SetTestClipping(bool flag)
    {
        fTestClipping = flag;
        if (flag)
        {
            fFFTWTestFuncs.insert(FFTWTestFuncs::value_type(fClippingTestID, &KTDigitizerTests::ClippingTest));
            fRealTestFuncs.insert(RealTestFuncs::value_type(fClippingTestID, &KTDigitizerTests::ClippingTest));
        }
        else
        {
            fFFTWTestFuncs.erase(fClippingTestID);
            fRealTestFuncs.erase(fClippingTestID);
        }
        return;
    }



} /* namespace Katydid */
