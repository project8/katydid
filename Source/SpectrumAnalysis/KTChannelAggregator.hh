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
  
  class KTChannelAggregatedData;
  class KTPowerSpectrumData;
  
  /*
   @class KTChannelAggregator
   @author P. T. Surukuchi
   
   @brief Multiple channel summation for Phase-III and IV
   
   @details
   More details to come.
   
   Configuration name: "channel-aggregator"
   
   Slots:
   - "ps": void (Nymph::KTDataPtr) -- Adds channels power, will also need to include summation using FFTW-phase information; Requires KTChannelAggregatedData; Adds summation of the channel results; Emits signal "agg-ps"
   - "psd": void (Nymph::KTDataPtr) -- Adds channels PSD, will also need to include summation using FFTW-phase information; Requires KTChannelAggregatedData; Adds summation of the channel results; Emits signal "agg-psd"
   - "fft-phase": void (Nymph::KTDataPtr) -- Adds channels PSD and power using FFTW-phase information for appropriate phase addition; Requires KTChannelAggregatedData; Adds summation of the channel results; Emits signal "agg-psd"
   
   Signals:
   - "agg-ps": void (Nymph::KTDataPtr) -- Emitted upon summation of all channels; Guarantees KTChannelAggregatedData
   - "agg-psd": void (Nymph::KTDataPtr) -- Emitted upon summation of all channels; Guarantees KTChannelAggregatedData
   */
  
  class KTChannelAggregator : public Nymph::KTProcessor
  {
  public:
    KTChannelAggregator(const std::string& name = "channel-aggregator");
    virtual ~KTChannelAggregator();
    
    bool Configure(const scarab::param_node* node);
  private:
    // This function is called once for each time slice
    bool SumChannelPower( KTPowerSpectrumData& );
    bool SumChannelPSD( KTPowerSpectrumData& );
    
  private:
    //PTS: This needs fixing, currently just setting each element to 0. But why does it have to be done to begin with.
    // Perhaps a some function in the utilities to do this ?
    bool NullPowerSpectrum(KTPowerSpectrum &);
    //    KTPowerSpectrum* SumChannels(const KTPowerSpectrum* powerSpectrum) const;
    //***************
    // Signals
    //***************
    
  private:
    Nymph::KTSignalData fSummedPowerData;
    Nymph::KTSignalData fSummedPSDData;
    
    //***************
    // Slots
    //***************
    
  private:
    Nymph::KTSlotDataOneType< KTPowerSpectrumData > fChPowerSumSlot;
    Nymph::KTSlotDataOneType< KTPowerSpectrumData > fChPSDSumSlot;
  };
  
  inline bool KTChannelAggregator::NullPowerSpectrum(KTPowerSpectrum &spectrum)
  {
    for (unsigned i = 0; i < spectrum.size(); ++i)
    {
      spectrum(i)=0.0;
    }
    return true;
  }
  
  
}

#endif  /* KTCHANNELAGGREGATOR_HH_  */
