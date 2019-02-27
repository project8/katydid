/*
 * KTChannelAggregator.cc
 *
 *  Created on: Jan 25, 2019
 *      Author: P. T. Surukuchi
 */

#include "KTChannelAggregator.hh"

#include "KTLogger.hh"

namespace Katydid
{
  KTLOGGER(evlog, "KTChannelAggregator");
  
  // Register the processor
  KT_REGISTER_PROCESSOR(KTChannelAggregator, "channel-aggregator");
  
  KTChannelAggregator::KTChannelAggregator(const std::string& name) :
  KTProcessor(name),
  fChPowerSumSlot("ps", this, &KTChannelAggregator::SumChannelPower, &fSummedPowerData),
  fChPSDSumSlot("psd", this, &KTChannelAggregator::SumChannelPSD, &fSummedPSDData),
  fSummedPowerData("agg-ps", this),
  fSummedPSDData("agg-psd", this)
  {
  }
  
  KTChannelAggregator::~KTChannelAggregator()
  {
  }
  
  bool KTChannelAggregator::Configure(const scarab::param_node* node)
  {
    if (node == NULL) return false;
    return true;
  }
  
  bool KTChannelAggregator::SumChannelPower(KTPowerSpectrumData& chData)
  {
    unsigned nComponents = chData.GetNComponents();
    
    int arraySize=(chData.GetSpectrum(0))->size();
    KTChannelAggregatedData& newData = chData.Of< KTChannelAggregatedData >().SetNComponents(1);
    
    KTPowerSpectrum* newSpectrum = new KTPowerSpectrum(arraySize, chData.GetSpectrum(0)->GetRangeMin(), chData.GetSpectrum(0)->GetRangeMax());

    NullPowerSpectrum(*newSpectrum);
    //Looping through all the components
    for (unsigned iComponent=0; iComponent<nComponents; ++iComponent)
    {
      (*newSpectrum)+=(*chData.GetSpectrum(iComponent));
      newSpectrum->ConvertToPowerSpectrum();
      KTDEBUG(evlog, "Channel "<< iComponent <<" added to the aggregate");
    }
    newData.SetSpectrum(newSpectrum, 0);
    KTINFO(evlog, "Completed channel aggegation of " << nComponents << " power spectra");
    return true;
  }
  
  bool KTChannelAggregator::SumChannelPSD(KTPowerSpectrumData& chData)
  {
    unsigned nComponents = chData.GetNComponents();
    
    int arraySize=(chData.GetSpectrum(0))->size();
    KTChannelAggregatedData& newData = chData.Of< KTChannelAggregatedData >().SetNComponents(1);
    
    KTPowerSpectrum* newSpectrum = new KTPowerSpectrum(arraySize, chData.GetSpectrum(0)->GetRangeMin(), chData.GetSpectrum(0)->GetRangeMax());

    NullPowerSpectrum(*newSpectrum);
    //Looping through all the components
    for (unsigned iComponent=0; iComponent<nComponents; ++iComponent)
    {
      (*newSpectrum)+=(*chData.GetSpectrum(iComponent));
      newSpectrum->ConvertToPowerSpectralDensity();
      KTDEBUG(evlog, "Channel "<< iComponent <<" added to the aggregate");
    }
    newData.SetSpectrum(newSpectrum, 0);
    KTINFO(evlog, "Completed channel aggegation of " << nComponents << " power spectra");
    return true;
  }
} // namespace Katydid
