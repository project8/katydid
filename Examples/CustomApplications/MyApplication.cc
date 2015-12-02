/*
 * MyApplication.cc
 *
 *  Created on: Sep 14, 2012
 *      Author: nsoblath
 */

#include "KTTimeSeriesFFTW.hh"

#include <iostream>

int main()
{
    Katydid::KTTimeSeriesFFTW ts(1, 0., 1.); // Create something Katydid-specific, just to show it works
    std::cout << "Hello World!" << std::endl; // What else did you expect to do in an example file?
    return 0;
}
