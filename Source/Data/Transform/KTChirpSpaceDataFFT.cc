/*
 * KTChirpSpaceDataFFT.cc
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#include "KTChirpSpaceDataFFT.hh"

namespace Katydid
{
    KTChirpSpaceDataFFTCore::KTChirpSpaceDataFFTCore() :
            KTChirpSpaceData(),
            fSpectra(1)
    {
        fSpectra[0] = NULL;
    }

    KTChirpSpaceDataFFTCore::~KTChirpSpaceDataFFTCore()
    {
        while (! fSpectra.empty())
        {
            delete fSpectra.back();
            fSpectra.pop_back();
        }
    }


    const std::string KTChirpSpaceDataFFT::sName("chirp-space-fft");

    KTChirpSpaceDataFFT::KTChirpSpaceDataFFT() :
            KTChirpSpaceDataFFTCore(),
            KTExtensibleData< KTChirpSpaceDataFFT >()
    {
    }

    KTChirpSpaceDataFFT::~KTChirpSpaceDataFFT()
    {
    }
//
    KTChirpSpaceDataFFT& KTChirpSpaceDataFFT::SetNComponents(unsigned SlopeComponent, unsigned InterceptComponent)
    {
        unsigned oldSize = fSpectra.size();
        // if components < oldSize
        for (unsigned iComponent = SlopeComponent; iComponent < oldSize; ++iComponent)
        {
	    for (unsigned jComponent = InterceptComponent; jComponent < oldSize; ++jComponent)
            {
		delete fSpectra[iComponent,jComponent];
	    }
        }
        fSpectra.resize(SlopeComponent);
        // if components > oldSize
        for (unsigned iComponent = oldSize; iComponent < SlopeComponent; ++iComponent)
        {
	    for (unsigned jComponent = oldSize; jComponent < InterceptComponent; ++jComponent)
            {
            	fSpectra[iComponent,jComponent] = NULL;
	    }
        }
        return *this;
    }


    const std::string KTChirpSpaceVarianceDataFFT::sName("chirp-space-variance-fft");

    KTChirpSpaceVarianceDataFFT::KTChirpSpaceVarianceDataFFT() :
            KTChirpSpaceVarianceDataCore(),
            KTExtensibleData()
    {
    }

    KTChirpSpaceVarianceDataFFT::~KTChirpSpaceVarianceDataFFT()
    {
    }

    KTChirpSpaceVarianceDataFFT& KTChirpSpaceVarianceDataFFT::SetNComponents(unsigned SlopeComponent, unsigned InterceptComponent)
    {
        unsigned oldSize = fSpectra.size();
        // if num < oldSize
        for (unsigned iComponent = SlopeComponent; iComponent < oldSize; ++iComponent)
        {
	    for (unsigned jComponent = InterceptComponent; jComponent < oldSize; ++jComponent)
	    {
            	delete fSpectra[iComponent, jComponent];
	    }
        }
        fSpectra.resize(SlopeComponent);
        // if num > oldSize
        for (unsigned iComponent = oldSize; iComponent < SlopeComponent; ++iComponent)
        {
	    for (unsigned jComponent = oldSize; jComponent < InterceptComponent; ++jComponent)
       	    {
            	fSpectra[iComponent,jComponent] = NULL;
	    }
        }
        return *this;
    }


} /* namespace Katydid */

