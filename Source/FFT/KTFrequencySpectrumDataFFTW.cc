/*
 * KTFrequencySpectrumDataFFTW.cc
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#include "KTFrequencySpectrumDataFFTW.hh"

#include "KTFrequencySpectrumFFTW.hh"
#include "KTTIFactory.hh"
#include "KTWriter.hh"

namespace Katydid
{
    static KTDerivedTIRegistrar< KTDataMap, KTDerivedDataMap< KTFrequencySpectrumDataFFTW > > sFSFFTWDMMRegistrar;

    KTFrequencySpectrumDataFFTW::KTFrequencySpectrumDataFFTW(unsigned nChannels) :
            KTWriteableData(),
            fSpectra(nChannels)
    {
    }

    KTFrequencySpectrumDataFFTW::~KTFrequencySpectrumDataFFTW()
    {
        while (! fSpectra.empty())
        {
            delete fSpectra.back();
            fSpectra.pop_back();
        }
    }

    void KTFrequencySpectrumDataFFTW::Accept(KTWriter* writer) const
    {
        writer->Write(this);
        return;
    }

} /* namespace Katydid */

