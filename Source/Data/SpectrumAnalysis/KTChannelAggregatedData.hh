/*
 * KTChannelAggregatedData.hh
 *
 *  Created on: Jan 28, 2019
 *      Author: P. T. Surukuchi
 *  Copied from KTPowerSpectrumData.hh. Currently a placeholder data class and needs revision to adjust to suit the needs for channel summed data.
 */

#ifndef KTCHANNELAGGREGATEDDATA_HH_
#define KTCHANNELAGGREGATEDDATA_HH_

#include "KTData.hh"

#include "KTPowerSpectrum.hh"

#include <vector>

namespace Katydid
{
  class KTChannelAggregatedDataCore
  {
  public:
    typedef KTPowerSpectrum spectrum_type;
    
  public:
    KTChannelAggregatedDataCore(); // Constructor
    virtual ~KTChannelAggregatedDataCore(); // Destructor
    
    virtual unsigned GetNComponents() const;
    
    const KTPowerSpectrum* GetSpectrum(unsigned component = 0) const;
    KTPowerSpectrum* GetSpectrum(unsigned component = 0);
    
//    const KTFrequencyDomainArray* GetArray(unsigned component = 0) const;
//    KTFrequencyDomainArray* GetArray(unsigned component = 0);
    
    void SetSpectrum(KTPowerSpectrum* spectrum, unsigned component = 0);
    
    virtual KTChannelAggregatedDataCore& SetNComponents(unsigned channels) = 0;
    
  protected:
    std::vector< KTPowerSpectrum* > fSpectra;
  };
  
  
  class KTChannelAggregatedData : public KTChannelAggregatedDataCore, public Nymph::KTExtensibleData< KTChannelAggregatedData >
  {
  public:
    KTChannelAggregatedData();
    virtual ~KTChannelAggregatedData();
    
    KTChannelAggregatedData& SetNComponents(unsigned channels);
    
  public:
    static const std::string sName;
    
  };
  
  inline const KTPowerSpectrum* KTChannelAggregatedDataCore::GetSpectrum(unsigned component) const
  {
    return fSpectra[component];
  }
  
  inline KTPowerSpectrum* KTChannelAggregatedDataCore::GetSpectrum(unsigned component)
  {
    return fSpectra[component];
  }
  
// Needs implementation
//  inline const KTFrequencyDomainArray* KTChannelAggregatedDataCore::GetArray(unsigned component) const
//  {
//    return fSpectra[component];
//  }
//
//  inline KTFrequencyDomainArray* KTChannelAggregatedDataCore::GetArray(unsigned component)
//  {
//    return fSpectra[component];
//  }
  
  inline unsigned KTChannelAggregatedDataCore::GetNComponents() const
  {
    return unsigned(fSpectra.size());
  }
  
  inline void KTChannelAggregatedDataCore::SetSpectrum(KTPowerSpectrum* spectrum, unsigned component)
  {
    if (component >= fSpectra.size()) SetNComponents(component+1);
    else delete fSpectra[component];
    fSpectra[component] = spectrum;
    return;
  }
  
} /* namespace Katydid */

#endif /* KTCHANNELAGGREGATEDDATA_HH_ */
