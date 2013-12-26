/**
 @file KTDigitizerTests.hh
 @brief Contains KTDigitizerTests
 @details Runs a suite of tests to measure digitizer health
 @author: N. Oblath
 @date: December 17, 2013
 */

#ifndef KTDIGITIZERTESTS_HH_
#define KTDIGITIZERTESTS_HH_

#include "KTProcessor.hh"

#include "KTDigitizerTestData.hh"
#include "KTSlot.hh"

#include <cmath>
#include <map>
#include <typeinfo>

namespace Katydid
{
    //class KTDigitizerTestData;
    class KTPStoreNode;
    class KTRawTimeSeriesData;
    class KTRawTimeSeries;
    class KTTimeSeriesData;
    class KTTimeSeriesFFTW;
    class KTTimeSeriesReal;

    /*!
     @class KTDigitizerTests
     @author N. Oblath

     @brief Runs a suite of tests to assess digitizer health

     @details
     Developer note: How to add a new test
     - Add a flag for running the test, getter/setter functions, and use of the setter function in Configure
     - Add public, inline functions Run[test type]Test and Run[test type]Test for Real and FFTW time series types
     - Add private function pointers f[test type]RealTestPtr and f[test type]FFTWTestPtr
     - Add private functions [test type]Test and [test type]Test for Real and FFTW time series types
     - Add setting of function pointers to the Initialize function


     Configuration name: "digitizer-tests"

     Available configuration values:
     - "n-digitizer-bits": unsigned -- Sets the full number of bits for the ADC in question
     - "test-bit-occupancy": bool -- Determines whether the bit occupancy test is run
     - "test-clipping": bool -- Determines whether the clipping test is run

     Slots:
     - "ts": void (KTDataPtr) -- [what it does]; Requires KTTimeSeriesData; Adds KTDigitizerTestData; Emits signal "dig-test"

     Signals:
     - "dig-test": void (KTDataPtr) -- Emitted upon completion of digitizer tests; Guarantees KTdigitizerTestData.
    */

    class KTDigitizerTests : public KTProcessor
    {
        private:
            typedef bool (KTDigitizerTests::*ptrToTestFunc)(const KTRawTimeSeries*, KTDigitizerTestData&, unsigned);

            typedef std::map< unsigned, ptrToTestFunc > TestFuncs;

         public:
            KTDigitizerTests(const std::string& name = "digitizer-tests");
            virtual ~KTDigitizerTests();

            bool Configure(const KTPStoreNode* node);

            unsigned GetNDigitizerBits() const;
            unsigned GetNDigitizerLevels() const;
            void SetNDigitizerBits(unsigned nBits);

            bool GetTestBitOccupancy() const;
            void SetTestBitOccupancy(bool flag);

            bool GetTestClipping() const;
            void SetTestClipping(bool flag);

        private:
            unsigned fNDigitizerBits;
            unsigned fNDigitizerLevels;

            bool fTestBitOccupancy;

            bool fTestClipping;

        public:
            bool RunTests(KTRawTimeSeriesData& data);

            // Actual test functions
            bool BitOccupancyTest(const KTRawTimeSeries* ts, KTDigitizerTestData& testData, unsigned component);

            bool ClippingTest(const KTRawTimeSeries* ts, KTDigitizerTestData& testData, unsigned component);

        private:
            TestFuncs fRawTestFuncs;

            unsigned fBitOccupancyTestID;
            unsigned fClippingTestID;

            //***************
            // Signals
            //***************

        private:
            KTSignalData fDigTestSignal;

            //***************
            // Slots
            //***************

        private:
            KTSlotDataOneType< KTRawTimeSeriesData > fDigTestRawSlot;

    };

    inline unsigned KTDigitizerTests::GetNDigitizerBits() const
    {
        return fNDigitizerBits;
    }
    inline unsigned KTDigitizerTests::GetNDigitizerLevels() const
    {
        return fNDigitizerLevels;
    }
    inline void KTDigitizerTests::SetNDigitizerBits(unsigned nBits)
    {
        fNDigitizerBits = nBits;
        fNDigitizerLevels = pow(2, nBits);
        return;
    }

    inline bool KTDigitizerTests::GetTestBitOccupancy() const
    {
        return fTestBitOccupancy;
    }

    inline bool KTDigitizerTests::GetTestClipping() const
    {
        return fTestClipping;
    }

} /* namespace Katydid */
#endif /* KTDIGITIZERTESTS_HH_ */
