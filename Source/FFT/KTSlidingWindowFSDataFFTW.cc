/*
 * KTSlidingWindowFSDataFFTW.cc
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#include "KTSlidingWindowFSDataFFTW.hh"

#include "KTFrequencySpectrumFFTW.hh"
#include "KTWriter.hh"

using std::vector;

namespace Katydid
{
    std::string KTSlidingWindowFSDataFFTW::fName("sliding-window-fs-fftw");

    const std::string& KTSlidingWindowFSDataFFTW::StaticGetName()
    {
        return fName;
    }

    KTSlidingWindowFSDataFFTW::KTSlidingWindowFSDataFFTW(unsigned nChannels) :
            KTWriteableData(),
            fSpectra(nChannels)
    {
    }

    KTSlidingWindowFSDataFFTW::~KTSlidingWindowFSDataFFTW()
    {
        while (! fSpectra.empty())
        {
            KTPhysicalArray< 1, KTFrequencySpectrumFFTW* >* backSpectra = fSpectra.back();
            for (KTPhysicalArray< 1, KTFrequencySpectrumFFTW* >::iterator iter = backSpectra->begin(); iter != backSpectra->end(); iter++)
            {
                delete *iter;
            }
            delete backSpectra;
            fSpectra.pop_back();
        }
    }

    void KTSlidingWindowFSDataFFTW::Accept(KTWriter* writer) const
    {
        writer->Write(this);
        return;
    }


} /* namespace Katydid */

