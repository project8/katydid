/*
 * FFTWStandin.hh
 *
 *  Created on: Sep 18, 2013
 *      Author: nsoblath
 */

#ifndef FFTWSTANDIN_HH_
#define FFTWSTANDIN_HH_

#ifndef FFTW_FOUND

#include <cstddef>

// typdef the complex (real, imag) type
typedef double fftw_complex[2];

// array memory allocation
fftw_complex* fftw_malloc(size_t size);

// array memory deallocation
void fftw_free(fftw_complex* array);

#endif

#endif /* FFTWSTANDIN_HH_ */
