/*
 * KTFrequencySpectrumDataPolar.cc
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#include "KTFrequencySpectrumDataPolar.hh"

#include "KTDataMap.hh"
#include "KTFrequencySpectrumPolar.hh"
#include "KTTIFactory.hh"
#include "KTWriter.hh"

namespace Katydid
{
    static KTDerivedTIRegistrar< KTDataMap, KTDerivedDataMap< KTFrequencySpectrumDataPolar > > sFSDMMRegistrar;

    KTFrequencySpectrumDataPolar::KTFrequencySpectrumDataPolar(UInt_t nChannels) :
            KTFrequencySpectrumData(),
            fSpectra(nChannels),
            fTimeInRun(0.),
            fTimeLength(0.),
            fSliceNumber(0)
    {
    }

    KTFrequencySpectrumDataPolar::~KTFrequencySpectrumDataPolar()
    {
        while (! fSpectra.empty())
        {
            delete fSpectra.back();
            fSpectra.pop_back();
        }
    }

    void KTFrequencySpectrumDataPolar::Accept(KTWriter* writer) const
    {
        writer->Write(this);
        return;
    }

} /* namespace Katydid */

