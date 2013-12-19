/*
 * ExtractBundles.cxx
 *
 *  Created on: Sep 9, 2011
 *      Author: nsoblath
 *      Summary: Copy of Lisa McBride's monarch/cicada-based fft program, minus the performance of the fft
 */

#include "KTEgg.hh"
#include "KTBundle.hh"

#include "Rtypes.h"

#include <cstdlib>
#include <iostream>
#include <unistd.h>

using namespace::Katydid;
using namespace::std;

int main(int argc, char *argv[])
{

    int arg;
    string fileName = string("No_File_Name_Given");
    string outputFilePrefix = string("file");
    unsigned numBundles = 1;

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
                numBundles = (unsigned)atoi(optarg);
                break;
        }

    KTEgg egg;

    egg.SetFileName(fileName);
    if (! egg.BreakEgg())
    {
        std::cout << "Egg did not break correctly" << std::endl;
        return 1;
    }
    if (! egg.ParseEggHeader())
    {
        std::cout << "Egg header not parsed correctly" << std::endl;
        return 1;
    }

    for (unsigned iBundle = 0; iBundle < numBundles; iBundle++)
    {
        if (! egg.HatchNextBundle()) return 1;
    }

    return 0;
}
