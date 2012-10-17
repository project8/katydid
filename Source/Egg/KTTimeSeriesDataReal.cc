/*
 * KTTimeSeriesDataReal.cc
 *
 *  Created on: Oct 16, 2012
 *      Author: nsoblath
 */

#include "KTTimeSeriesDataReal.hh"

namespace Katydid
{
    std::string KTTimeSeriesDataReal::fName("time-series-real");

    const std::string& KTTimeSeriesDataReal::StaticGetName()
    {
        return fName;
    }

    KTTimeSeriesDataReal::KTTimeSeriesDataReal(UInt_t nChannels) :
            KTTimeSeriesData(nChannels)
    {
    }

    KTTimeSeriesDataReal::~KTTimeSeriesDataReal()
    {
    }

} /* namespace Katydid */
