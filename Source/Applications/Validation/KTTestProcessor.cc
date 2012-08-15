/*
 * KTTestProcessor.cc
 *
 *  Created on: Aug 15, 2012
 *      Author: nsoblath
 */

#include "KTTestProcessor.hh"

#include <iostream>
using std::cout;
using std::endl;

namespace Katydid
{

    KTTestProcessorA::KTTestProcessorA() :
            fTheSignal()
    {
        RegisterSignal("the_signal", &fTheSignal);
    }

    KTTestProcessorA::~KTTestProcessorA()
    {
    }

    void KTTestProcessorA::EmitSignals(int value)
    {
        fTheSignal(value);
        return;
    }




    KTTestProcessorB::KTTestProcessorB()
    {
        RegisterSlot2("first_slot", this, &KTTestProcessorB::Slot1);
        RegisterSlot2("second_slot", this, &KTTestProcessorB::Slot2);
    }

    KTTestProcessorB::~KTTestProcessorB()
    {
    }

    void KTTestProcessorB::Slot1(int input)
    {
        cout << "Slot1: input is " << input << endl;
        return;
    }

    void KTTestProcessorB::Slot2(int input)
    {
        cout << "Slot2: twice input is " << 2*input << endl;
        return;
    }



} /* namespace Katydid */
