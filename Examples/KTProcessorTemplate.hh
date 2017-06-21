/*
 * KTProcessorTemplate.hh
 *
 *  Created on: Oct 13, 2016
 *      Author: ezayas
 */

#ifndef KTPROCESSORTEMPLATE_HH_
#define KTPROCESSORTEMPLATE_HH_

#include "KTProcessor.hh"
#include "KTData.hh"

#include "KTSlot.hh"

namespace Katydid
{
    
    KTLOGGER(avlog_hh, "KTProcessorTemplate.hh");

    class KTDummyDataObject1;
    class KTDummyDataObject2;
    class KTDummyDataObject3;

    /*
     @class KTProcessorTemplate
     @author E. Zayas

     @brief An example processor to show the structure of these classes

     @details
     This is a "template" processor which has member variables, signals, and slots which act on fictional data objects. Its purpose
     is to show the general structure of a processor class, and to be used as a starting point when creating a new processor.

     Configuration name: "processor-template"

     Available configuration values:
     - "member-variable-1": double -- An example member variable
     - "member-variable-2": double -- An example member variable
     - "member-variable-3": double -- An example member variable

     Slots:
     - "dummy-slot-1": void (Nymph::KTDataPtr) -- Does something with dummy data object #1; Requires KTDummyDataObject1; Adds something perhaps (in this case it will add nothing)
     - "dummy-slot-2": void (Nymph::KTDataPtr) -- Does something with dummy data object #2; Requires KTDummyDataObject2; Adds something perhaps (in this case it will add nothing)
     - "dummy-slot-3": void (Nymph::KTDataPtr) -- Does something with dummy data object #3; Requires KTDummyDataObject3; Adds something perhaps (in this case it will add nothing)

     Signals:
     - "dummy-signal-1": void (Nymph::KTDataPtr) -- Emitted upon successful processing of dummy data object #1; Guarantees KTDummyDataObject1
     - "dummy-signal-2": void (Nymph::KTDataPtr) -- Emitted upon successful processing of dummy data object #2; Guarantees KTDummyDataObject2
     - "dummy-signal-3": void (Nymph::KTDataPtr) -- Emitted upon successful processing of dummy data object #3; Guarantees KTDummyDataObject3
    */

    class KTProcessorTemplate : public Nymph::KTProcessor
    {
        public:
            KTProcessorTemplate(const std::string& name = "processor-template");
            virtual ~KTProcessorTemplate();

            bool Configure(const scarab::param_node* node);

            double GetMemberVariable1() const;
            void SetMemberVariable1(double value);

            double GetMemberVariable2() const;
            void SetMemberVariable2(double value);

            double GetMemberVariable3() const;
            void SetMemberVariable3(double value);

        private:
            double fMemberVariable1;
            double fMemberVariable2;
            double fMemberVariable3;

        public:
            bool AnalyzeDummyObject1( KTDummyDataObject1& someData );
            bool AnalyzeDummyObject2( KTDummyDataObject2& someData );
            bool AnalyzeDummyObject3( KTDummyDataObject3& someData );

            //***************
            // Signals
            //***************

        private:
            Nymph::KTSignalData fDummySignal1;
            Nymph::KTSignalData fDummySignal2;
            Nymph::KTSignalData fDummySignal3;

            //***************
            // Slots
            //***************

        private:
            // If you would like to build this template as-is, you must comment out the following slot initializations because they use made-up data types
            Nymph::KTSlotDataOneType< KTDummyDataObject1 > fSlotOne;
            Nymph::KTSlotDataOneType< KTDummyDataObject2 > fSlotTwo;
            Nymph::KTSlotDataOneType< KTDummyDataObject3 > fSlotThree;
    };

    inline double KTProcessorTemplate::GetMemberVariable1() const
    {
        return fMemberVariable1;
    }

    inline void KTProcessorTemplate::SetMemberVariable1(double value)
    {
        fMemberVariable1 = value;
        return;
    }

    inline double KTProcessorTemplate::GetMemberVariable2() const
    {
        return fMemberVariable2;
    }

    inline void KTProcessorTemplate::SetMemberVariable2(double value)
    {
        fMemberVariable2 = value;
        return;
    }

    inline double KTProcessorTemplate::GetMemberVariable3() const
    {
        return fMemberVariable3;
    }

    inline void KTProcessorTemplate::SetMemberVariable3(double value)
    {
        fMemberVariable3 = value;
        return;
    }

}

#endif /* KTPROCESSORTEMPLATE_HH_ */
