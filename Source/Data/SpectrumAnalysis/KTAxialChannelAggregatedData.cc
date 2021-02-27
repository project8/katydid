/*
 * KTAxialChannelAggregatedData.cc
 *
 *  Created on: Apr 28, 2020
 *      Author: P. T. Surukuchi
 */

#include "KTAxialChannelAggregatedData.hh"

namespace Katydid
{
    /*****************  Beginning of KTAxialAggregatedFrequencySpectrumDataFFTW ************************/

    const std::string KTAxialAggregatedFrequencySpectrumDataFFTW::sName("axial-aggregated-frequency-spectrum-fftw");

    KTAxialAggregatedFrequencySpectrumDataFFTW::KTAxialAggregatedFrequencySpectrumDataFFTW() :
            KTAxialAggregatedDataCore(),
            KTFrequencySpectrumDataFFTWCore(),
            Nymph::KTExtensibleData< KTAxialAggregatedFrequencySpectrumDataFFTW >()
    {
    }

    KTAxialAggregatedFrequencySpectrumDataFFTW::~KTAxialAggregatedFrequencySpectrumDataFFTW()
    {
    }

    KTAxialAggregatedFrequencySpectrumDataFFTW& KTAxialAggregatedFrequencySpectrumDataFFTW::SetNComponents(unsigned num)
    {
        unsigned oldSize = fSpectra.size();

        // If old size is bigger than num, delete all the extra terms
        for (unsigned iComponent = num; iComponent < oldSize; ++iComponent)
        {
            delete fSpectra[iComponent];
        }

        //Resize old size is smaller than old size
        fSpectra.resize(num);

        for (unsigned iComponent = oldSize; iComponent < num; ++iComponent)
        {
            fSpectra[iComponent] = NULL;
        }
        return *this;
    }
    /*****************  End of KTAxialAggregatedFrequencySpectrumDataFFTW ************************/

    const std::string KTAxialAggregatedPowerSpectrumData::sName("axial-aggregated-frequency-spectrum-fftw");

    KTAxialAggregatedPowerSpectrumData::KTAxialAggregatedPowerSpectrumData() :
            KTAxialAggregatedDataCore(),
            KTPowerSpectrumDataCore(),
            Nymph::KTExtensibleData< KTAxialAggregatedPowerSpectrumData >()
    {
    }

    KTAxialAggregatedPowerSpectrumData::~KTAxialAggregatedPowerSpectrumData()
    {
    }

    KTAxialAggregatedPowerSpectrumData& KTAxialAggregatedPowerSpectrumData::SetNComponents(unsigned num)
    {
        unsigned oldSize = fSpectra.size();

        // If old size is bigger than num, delete all the extra terms
        for (unsigned iComponent = num; iComponent < oldSize; ++iComponent)
        {
            delete fSpectra[iComponent];
        }

        //Resize old size is smaller than old size
        fSpectra.resize(num);

        for (unsigned iComponent = oldSize; iComponent < num; ++iComponent)
        {
            fSpectra[iComponent] = NULL;
        }
        return *this;
    }
} /* namespace Katydid */

