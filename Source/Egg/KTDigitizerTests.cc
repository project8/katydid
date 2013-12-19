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

using boost::shared_ptr;

namespace Katydid
{
    KTLOGGER(dtlog, "katydid.egg");

    static KTDerivedNORegistrar< KTProcessor, KTDigitizerTests > sDigTestRegistrar("digitizer-tests");

    KTDigitizerTests::KTDigitizerTests(const std::string& name) :
            KTProcessor(name),
            fNDigitizerBits(8),
            fTestBitOccupancy(true),
            fTestClipping(true),
            fRunInitialize(true),
            fBitOccupancyFFTWTestPtr(&KTDigitizerTests::BitOccupancyTest),
            fBitOccupancyRealTestPtr(&KTDigitizerTests::BitOccupancyTest),
            fClippingFFTWTestPtr(&KTDigitizerTests::ClippingTest),
            fClippingRealTestPtr(&KTDigitizerTests::ClippingTest),
            fDigTestSignal("dig-test", this),
            fDigTestRealSlot("ts-real", this, &KTDigitizerTests::RunTestsOnRealTS, &fDigTestSignal),
            fDigTestFFTWSlot("ts-fftw", this, &KTDigitizerTests::RunTestsOnFFTWTS, &fDigTestSignal)
    {
    }

    KTDigitizerTests::~KTDigitizerTests()
    {
    }

    Bool_t KTDigitizerTests::Configure(const KTPStoreNode* node)
    {
        if (node == NULL) return false;

        fNDigitizerBits = node->GetData< UInt_t >("n-digitizer-bits", fNDigitizerBits);

        SetTestBitOccupancy(node->GetData< Bool_t >("test-bit-occupancy", fTestBitOccupancy));

        SetTestClipping(node->GetData< Bool_t >("test-clipping", fTestClipping));

        return true;
    }

    void KTDigitizerTests::Initialize()
    {
        if (fTestBitOccupancy)
        {
            fBitOccupancyFFTWTestPtr = &KTDigitizerTests::BitOccupancyTest;
            fBitOccupancyRealTestPtr = &KTDigitizerTests::BitOccupancyTest;
        }
        else
        {
            fBitOccupancyFFTWTestPtr = &KTDigitizerTests::NullTest;
            fBitOccupancyRealTestPtr = &KTDigitizerTests::NullTest;
        }

        if (fTestClipping)
        {
            fBitOccupancyFFTWTestPtr = &KTDigitizerTests::ClippingTest;
            fBitOccupancyRealTestPtr = &KTDigitizerTests::ClippingTest;
        }
        else
        {
            fBitOccupancyFFTWTestPtr = &KTDigitizerTests::NullTest;
            fBitOccupancyRealTestPtr = &KTDigitizerTests::NullTest;
        }

        return;
    }

    Bool_t KTDigitizerTests::RunTestsOnRealTS(KTTimeSeriesData& data)
    {
        return true;
    }

    Bool_t KTDigitizerTests::RunTestsOnFFTWTS(KTTimeSeriesData& data)
    {
        return true;
    }

    Bool_t KTDigitizerTests::BitOccupancyTest(const KTTimeSeriesFFTW* ts, KTDigitizerTestData& testData, UInt_t component)
    {
        return true;
    }
    Bool_t KTDigitizerTests::BitOccupancyTest(const KTTimeSeriesReal* ts, KTDigitizerTestData& testData, UInt_t component)
    {
        return true;
    }

    Bool_t KTDigitizerTests::ClippingTest(const KTTimeSeriesFFTW* ts, KTDigitizerTestData& testData, UInt_t component)
    {
        return true;
    }
    Bool_t KTDigitizerTests::ClippingTest(const KTTimeSeriesReal* ts, KTDigitizerTestData& testData, UInt_t component)
    {
        return true;
    }


} /* namespace Katydid */
