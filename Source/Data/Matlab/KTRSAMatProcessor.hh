/**
 @file KTRSAMatProcessor.hh
 @brief: Reads matlab files created by the PNNL RSA
 @details: Lots
 @author: ldeviveiros
 @date: 2014-05-09
 */

#ifndef KTRSAMATPROCESSOR_HH_
#define KTRSAMATPROCESSOR_HH_

#include "KTPrimaryProcessor.hh"

#include "KTData.hh"
#include "KTEggReader.hh"
#include "KTSlot.hh"
#include "KTProcSummary.hh"



namespace Katydid
{
    // Forward declare whatever classes you can
    // input data type . . .
    // output data type . . .
    class KTEggHeader;
    class KTParamNode;

    /*!
     @class KTProcessorTemplate
     @author [name]

     @brief [brief class description]

     @details
     [detailed class description]

     Configuration name: "[config-name]"

     Available configuration values:
     - "some-name": [type] -- [what it does]

     Slots:
     - "header": void (const KTEggHeader* header) -- [what it does]
     - "[slot-name]": void (shared_ptr<KTData>) -- [what it does]; Requires [input data type]; Adds [output data type]; Emits signal "[signal-name]"

     Signals:
     - "header": void (KTEggHeader*) -- emitted when the file header is parsed.
     - "raw-ts" void (KTDataPtr) -- emitted when a new raw time series is produced; guarantees KTRawTimeSeriesData
     - "ts": void (KTDataPtr) -- emitted when the new calibrated time series is produced; Guarantees KTTimeSeriesData
     - "egg-done": void () --  emitted when a file is finished.
     - "summary": void (const KTProcSummary*) -- emitted when a file is finished (after "egg-done")
    */

    class KTRSAMatProcessor : public KTPrimaryProcessor
    {
        public:
            KTRSAMatProcessor(const std::string& name = "mat-processor");
            virtual ~KTRSAMatProcessor();

            bool Configure(const KTParamNode* node);
            bool Run();

            // Getters and setters for configurable parameters go here
        private:
            // configurable member parameters go here

        //public:
            // Functions to do the job of the processor go here
            // These allow the processor to be used on data objects manually


        //private:
            // Perhaps there are some non-public helper functions?

            // And then any non-configurable member variables


        //***************
        // Signals
        //***************
        //private:


        //***************
        // Slots
        //***************
        //private:
            // None

    };

  inline bool KTRSAMatProcessor::Run()
    {
        printf("Running KTRSAMatProcessor\n");
        // Do nothing;
        return true;
    }



}
 /* namespace Katydid */
#endif

