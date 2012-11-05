/*
 * KTCorrelationDataFFTW.cc
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#include "KTCorrelationDataFFTW.hh"

#include "KTFrequencySpectrumFFTW.hh"
#include "KTWriter.hh"

namespace Katydid
{
    std::string KTCorrelationDataFFTW::fName("correlation-fftw");

    const std::string& KTCorrelationDataFFTW::StaticGetName()
    {
        return fName;
    }

    KTCorrelationDataFFTW::KTCorrelationDataFFTW(UInt_t nPairs) :
            KTWriteableData(),
            fData(nPairs)
    {
    }

    KTCorrelationDataFFTW::~KTCorrelationDataFFTW()
    {
        while (! fData.empty())
        {
            delete fData.back().fCorrelation;
            fData.pop_back();
        }
    }

    void KTCorrelationDataFFTW::Accept(KTWriter* writer) const
    {
        writer->Write(this);
        return;
    }


} /* namespace Katydid */

