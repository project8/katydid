/*
 * TestSignalsAndSlots.cc
 *
 *  Created on: Aug 15, 2012
 *      Author: nsoblath
 */


#include "KTTestProcessor.hh"
#include "KTLogger.hh"

using namespace Katydid;

KTLOGGER(testsiglog, "TestSignalsAndSlots")

int main()
{

    KTTestProcessorA tpA;
    KTTestProcessorB tpB;

    /* Basic Test
    KTSignalWrapper* signalPtr = tpA.GetSignal("the_signal");
    KTSlotWrapper* slot1Ptr = tpB.GetSlot("first_slot");

    slot1Ptr->SetConnection(signalPtr);

    tpA.EmitSignals(5);

    slot1Ptr->Disconnect();

    return 0;
    */

    /* More Complicated Test */
    KTINFO(testsiglog, "Connecting the_signal to first_slot and second_slot");
    try
    {
        tpA.ConnectASlot("the_signal", &tpB, "first_slot", 20);
        tpB.ConnectASignal(&tpA, "the_signal", "second_slot", 10);
    }
    catch(std::exception& e)
    {
        KTERROR(testsiglog, "A problem occurred while connecting the signal and slots:\n" << e.what());
        return -1;
    }

    KTINFO(testsiglog, "Emitting signals");
    KTINFO(testsiglog, "First test signal: 5");
    tpA.EmitSignals(5);
    KTINFO(testsiglog, "Second test signal: 18");
    tpA.EmitSignals(18);

    KTINFO(testsiglog, "Tests complete");
    return 0;
    /**/
}

