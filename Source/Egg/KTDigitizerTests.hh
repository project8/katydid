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

#include "KTSlot.hh"


namespace Katydid
{
    class KTPStoreNode;
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
     - "n-digitizer-bits": UInt_t -- Sets the full number of bits for the ADC in question
     - "test-bit-occupancy": Bool_t -- Determines whether the bit occupancy test is run
     - "test-clipping": Bool_t -- Determines whether the clipping test is run

     Slots:
     - "ts": void (KTDataPtr) -- [what it does]; Requires KTTimeSeriesData; Adds KTDigitizerTestData; Emits signal "dig-test"

     Signals:
     - "dig-test": void (KTDataPtr) -- Emitted upon completion of digitizer tests; Guarantees KTdigitizerTestData.
    */

    class KTDigitizerTests : public KTProcessor
    {
        private:
            typedef Bool_t (KTDigitizerTests::*ptrToRealTestFunc)(const KTTimeSeriesReal*, KTDigitizerTestData&, UInt_t);
            typedef Bool_t (KTDigitizerTests::*ptrToFFTWTestFunc)(const KTTimeSeriesFFTW*, KTDigitizerTestData&, UInt_t);

         public:
            KTDigitizerTests(const std::string& name = "digitizer-tests");
            virtual ~KTDigitizerTests();

            Bool_t Configure(const KTPStoreNode* node);

            UInt_t GetNDigitizerBits() const;
            void SetNDigitizerBits(UInt_t nBits);

            Bool_t GetTestBitOccupancy() const;
            void SetTestBitOccupancy(Bool_t flag);

            Bool_t GetTestClipping() const;
            void SetTestClipping(Bool_t flag);

            void Initialize();

        private:
            UInt_t fNDigitizerBits;

            Bool_t fTestBitOccupancy;

            Bool_t fTestClipping;

            Bool_t fRunInitialize;

        public:
            Bool_t RunTestsOnRealTS(KTTimeSeriesData& data);
            Bool_t RunTestsOnFFTWTS(KTTimeSeriesData& data);

            Bool_t RunBitOccupancyTest(const KTTimeSeriesFFTW* ts, KTDigitizerTestData& testData, UInt_t component);
            Bool_t RunBitOccupancyTest(const KTTimeSeriesFFTW* ts, KTDigitizerTestData& testData, UInt_t component);

            Bool_t RunClippingTest(const KTTimeSeriesFFTW* ts, KTDigitizerTestData& testData, UInt_t component);
            Bool_t RunClippingTest(const KTTimeSeriesFFTW* ts, KTDigitizerTestData& testData, UInt_t component);

        private:
            // Test function pointers
            ptrToFFTWTestFunc* fBitOccupancyFFTWTestPtr;
            ptrToRealTestFunc* fBitOccupancyRealTestPtr;

            ptrToFFTWTestFunc* fClippingFFTWTestPtr;
            ptrToRealTestFunc* fClippingRealTestPtr;

            // Actual test functions
            Bool_t BitOccupancyTest(const KTTimeSeriesFFTW* ts, KTDigitizerTestData& testData, UInt_t component);
            Bool_t BitOccupancyTest(const KTTimeSeriesReal* ts, KTDigitizerTestData& testData, UInt_t component);

            Bool_t ClippingTest(const KTTimeSeriesFFTW* ts, KTDigitizerTestData& testData, UInt_t component);
            Bool_t ClippingTest(const KTTimeSeriesReal* ts, KTDigitizerTestData& testData, UInt_t component);


            // Null test functions
            Bool_t NullTest(const KTTimeSeriesFFTW*, KTDigitizerTestData&, UInt_t);
            Bool_t NullTest(const KTTimeSeriesReal*, KTDigitizerTestData&, UInt_t);


            //***************
            // Signals
            //***************

        private:
            KTSignalData fDigTestSignal;

            //***************
            // Slots
            //***************

        private:
            KTSlotDataOneType< KTTimeSeriesData > fDigTestRealSlot;
            KTSlotDataOneType< KTTimeSeriesData > fDigTestFFTWSlot;

    };

    inline UInt_t KTDigitizerTests::GetNDigitizerBits() const
    {
        return fNDigitizerBits;
    }
    inline void KTDigitizerTests::SetNDigitizerBits(UInt_t nBits)
    {
        fNDigitizerBits = nBits;
        return;
    }

    inline Bool_t KTDigitizerTests::GetTestBitOccupancy() const
    {
        return fTestBitOccupancy;
    }
    inline void KTDigitizerTests::SetTestBitOccupancy(Bool_t flag)
    {
        fTestBitOccupancy = flag;
        fRunInitialize = true;
        return;
    }

    inline Bool_t KTDigitizerTests::GetTestClipping() const
    {
        return fTestClipping;
    }
    inline void KTDigitizerTests::SetTestClipping(Bool_t flag)
    {
        fTestClipping = flag;
        fRunInitialize = true;
        return;
    }

    inline Bool_t KTDigitizerTests::RunBitOccupancyTest(const KTTimeSeriesFFTW* ts, KTDigitizerTestData& testData, UInt_t component)
    {
        if (fRunInitialize) Initialize();
        return (this->*fBitOccupancyRealTestPtr)(ts, testData, component);
    }
    inline Bool_t KTDigitizerTests::RunBitOccupancyTest(const KTTimeSeriesFFTW* ts, KTDigitizerTestData& testData, UInt_t component)
    {
        if (fRunInitialize) Initialize();
        return (this->*fBitOccupancyFFTWTestPtr)(ts, testData, component);
    }

    inline Bool_t KTDigitizerTests::RunClippingTest(const KTTimeSeriesFFTW* ts, KTDigitizerTestData& testData, UInt_t component)
    {
        if (fRunInitialize) Initialize();
        return (this->*fClippingRealTestPtr)(ts, testData, component);
    }
    inline Bool_t KTDigitizerTests::RunClippingTest(const KTTimeSeriesFFTW* ts, KTDigitizerTestData& testData, UInt_t component)
    {
        if (fRunInitialize) Initialize();
        return (this->*fClippingFFTWTestPtr)(ts, testData, component);
    }

    inline Bool_t KTDigitizerTests::NullTest(const KTTimeSeriesFFTW*, KTDigitizerTestData&, UInt_t)
    {
        return true;
    }

    inline Bool_t KTDigitizerTests::NullTest(const KTTimeSeriesReal*, KTDigitizerTestData&, UInt_t)
    {
        return true;
    }

 /* namespace Katydid */
#endif /* KTDIGITIZERTESTS_HH_ */
