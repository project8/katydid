/*
 * KTTimeSeriesChannelData.cc
 *
 *  Created on: Sep 9, 2011
 *      Author: nsoblath
 */

#include "KTTimeSeriesChannelData.hh"

#include "KTWriter.hh"

using std::vector;

namespace Katydid
{
    KTTimeSeriesChannelData::KTTimeSeriesChannelData() :
            KTTimeSeriesData()
    {
    }

    KTTimeSeriesChannelData::~KTTimeSeriesChannelData()
    {
    }


    KTBasicTimeSeriesData::KTBasicTimeSeriesData(UInt_t nChannels) :
            KTTimeSeriesChannelData()
    {
    }

    KTBasicTimeSeriesData::~KTBasicTimeSeriesData()
    {
        while (! fChannelData.empty())
        {
            delete fChannelData.back();
            fChannelData.pop_back();
        }
    }

    void KTBasicTimeSeriesData::Accept(KTWriter* writer) const
    {
        writer->Write(this);
        return;
    }



    KTProgenitorTimeSeriesData::KTProgenitorTimeSeriesData(UInt_t nChannels) :
            KTTimeSeriesChannelData(),
            fSampleRate(0.),
            fRecordLength(0.),
            fBinWidth(1.),
            fRecordSize(0),
            fChannelData(nChannels)
    {
    }

    KTProgenitorTimeSeriesData::~KTProgenitorTimeSeriesData()
    {
        while (! fChannelData.empty())
        {
            delete fChannelData.back().fRecord;
            fChannelData.pop_back();
        }
    }

    void KTProgenitorTimeSeriesData::Accept(KTWriter* writer) const
    {
        writer->Write(this);
        return;
    }


} /* namespace Katydid */
