/*
 * TestSignalsAndSlots.cc
 *
 *  Created on: Aug 15, 2012
 *      Author: nsoblath
 */


#include "KTTestProcessor.hh"
#include "KTLogger.hh"

using namespace Katydid;

KTLOGGER(testsiglog, "katydid.verification.testsig")

int main(int argc, char** argv)
{

    KTTestProcessorA tpA;
    KTTestProcessorB tpB;

    /*
    KTSignalWrapper* signalPtr = tpA.GetSignal("the_signal");
    KTSlotWrapper* slot1Ptr = tpB.GetSlot("first_slot");

    slot1Ptr->SetConnection(signalPtr);

    //signalPtr->PrintTestFunc< void (int) >();
    //slot1Ptr->PrintTestFunc< void (int) >();

    //boost::function< void (int) > func = boost::bind(&KTTestProcessorB::Slot1, boost::ref(tpB), _1);

    //slot1Ptr->SetConnection(tpA.fTheSignal.connect(func));
    //KTConnection conn = tpA.fTheSignal.connect(func);

    tpA.EmitSignals(5);

    //conn.disconnect();
    slot1Ptr->Disconnect();

    return 0;
    */
    /**/
    KTINFO(testsiglog, "Connecting the_signal to first_slot and second_slot");
    try
    {
        tpA.ConnectToSignal< KTTestProcessorB, KTTestProcessorA::TheSignal >("the_signal", &tpB, "first_slot");
        tpA.ConnectToSignal< KTTestProcessorB, KTTestProcessorA::TheSignal >("the_signal", &tpB, "second_slot");
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

