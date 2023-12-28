/*
 * KTFFT.cc
 *
 *  Created on: Sep 16, 2011
 *      Author: nsoblath
 */

#include "KTFFT.hh"

#include "logger.hh"

#ifdef FFTW_FOUND
#include <fftw3.h>
#endif

namespace Katydid
{
    LOGGER(fftlog, "KTFFT");

    KTFFT::KTFFT()
    {
    }

    KTFFT::~KTFFT()
    {
    }

    KTFFTW::KTFFTW() :
            KTFFT()
    {
        sInstanceCount++;
    }

    KTFFTW::~KTFFTW()
    {
#ifdef FFTW_NTHREADS
        if (sInstanceCount == 1)
        {
            fftw_cleanup_threads();
        }
#endif
        sInstanceCount--;
    }

    void KTFFTW::InitializeMultithreaded()
    {
#ifdef FFTW_NTHREADS
        if (! sMultithreadedIsInitialized)
        {
            fftw_init_threads();
            fftw_plan_with_nthreads(FFTW_NTHREADS);
            LDEBUG(fftlog, "Configuring FFTW to use up to " << FFTW_NTHREADS << " threads.");
            sMultithreadedIsInitialized = true;
        }
#endif
        return;
    }


    unsigned KTFFTW::sInstanceCount = 0;
    bool KTFFTW::sMultithreadedIsInitialized = false;

} /* namespace Katydid */
