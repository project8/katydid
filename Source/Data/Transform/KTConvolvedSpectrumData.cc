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

    const std::string KTConvolvedPowerSpectrumVarianceData::sName("convolved-power-spectrum-variance");

    KTConvolvedPowerSpectrumVarianceData::KTConvolvedPowerSpectrumVarianceData() :
            KTPowerSpectrumDataCore(),
            KTExtensibleData()
    {
    }

    KTConvolvedPowerSpectrumVarianceData::~KTConvolvedPowerSpectrumVarianceData()
    {
    }

    KTConvolvedPowerSpectrumVarianceData& KTConvolvedPowerSpectrumVarianceData::SetNComponents(unsigned num)
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

    const std::string KTConvolvedFrequencySpectrumVarianceDataFFTW::sName("convolved-frequency-spectrum-variance-fftw");

    KTConvolvedFrequencySpectrumVarianceDataFFTW::KTConvolvedFrequencySpectrumVarianceDataFFTW() :
            KTFrequencySpectrumDataFFTWCore(),
            KTExtensibleData()
    {
    }

    KTConvolvedFrequencySpectrumVarianceDataFFTW::~KTConvolvedFrequencySpectrumVarianceDataFFTW()
    {
    }

    KTConvolvedFrequencySpectrumVarianceDataFFTW& KTConvolvedFrequencySpectrumVarianceDataFFTW::SetNComponents(unsigned num)
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

    const std::string KTConvolvedFrequencySpectrumVarianceDataPolar::sName("convolved-frequency-spectrum-variance-polar");

    KTConvolvedFrequencySpectrumVarianceDataPolar::KTConvolvedFrequencySpectrumVarianceDataPolar() :
            KTFrequencySpectrumDataPolarCore(),
            KTExtensibleData()
    {
    }

    KTConvolvedFrequencySpectrumVarianceDataPolar::~KTConvolvedFrequencySpectrumVarianceDataPolar()
    {
    }

    KTConvolvedFrequencySpectrumVarianceDataPolar& KTConvolvedFrequencySpectrumVarianceDataPolar::SetNComponents(unsigned num)
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
