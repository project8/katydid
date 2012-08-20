/*
 * TestEggHatching.cc
 *
 *  Created on: Aug 20, 2012
 *      Author: nsoblath
 *
 *  Usage:  bin/TestEggHatching filename.egg
 */


#include "KTEgg.hh"
#include "KTEggHeader.hh"
#include "KTEvent.hh"
#include "KTLogger.hh"

#include <iostream>
#include <string>


using namespace std;
using namespace Katydid;


KTLOGGER(testegg, "katydid.validation.egghatch");


int main(int argc, char** argv)
{

    if (argc != 2) return 0;
    string filename(argv[1]);

    KTINFO(testegg, "Test of hatching egg file <" << filename << ">");

    cout << "Opening file" << endl;
    KTEgg* egg = new KTEgg();
    if (egg->BreakEgg(filename))
    {
        KTINFO(testegg, "Egg opened successfully");
    }
    else
    {
        KTERROR(testegg, "Egg file was not opened");
        return -1;
    }

    KTEggHeader* header = egg->GetHeader();
    if (header == NULL)
    {
        KTERROR(testegg, "No header received");
        return -1;
    }
    KTINFO(testegg, "Some header information:\n"
           << "\tFilename: " << header->GetFilename() << '\n'
           << "\tAcquisition Mode: " << header->GetAcquisitionMode() << '\n'
           << "\tRecord Size: " << header->GetRecordSize());

    KTINFO(testegg, "Hatching event");
    KTEvent* event = egg->HatchNextEvent();
    if (event == NULL)
    {
        KTERROR(testegg, "Event did not hatch");
        return -1;
    }
    KTINFO(testegg, "This event contains " << event->GetNRecords() << " records");

    KTINFO(testegg, "Test complete; cleaning up");
    egg->CloseEgg();
    delete event;
    delete header;
    delete egg;

    return 0;
}

