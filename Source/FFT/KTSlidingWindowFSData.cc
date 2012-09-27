/*
 * KTSlidingWindowFSData.cc
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#include "KTSlidingWindowFSData.hh"

#include "KTFrequencySpectrum.hh"
#include "KTWriter.hh"

using std::vector;

namespace Katydid
{
    std::string KTSlidingWindowFSData::fName("sliding-window-fs");

    const std::string& KTSlidingWindowFSData::StaticGetName()
    {
        return fName;
    }

    KTSlidingWindowFSData::KTSlidingWindowFSData(unsigned nChannels) :
            KTWriteableData(),
            fSpectra(nChannels)
    {
    }

    KTSlidingWindowFSData::~KTSlidingWindowFSData()
    {
        while (! fSpectra.empty())
        {
            KTPhysicalArray< 1, KTFrequencySpectrum* >* backSpectra = fSpectra.back();
            for (KTPhysicalArray< 1, KTFrequencySpectrum* >::iterator iter = backSpectra->begin(); iter != backSpectra->end(); iter++)
            {
                delete *iter;
            }
            delete backSpectra;
            fSpectra.pop_back();
        }
    }

    void KTSlidingWindowFSData::Accept(KTWriter* writer) const
    {
        writer->Write(this);
        return;
    }


} /* namespace Katydid */

