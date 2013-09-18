/*
 * FFTWStandin.cc
 *
 *  Created on: Sep 18, 2013
 *      Author: nsoblath
 */

#ifndef FFTW_FOUND

#include "FFTWStandIn.hh"

fftw_complex* fftw_malloc(size_t size)
{
    return new fftw_complex[size];
}

void fftw_free(fftw_complex* array)
{
    delete [] array;
}

#endif


