/*
 * KTTimeSeriesDataFFTW.cc
 *
 *  Created on: Oct 16, 2012
 *      Author: nsoblath
 */

#include "KTTimeSeriesDataFFTW.hh"

#include "KTWriter.hh"


namespace Katydid
{
    std::string KTBasicTimeSeriesDataFFTW::fName("basic-time-series-fftw");

    const std::string& KTBasicTimeSeriesDataFFTW::StaticGetName()
    {
        return fName;
    }

    KTBasicTimeSeriesDataFFTW::KTBasicTimeSeriesDataFFTW(UInt_t nChannels) :
            KTBasicTimeSeriesData()
    {
    }

    KTBasicTimeSeriesDataFFTW::~KTBasicTimeSeriesDataFFTW()
    {
    }

    void KTBasicTimeSeriesDataFFTW::Accept(KTWriter* writer) const
    {
        writer->Write(this);
        return;
    }




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

    void KTTimeSeriesDataFFTW::Accept(KTWriter* writer) const
    {
        writer->Write(this);
        return;
    }



} /* namespace Katydid */
