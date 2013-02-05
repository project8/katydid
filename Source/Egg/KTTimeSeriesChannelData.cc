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


  static KTDerivedTIRegistrar< KTDataMap, KTDerivedDataMap< KTBasicTimeSeriesData > > sBTSDRegistrar;

    KTBasicTimeSeriesData::KTBasicTimeSeriesData(UInt_t nChannels) :
            KTTimeSeriesChannelData(),
            fTimeInRun(0.),
            fSliceNumber(0)
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
            fSliceLength(0.),
            fBinWidth(1.),
            fSliceSize(0),
            fTimeInRun(0.),
            fSliceNumber(0),
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
