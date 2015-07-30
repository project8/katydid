/*
 * KTTestProcessor.cc
 *
 *  Created on: Aug 15, 2012
 *      Author: nsoblath
 */

#include "KTTestProcessor.hh"

#include "KTLogger.hh"

namespace Nymph
{
    KTLOGGER(testsiglog, "KTTestProcessor")

    KTTestProcessorA::KTTestProcessorA() :
            fTheSignal()
    {
        RegisterSignal("the_signal", &fTheSignal);
    }

    KTTestProcessorA::~KTTestProcessorA()
    {
    }

    bool KTTestProcessorA::Configure(const KTParamNode*)
    {
        return true;
    }

    void KTTestProcessorA::EmitSignals(int value)
    {
        fTheSignal(value);
        return;
    }




    KTTestProcessorB::KTTestProcessorB()
    {
        RegisterSlot("first_slot", this, &KTTestProcessorB::Slot1);
        RegisterSlot("second_slot", this, &KTTestProcessorB::Slot2);
    }

    KTTestProcessorB::~KTTestProcessorB()
    {
    }

    bool KTTestProcessorB::Configure(const KTParamNode*)
    {
        return true;
    }

    void KTTestProcessorB::Slot1(int input)
    {
        KTINFO(testsiglog, "Slot1: input is " << input);
        return;
    }

    void KTTestProcessorB::Slot2(int input)
    {
        KTINFO(testsiglog, "Slot2: twice input is " << 2*input);
        return;
    }



} /* namespace Nymph */
