/*
 * KTFrequencySpectrumData.cc
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#include "KTFrequencySpectrumData.hh"

#include "KTPublisher.hh"

#include <typeinfo>

namespace Katydid
{
    std::string KTFrequencySpectrumData::fName("FrequencySpectrum");

    const std::string& KTFrequencySpectrumData::StaticGetName()
    {
        return fName;
    }

    KTFrequencySpectrumData::KTFrequencySpectrumData(unsigned nChannels) :
            KTWriteableData(),
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

    void KTFrequencySpectrumData::Accept(KTPublisher* publisher) const
    {
        publisher->Write(this);
        return;
    }


} /* namespace Katydid */

