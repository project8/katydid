/*
 * KTSpectrumVarianceData.cc
 *
 *  Created on: Feb 9, 2018
 *      Author: ezayas
 */

#include "KTSpectrumVarianceData.hh"


namespace Katydid
{
    const std::string KTPowerSpectrumVarianceData::sName("power-spectrum-variance");

    KTPowerSpectrumVarianceData::KTPowerSpectrumVarianceData() :
            KTPowerSpectrumDataCore(),
            KTExtensibleData()
    {
    }

    KTPowerSpectrumVarianceData::~KTPowerSpectrumVarianceData()
    {
    }

    KTPowerSpectrumVarianceData& KTPowerSpectrumVarianceData::SetNComponents(unsigned num)
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

    const std::string KTFrequencySpectrumVarianceDataFFTW::sName("frequency-spectrum-variance-fftw");

    KTFrequencySpectrumVarianceDataFFTW::KTFrequencySpectrumVarianceDataFFTW() :
            KTFrequencySpectrumDataFFTWCore(),
            KTExtensibleData()
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

    const std::string KTFrequencySpectrumVarianceDataPolar::sName("frequency-spectrum-variance-polar");

    KTFrequencySpectrumVarianceDataPolar::KTFrequencySpectrumVarianceDataPolar() :
            KTFrequencySpectrumDataPolarCore(),
            KTExtensibleData()
    {
    }

    KTFrequencySpectrumVarianceDataPolar::~KTFrequencySpectrumVarianceDataPolar()
    {
    }

    KTFrequencySpectrumVarianceDataPolar& KTFrequencySpectrumVarianceDataPolar::SetNComponents(unsigned num)
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
