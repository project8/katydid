/*
 * KTPairedTimeSeriesData.cc
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#include "KTPairedTimeSeriesData.hh"

#include "KTFrequencySpectrumFFTW.hh"
#include "KTWriter.hh"

namespace Katydid
{
    std::string KTPairedTimeSeriesData::fName("correlation-fftw");

    const std::string& KTPairedTimeSeriesData::StaticGetName()
    {
        return fName;
    }

    KTPairedTimeSeriesData::KTPairedTimeSeriesData(UInt_t nPairs) :
            KTWriteableData(),
            fData(nPairs)
    {
    }

    KTPairedTimeSeriesData::~KTPairedTimeSeriesData()
    {
        while (! fData.empty())
        {
            delete fData.back().fCorrelation;
            fData.pop_back();
        }
    }

    void KTPairedTimeSeriesData::Accept(KTWriter* writer) const
    {
        writer->Write(this);
        return;
    }


} /* namespace Katydid */

