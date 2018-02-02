/*
 * KTProcessorTemplate.cc
 *
 *  Created on: Oct 13, 2016
 *      Author: ezayas
 */

#include "KTProcessorTemplate.hh"

#include "KTLogger.hh"

namespace Katydid
{
    KTLOGGER(evlog, "KTProcessorTemplate");

    // Register the processor
    KT_REGISTER_PROCESSOR(KTProcessorTemplate, "processor-template");

    KTProcessorTemplate::KTProcessorTemplate(const std::string& name) :
            KTProcessor(name),
            fMemberVariable1(0.),
            fMemberVariable2(0.),
            fMemberVariable3(0.),
            fDummySignal1("dummy-signal-1", this),
            fDummySignal2("dummy-signal-2", this),
            fDummySignal3("dummy-signal-3", this),

            // If you would like to build this template as-is, you must comment out the following slot initializations because they use made-up data types
            fSlotOne("dummy-slot-1", this, &KTProcessorTemplate::AnalyzeDummyObject1, &fDummySignal1),
            fSlotTwo("dummy-slot-2", this, &KTProcessorTemplate::AnalyzeDummyObject2, &fDummySignal2),
            fSlotThree("dummy-slot-3", this, &KTProcessedTemplate::AnalyzeDummyObject3, &fDummySignal3)
    {
    }

    KTProcessorTemplate::~KTProcessorTemplate()
    {
    }

    bool KTProcessorTemplate::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return false;

        SetMemberVariable1(node->get_value< double >("member-variable-1", fMemberVariable1));
        SetMemberVariable2(node->get_value< double >("member-variable-2", fMemberVariable2));
        SetMemberVariable3(node->get_value< double >("member-variable-3", fMemberVariable3));
        
        return true;
    }

    bool KTProcessorTemplate::AnalyzeDummyObject1( KTDummyDataObject1& someData )
    {
        // The meat of the processor slot goes here

        return true;
    }

    bool KTProcessorTemplate::AnalyzeDummyObject2( KTDummyDataObject2& someData )
    {
        // The meat of the processor slot goes here

        return true;
    }

    bool KTProcessorTemplate::AnalyzeDummyObject3( KTDummyDataObject3& someData )
    {
        // The meat of the processor slot goes here

        return true;
    }

} // namespace Katydid
