/*
 * KTFrequencySpectrumDataFFTW.cc
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#include "KTFrequencySpectrumDataFFTW.hh"

namespace Katydid
{
    KTFrequencySpectrumDataFFTWCore::KTFrequencySpectrumDataFFTWCore() :
            KTFrequencySpectrumData(),
            fSpectra(1)
    {
        fSpectra[0] = NULL;
    }

    KTFrequencySpectrumDataFFTWCore::~KTFrequencySpectrumDataFFTWCore()
    {
        while (! fSpectra.empty())
        {
            delete fSpectra.back();
            fSpectra.pop_back();
        }
    }


    KTFrequencySpectrumDataFFTW::KTFrequencySpectrumDataFFTW() :
            KTFrequencySpectrumDataFFTWCore(),
            Data()
    {
    }

    KTFrequencySpectrumDataFFTW::~KTFrequencySpectrumDataFFTW()
    {
    }

    KTFrequencySpectrumDataFFTW& KTFrequencySpectrumDataFFTW::SetNComponents(unsigned components)
    {
        unsigned oldSize = fSpectra.size();
        // if components < oldSize
        for (unsigned iComponent = components; iComponent < oldSize; ++iComponent)
        {
            delete fSpectra[iComponent];
        }
        fSpectra.resize(components);
        // if components > oldSize
        for (unsigned iComponent = oldSize; iComponent < components; ++iComponent)
        {
            fSpectra[iComponent] = NULL;
        }
        return *this;
    }


    KTFrequencySpectrumVarianceDataFFTW::KTFrequencySpectrumVarianceDataFFTW() :
            KTFrequencySpectrumVarianceDataCore(),
            Data()
    {
    }

    KTFrequencySpectrumVarianceDataFFTW::~KTFrequencySpectrumVarianceDataFFTW()
    {
    }

    KTFrequencySpectrumVarianceDataFFTW& KTFrequencySpectrumVarianceDataFFTW::SetNComponents(unsigned num)
    {
        unsigned oldSize = fSpectra.size();
        // if num < oldSize
        for (unsigned iComponent = num; iComponent < oldSize; ++iComponent)
        {
            delete fSpectra[iComponent];
        }
        fSpectra.resize(num);
        // if num > oldSize
        for (unsigned iComponent = oldSize; iComponent < num; ++iComponent)
        {
            fSpectra[iComponent] = NULL;
        }
        return *this;
    }


} /* namespace Katydid */

