/*
 * KTAggregatedChannelOptimizer.hh
 *
 *  Created on: Apr 21, 2019
 *      Author: P. T. Surukuchi
 */

#ifndef KTAGGREGATEDCHANNELOPTIMIZER_HH_
#define KTAGGREGATEDCHANNELOPTIMIZER_HH_

#include "KTProcessor.hh"
#include "KTData.hh"
#include "KTChannelAggregatedData.hh"
#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTAxisProperties.hh"

#include "KTSlot.hh"

#include "KTMath.hh"

namespace Katydid
{
  
  KTLOGGER(avlog_hh, "KTAggregatedChannelOptimizer.hh");
  
  class KTAggregatedFrequencySpectrumDataFFTW;
  class KTFrequencySpectrumDataFFTW;
  
  /*
   @class KTAggregatedChannelOptimizer
   @author P. T. Surukuchi
   
   @brief Finds the point that optimizes the aggreagted channels
   
   @details
   
   Slots:
   - "fft": void (Nymph::KTDataPtr) -- Finds the point that optimizes the aggreagted channels; Requires KTAggregatedFrequencySpectrumDataFFTW;Finds the point that optimizes the aggreagted channels ; Emits signal "fft"
   
   Signals:
   - "fft": void (Nymph::KTDataPtr) -- Emitted upon finding the optimized point ; Guarantees KTFrequencySpectrumDataFFTW
   */
  
  class KTAggregatedChannelOptimizer : public Nymph::KTProcessor
  {
  public:
    KTAggregatedChannelOptimizer(const std::string& name = "channelsum-optimizer");
    virtual ~KTAggregatedChannelOptimizer();
    
    bool Configure(const scarab::param_node* node);
    
  private:
    
    bool FindOptimumSum( KTAggregatedFrequencySpectrumDataFFTW& );
    
    //***************
    // Signals
    //***************
    
  private:
    Nymph::KTSignalData fSummedFrequencyData;
    
    //***************
    // Slots
    //***************
    
  private:
    Nymph::KTSlotDataOneType< KTAggregatedFrequencySpectrumDataFFTW > fOptimalSumSlot;
  };
}

#endif  /* KTAGGREGATEDCHANNELOPTIMIZER_HH_  */