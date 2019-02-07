/*
 * KTChannelAggregator.hh
 *
 *  Created on: Jan 25, 2019
 *      Author: P. T. Surukuchi
 */

#ifndef KTCHANNELAGGREGATOR_HH_
#define KTCHANNELAGGREGATOR_HH_

#include "KTProcessor.hh"
#include "KTData.hh"
#include "KTPowerSpectrumData.hh"
#include "KTChannelAggregatedData.hh"

#include "KTSlot.hh"

namespace Katydid
{
  
  KTLOGGER(avlog_hh, "KTChannelAggregator.hh");
  
  //    class KTChannelAggregatedData; PTS: To be used when the spectrum data is converted to a grid-like datatype
  class KTPowerSpectrumData;
  
  /*
   @class KTChannelAggregator
   @author P. T. Surukuchi
   
   @brief Multiple channel summation for Phase-III and IV
   
   @details
   More details to come.
   
   Configuration name: "channel-aggregator"
   
   Slots:
   - "ps-ch": void (Nymph::KTDataPtr) -- Adds channels PSD now, will be moved to addition in frequencies; Requires KTPowerSpectrumData; Adds summation of the channel results; Emits signal "ps"
   
   Signals:
   - "ps": void (Nymph::KTDataPtr) -- Emitted upon summation of all channels; Guarantees KTPowerSpectrumData
   */
  
  class KTChannelAggregator : public Nymph::KTProcessor
  {
  public:
    KTChannelAggregator(const std::string& name = "channel-aggregator");
    virtual ~KTChannelAggregator();
    
    bool Configure(const scarab::param_node* node);
    
    double GetMemberVariable1() const;
    void SetMemberVariable1(double value);
    
  private:
    double fMemberVariable1;
    
  public:
    bool SumChannels( KTPowerSpectrumData& chData );
    
//    KTPowerSpectrum* SumChannels(const KTPowerSpectrum* powerSpectrum) const;
    //***************
    // Signals
    //***************
    
  private:
    Nymph::KTSignalData summedData;
    
    //***************
    // Slots
    //***************
    
  private:
    Nymph::KTSlotDataOneType< KTPowerSpectrumData > fChSumSlot;
  };
  
  inline double KTChannelAggregator::GetMemberVariable1() const
  {
    return fMemberVariable1;
  }
  
  inline void KTChannelAggregator::SetMemberVariable1(double value)
  {
    fMemberVariable1 = value;
    return;
  }
  
}

#endif  /* KTCHANNELAGGREGATOR_HH_  */
