/*
 * KTFrequencySpectrumData.cc
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#include "KTFrequencySpectrumData.hh"

#include "KTDataMap.hh"
#include "KTFrequencySpectrum.hh"
#include "KTTIFactory.hh"
#include "KTWriter.hh"

namespace Katydid
{
    static KTDerivedTIRegistrar< KTDataMap, KTDerivedDataMap< KTFrequencySpectrumData > > sFSDMMRegistrar;

    KTFrequencySpectrumData::KTFrequencySpectrumData(UInt_t nChannels) :
            KTWriteableData(),
            fSpectra(nChannels),
            fTimeInRun(0.),
            fSliceNumber(0)
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

    void KTFrequencySpectrumData::Accept(KTWriter* writer) const
    {
        writer->Write(this);
        return;
    }

} /* namespace Katydid */

