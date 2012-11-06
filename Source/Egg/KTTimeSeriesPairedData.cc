/*
 * KTTimeSeriesPairedData.cc
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#include "KTTimeSeriesPairedData.hh"

#include "KTTimeSeries.hh"
#include "KTWriter.hh"

namespace Katydid
{
    std::string KTTimeSeriesPairedData::fName("time-series-paired");

    const std::string& KTTimeSeriesPairedData::StaticGetName()
    {
        return fName;
    }

    KTTimeSeriesPairedData::KTTimeSeriesPairedData(UInt_t nPairs) :
            KTTimeSeriesData(),
            fData(nPairs)
    {
    }

    KTTimeSeriesPairedData::~KTTimeSeriesPairedData()
    {
        while (! fData.empty())
        {
            delete fData.back().fTimeSeries;
            fData.pop_back();
        }
    }

    void KTTimeSeriesPairedData::Accept(KTWriter* writer) const
    {
        writer->Write(this);
        return;
    }


} /* namespace Katydid */

