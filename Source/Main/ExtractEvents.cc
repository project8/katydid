/*
 * ExtractEvents.cxx
 *
 *  Created on: Sep 9, 2011
 *      Author: nsoblath
 *      Summary: Copy of Lisa McBride's monarch/cicada-based fft program, minus the performance of the fft
 */

#include "KTEgg.hh"

#include <cstdlib>
#include <iostream>
//#include <sstream>
//using std::stringstream;
#include <unistd.h>

using namespace::Katydid;

int main(int argc, char *argv[])
{

    Int_t arg;
    string fileName = string("No_File_Name_Given");
    string outputFilePrefix = string("file");
    UInt_t numEvents = 1;

    extern char *optarg;

    while ((arg = getopt(argc, argv, "f:p:n:")) != -1)
        switch (arg)
        {
            case 'f':
                fileName = string(optarg);
                break;
            case 'p':
                outputFilePrefix = string(optarg);
                break;
            case 'n':
                numEvents = (UInt_t)atoi(optarg);
                break;
        }

    KTEgg* egg = new KTEgg();
    //struct FFTinput *input;
    //input = malloc(sizeof(struct FFTinput));

    egg->SetFileName(fileName);
    if (! egg->BreakEgg())
    {
        std::cout << "Egg did not break correctly" << std::endl;
        return 1;
    }
    if (! egg->ParseEggHeader())
    {
        std::cout << "Egg header not parsed correctly" << std::endl;
        return 1;
    }

    //setUp(current, input);
    //createPlan(input);

    for (UInt_t iEvent = 0; iEvent < numEvents; iEvent++)
    {
        if (! egg->HatchNextEvent()) continue;

        //stringstream conv;
        //string outputFileName;
        //conv << iEvent;
        //conv >> outputFileName;
        //outputFileName = outputFilePrefix + string("_event") + outputFileName;

        //inputData(current, input);
        //executePlan(input);
        //createOutputFile(outputFileName, input);



    }

    //cleanUp(input);

    return 0;
}
