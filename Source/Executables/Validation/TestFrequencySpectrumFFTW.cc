/*
 * TestFrequencySpectrumFFTW.cc
 *
 *  Created on: Nov 8, 2012
 *      Author: nsoblath
 */


#include "KTFrequencySpectrumFFTW.hh"
#include "logger.hh"

#include <sstream>

using namespace Katydid;
using namespace std;

LOGGER(testlog, "TestFrequencySpectrumFFTW");

int main()
{
    LINFO(testlog, "Creating a KTPhysicalVectorFFTW and a KTFrequencySpectrumFFTW of length 10, and filling with integers from 0 to 9.");
    LINFO(testlog, "Access to the arrays will be via operator().");

    unsigned size = 10;
    double min = 0.;
    double max = 10.;

    KTPhysicalArray<1, fftw_complex> physArr(size, min, max);
    KTFrequencySpectrumFFTW freqSpect(size, min, max);

    for (unsigned i=0; i<size; i++)
    {
        physArr(i)[0] = double(i);
        physArr(i)[1] = 0.;
        freqSpect(i)[0] = double(i);
        freqSpect(i)[1] = 0.;
    }

    LINFO(testlog, "Printing contents of the two arrays by accessing the data storage directly.");
    const KTPhysicalArray< 1, fftw_complex >::array_type dataForPhysArr = physArr.GetData();
    const KTFrequencySpectrumFFTW::array_type dataForFreqSpect = freqSpect.GetData();

    stringstream printer;
    printer << '\n';
    printer << "\tphysArr\tfreqSpect\n" << "\t-------\t---------\n";
    for (unsigned i=0; i<size; i++)
    {
        printer << '\t' << dataForPhysArr[i] << '\t' << dataForFreqSpect[i] << '\n';
    }
    LWARN(testlog, printer.str());

    LINFO(testlog, "Test complete.");

    return 0;
}

