/*
 * KTConvolvedSpectrumData.cc
 *
 *  Created on: Aug 25, 2017
 *      Author: ezayas
 */

#include "KTConvolvedSpectrumData.hh"


namespace Katydid
{
    const std::string KTConvolvedPowerSpectrumData::sName("convolved-power-spectrum");

    KTConvolvedPowerSpectrumData::KTConvolvedPowerSpectrumData() :
            KTPowerSpectrumDataCore(),
            KTExtensibleData()
    {
    }

    KTConvolvedPowerSpectrumData::~KTConvolvedPowerSpectrumData()
    {
    }

    KTConvolvedPowerSpectrumData& KTConvolvedPowerSpectrumData::SetNComponents(unsigned num)
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

    const std::string KTConvolvedFrequencySpectrumDataFFTW::sName("convolved-frequency-spectrum-fftw");

    KTConvolvedFrequencySpectrumDataFFTW::KTConvolvedFrequencySpectrumDataFFTW() :
            KTFrequencySpectrumDataFFTWCore(),
            KTExtensibleData()
    {
    }

    KTConvolvedFrequencySpectrumDataFFTW::~KTConvolvedFrequencySpectrumDataFFTW()
    {
    }

    KTConvolvedFrequencySpectrumDataFFTW& KTConvolvedFrequencySpectrumDataFFTW::SetNComponents(unsigned num)
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

    const std::string KTConvolvedFrequencySpectrumDataPolar::sName("convolved-frequency-spectrum-polar");

    KTConvolvedFrequencySpectrumDataPolar::KTConvolvedFrequencySpectrumDataPolar() :
            KTFrequencySpectrumDataPolarCore(),
            KTExtensibleData()
    {
    }

    KTConvolvedFrequencySpectrumDataPolar::~KTConvolvedFrequencySpectrumDataPolar()
    {
    }

    KTConvolvedFrequencySpectrumDataPolar& KTConvolvedFrequencySpectrumDataPolar::SetNComponents(unsigned num)
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

