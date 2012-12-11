/*
 * KTGainVariationData.cc
 *
 *  Created on: Dec 10, 2012
 *      Author: nsoblath
 */

#include "KTGainVariationData.hh"

#include "KTFrequencySpectrum.hh"
#include "KTWriter.hh"

namespace Katydid
{
    KTGainVariationData::KTGainVariationData(unsigned nChannels) :
            KTWriteableData(),
            fSpectra(nChannels)
    {
    }

    KTGainVariationData::~KTGainVariationData()
    {
        while (! fSpectra.empty())
        {
            delete fSpectra.back();
            fSpectra.pop_back();
        }
    }

    void KTGainVariationData::Accept(KTWriter* writer) const
    {
        writer->Write(this);
        return;
    }

} /* namespace Katydid */

