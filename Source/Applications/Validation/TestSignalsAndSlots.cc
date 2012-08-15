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

    KTINFO(testsiglog, "Connecting the_signal to first_slot and second_slot");
    try
    {
        tpA.ConnectToSignal2< KTTestProcessorB, void(int) >("the_signal", &tpB, "first_slot");
        tpA.ConnectToSignal2< KTTestProcessorB, void(int) >("the_signal", &tpB, "second_slot");
    }
    catch(std::exception& e)
    {
        KTERROR(testsiglog, "A problem occured while connecting the signal and slots:\n" << e.what());
        return -1;
    }

    KTINFO(testsiglog, "Emitting signals");
    KTINFO(testsiglog, "First test signal: 5");
    tpA.EmitSignals(5);
    KTINFO(testsiglog, "Second test signal: 18");
    tpA.EmitSignals(18);

    KTINFO(testsiglog, "Tests complete");
    return 0;
}

