/*
 * KTTimeSeriesData.cc
 *
 *  Created on: Sep 9, 2011
 *      Author: nsoblath
 */

#include "KTTimeSeriesData.hh"

using std::vector;

namespace Katydid
{
    KTBasicTimeSeriesData::KTBasicTimeSeriesData() :
            KTWriteableData()
    {
    }

    KTBasicTimeSeriesData::~KTBasicTimeSeriesData()
    {
    }



    KTTimeSeriesData::KTTimeSeriesData(UInt_t nChannels) :
            KTBasicTimeSeriesData(),
            fSampleRate(0.),
            fRecordLength(0.),
            fBinWidth(1.),
            fRecordSize(0),
            fChannelData(nChannels)
    {
    }

    KTTimeSeriesData::~KTTimeSeriesData()
    {
        while (! fChannelData.empty())
        {
            delete fChannelData.back().fRecord;
            fChannelData.pop_back();
        }
    }

} /* namespace Katydid */
