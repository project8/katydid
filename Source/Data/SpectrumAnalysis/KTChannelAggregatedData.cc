/*
 * KTChannelAggregatedData.cc
 *
 *  Created on: Jan 28, 2019
 *      Author: P. T. Surukuchi
 */

#include "KTChannelAggregatedData.hh"

namespace Katydid
{
  KTChannelAggregatedDataCore::KTChannelAggregatedDataCore()
  {
  }
  
  KTChannelAggregatedDataCore::~KTChannelAggregatedDataCore()
  {
    while (! fSpectra.empty())
    {
      delete fSpectra.back();
      fSpectra.pop_back();
    }
  }
  
  const std::string KTChannelAggregatedData::sName("channel-aggregate");
  
  KTChannelAggregatedData::KTChannelAggregatedData() :
  KTChannelAggregatedDataCore(),
  KTExtensibleData<KTChannelAggregatedData>()
  {
  }
  
  KTChannelAggregatedData::~KTChannelAggregatedData()
  {
  }
  
  KTChannelAggregatedData& KTChannelAggregatedData::SetNComponents(unsigned num)
  {
    unsigned oldSize = fSpectra.size();
    for (unsigned iComponent = num; iComponent < oldSize; ++iComponent)
    {
      delete fSpectra[iComponent];
    }
    fSpectra.resize(num);
    for (unsigned iComponent = oldSize; iComponent < num; ++iComponent)
    {
      fSpectra[iComponent] = NULL;
    }
    return *this;
  }
  
} /* namespace Katydid */

