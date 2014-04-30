/**
 @file KTProcessorTemplate.hh
 @brief Contains KTProcessorTemplate
 @details [does something]
 @author: [name]
 @date: [date]
 */

#ifndef KTPROCESSORTEMPLATE_HH_
#define KTPROCESSORTEMPLATE_HH_

#include "KTProcessor.hh"

#include "KTSlot.hh"



namespace Katydid
{
    // Forward declare whatever classes you can
    // input data type . . .
    // output data type . . .
    class KTEggHeader;
    class KTPStoreNode;

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
     - "[signal-name]": void (shared_ptr<KTData>) -- Emitted upon [whatever was done]; Guarantees [output data type].
    */

    class KTProcessorTemplate : public KTProcessor
    {
        public:
            KTProcessorTemplate(const std::string& name = "[config-name]");
            virtual ~KTProcessorTemplate();

            bool Configure(const KTPStoreNode* node);

            // Getters and setters for configurable parameters go here
        private:
            // configurable member parameters go here

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
            KTSignalData f[SomeName]Signal;

            //***************
            // Slots
            //***************

        private:
            KTSlotOneArg< void (const KTEggHeader*) > fHeaderSlot;
            KTSlotDataOneType< [Input Data Type] > f[SomeName]Slot;

    };
}
 /* namespace Katydid */
#endif /* KTCOMPLEXFFTW_HH_ */
