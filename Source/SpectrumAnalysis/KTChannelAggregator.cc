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
  fMemberVariable1(0.),
  // If you would like to build this template as-is, you must comment out the following slot initializations because they use made-up data types
  fChSumSlot("ps-ch", this, &KTChannelAggregator::SumChannels, &summedData),
  summedData("ps", this)
  {
  }
  
  KTChannelAggregator::~KTChannelAggregator()
  {
  }
  
  bool KTChannelAggregator::Configure(const scarab::param_node* node)
  {
    if (node == NULL) return false;
    
    SetMemberVariable1(node->get_value< double >("member-variable-1", fMemberVariable1));
    
    return true;
  }
  
  bool KTChannelAggregator::SumChannels( KTPowerSpectrumData& chData )
  {
    unsigned nComponents = chData.GetNComponents();
    int arraySize=(chData.GetSpectrum(0))->size();
    KTChannelAggregatedData& newData = chData.Of< KTChannelAggregatedData >().SetNComponents(nComponents);
    
    KTPowerSpectrum* newSpectrum = new KTPowerSpectrum(arraySize, chData.GetSpectrum(0)->GetRangeMin(), chData.GetSpectrum(0)->GetRangeMax());
    //Looping through all the components
    for (unsigned iComponent=0; iComponent<nComponents; ++iComponent)
    {
      (*newSpectrum)+=(*chData.GetSpectrum(iComponent));
      KTDEBUG(evlog, "Channel aggregation completed");
    }
    newData.SetSpectrum(newSpectrum, 0);
    for (unsigned i = 0; i < newSpectrum->size(); ++i)
    {
      //Currently adds the channel power spectrum for all the segments and print out for
      if(i%100==0)
        std::cout<< i<< "  "<< (*chData.GetSpectrum(0))(i) << " "<< (*newSpectrum)(i) <<std::endl;
    }
    KTINFO(evlog, "Completed channel aggegation of " << nComponents << " power spectra");
    
    return true;
  }
} // namespace Katydid
