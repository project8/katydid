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
    // Forward declare whatever classes you can
    // input data type . . .
    // output data type . . .
    class KTPStoreNode;

    /*!
     @class KTDigitizerTests
     @author N. Oblath

     @brief Runs a suite of tests to assess digitizer health

     @details

     Configuration name: "digitizer-tests"

     Available configuration values:
     - "n-digitizer-bits": UInt_t -- Sets the full number of bits for the ADC in question

     Slots:
     - "header": void (const KTEggHeader* header) -- [what it does]
     - "[slot-name]": void (shared_ptr<KTData>) -- [what it does]; Requires [input data type]; Adds [output data type]; Emits signal "[signal-name]"

     Signals:
     - "[signal-name]": void (shared_ptr<KTData>) -- Emitted upon [whatever was done]; Guarantees [output data type].
    */

    class KTDigitizerTests : public KTProcessor
    {
        public:
            KTDigitizerTests(const std::string& name = "digitizer-tests");
            virtual ~KTDigitizerTests();

            Bool_t Configure(const KTPStoreNode* node);

            UInt_t GetNDigitizerBits() const;
            void SetNDigitizerBits(UInt_t nBits);

        private:
            UInt_t fNDigitizerBits;

        public:
            // Functions to do the job of the processor go here
            // These allow the processor to be used on data objects manually


        private:
            // Perhaps there are some non-public helper functions?

            // And then any non-configurable member variables

            //***************
            // Signals
            //***************

        private:
            //KTSignalData f[SomeName]Signal;

            //***************
            // Slots
            //***************

        private:
            //KTSlotOneArg< void (const KTEggHeader*) > fHeaderSlot;
            //KTSlotDataOneType< [Input Data Type] > f[SomeName]Slot;

    };

    UInt_t KTDigitizerTests::GetNDigitizerBits() const
    {
        return fNDigitizerBits;
    }
    void KTDigitizerTests::SetNDigitizerBits(UInt_t nBits)
    {
        fNDigitizerBits = nBits;
        return;
    }


 /* namespace Katydid */
#endif /* KTDIGITIZERTESTS_HH_ */
