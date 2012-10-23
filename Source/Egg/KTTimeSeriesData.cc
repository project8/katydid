/*
 * KTTimeSeriesData.cc
 *
 *  Created on: Sep 9, 2011
 *      Author: nsoblath
 */

#include "KTTimeSeriesData.hh"

#include "KTWriter.hh"

using std::vector;

namespace Katydid
{
    KTTimeSeriesData::KTTimeSeriesData() :
            KTWriteableData()
    {
    }

    KTTimeSeriesData::~KTTimeSeriesData()
    {
    }


    std::string KTBasicTimeSeriesData::fName("basic-time-series");

    const std::string& KTBasicTimeSeriesData::StaticGetName()
    {
        return fName;
    }

    KTBasicTimeSeriesData::KTBasicTimeSeriesData(UInt_t nChannels) :
            KTTimeSeriesData()
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



    std::string KTProgenitorTimeSeriesData::fName("progenitor-time-series");

    const std::string& KTProgenitorTimeSeriesData::StaticGetName()
    {
        return fName;
    }

    KTProgenitorTimeSeriesData::KTProgenitorTimeSeriesData(UInt_t nChannels) :
            KTTimeSeriesData(),
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
