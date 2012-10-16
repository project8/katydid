/*
 * KTTimeSeriesDataFFTW.cc
 *
 *  Created on: Oct 16, 2012
 *      Author: nsoblath
 */

#include "KTTimeSeriesDataFFTW.hh"


namespace Katydid
{
    std::string KTTimeSeriesDataFFTW::fName("time-series-fftw");

    const std::string& KTTimeSeriesDataFFTW::StaticGetName()
    {
        return fName;
    }

    KTTimeSeriesDataFFTW::KTTimeSeriesDataFFTW(UInt_t nChannels) :
            KTTimeSeriesData(nChannels)
    {
    }

    KTTimeSeriesDataFFTW::~KTTimeSeriesDataFFTW()
    {
    }


} /* namespace Katydid */
