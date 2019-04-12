/**
 @file KTAggregatedFrequencyData
 @brief Contains KTAggregatedFrequencyData
 @details The summed spectra from all the channels in the azimuthal direction.
 Also includes of the reconstructed radial position of the electron.
 Currently voltage summation in the frequency domain is performed.
 Can be extended to the power summation and in the time domain.
 Warning: It is the user's responsibility to make sure that the spectra
 corresponds to the right grid point.
 @author: P. T. Surukuchi
 @date: Apr 8, 2019
 */

#ifndef KTCHANNELAGGREGATEDDATA_HH_
#define KTCHANNELAGGREGATEDDATA_HH_

#include "KTData.hh"

#include "KTFrequencySpectrumFFTW.hh"
#include "KTFrequencySpectrumDataFFTW.hh"

#include <vector>

namespace Katydid
{
  
  class KTAggregatedFrequencySpectrumDataFFTW :public KTFrequencySpectrumDataFFTWCore,public Nymph::KTExtensibleData< KTAggregatedFrequencySpectrumDataFFTW >
  {
  public:
    KTAggregatedFrequencySpectrumDataFFTW();
    virtual ~KTAggregatedFrequencySpectrumDataFFTW();
    
    virtual KTAggregatedFrequencySpectrumDataFFTW& SetNComponents(unsigned);
    
    /*
     Set the X,Y pair corresponding to the component.
     */
    virtual void SetGridPoint(int,double,double);
    
    /*
     Get the X,Y pair corresponding to the component number.
     */
    virtual void GetGridPoint(int,double &, double &) const;
    
  public:
    static const std::string sName;
    
  protected:
    std::vector<std::pair<double,double>> fGridPoints;
  };
  
  inline void KTAggregatedFrequencySpectrumDataFFTW::SetGridPoint(int component,double gridValueX,double gridValueY)
  {
    fGridPoints[component]=std::make_pair(gridValueX,gridValueY);
    return;
  }
  
  inline void KTAggregatedFrequencySpectrumDataFFTW::GetGridPoint(int component, double &gridLocationX,double &gridLocationY) const
  {
    gridLocationX=fGridPoints[component].first;
    gridLocationY=fGridPoints[component].second;
    return;
  }
} /* namespace Katydid */

#endif /* KTCHANNELAGGREGATEDDATA_HH_ */
