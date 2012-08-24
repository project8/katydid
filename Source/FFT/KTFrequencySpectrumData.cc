/*
 * KTFrequencySpectrumData.cc
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#include "KTFrequencySpectrumData.hh"

namespace Katydid
{
    std::string KTFrequencySpectrumData::fName("FrequencySpectrum");

    const std::string& KTFrequencySpectrumData::StaticGetName()
    {
        return fName;
    }

    KTFrequencySpectrumData::KTFrequencySpectrumData(unsigned nChannels) :
            KTData(),
            fSpectra(nChannels)
    {
    }

    KTFrequencySpectrumData::~KTFrequencySpectrumData()
    {
        while (! fSpectra.empty())
        {
            delete fSpectra.back();
            fSpectra.pop_back();
        }
    }

} /* namespace Katydid */
