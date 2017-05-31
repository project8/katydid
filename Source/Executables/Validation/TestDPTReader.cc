/*
 * TestDPTReader.cc
 *
 *  Created on: Oct 14, 2016
 *      Author: N.S. Oblath
 *
 *  Usage:
 *    > bin/TestDPTReader filename.dpt
 *
 */


#include "KTDPTReader.hh"
#include "KTLogger.hh"
#include "KTPowerSpectrumData.hh"

#include <algorithm>
#include <string>
#include <strstream>

using namespace Katydid;


KTLOGGER(testlog, "TestDPTReader");


int main(int argc, char** argv)
{

    if (argc < 2)
    {
        KTERROR(testlog, "Please provide a filename\nUsage: bin/TestDPTReader filename.dpt");
        return 0;
    }
    std::string filename(argv[1]);

    KTINFO(testlog, "Testing DPT reader with file <" << filename << ">");

    KTDPTReader reader;
    reader.Filename() = filename;

    KTINFO(testlog, "Reading the file");
    Nymph::KTData data;
    if (! reader.ReadFile(data))
    {
        KTERROR(testlog, "Something went wrong while reading the file");
        return -1;
    }

    KTINFO(testlog, "Reading is complete");

    KTPowerSpectrumData& psData = data.Of< KTPowerSpectrumData >();
    if (psData.GetNComponents() != 1)
    {
        KTERROR(testlog, "PS data does not have 1 component: " << psData.GetNComponents());
        return -1;
    }

    KTPowerSpectrum* spectrum = psData.GetSpectrum(0);

    std::stringstream binStr;
    unsigned maxBin = std::min(spectrum->size(), (size_t)10);
    for (unsigned iBin = 0; iBin < maxBin; ++iBin)
    {
        binStr << "\t\tBin " << iBin << "; value = " << (*spectrum)(iBin) << "\n";
    }

    KTINFO(testlog, "Power spectrum extracted from the DPT file:\n"
           << "\tNumber of bins: " << spectrum->size() << '\n'
           << "\tMin frequency: " << spectrum->GetRangeMin() << " Hz\n"
           << "\tMax frequency: " << spectrum->GetRangeMax() << " Hz\n"
           << "\tFirst 10 bins:\n"
           << binStr.str());

    return 0;
}


