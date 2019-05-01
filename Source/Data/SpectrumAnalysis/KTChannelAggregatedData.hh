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

#include "KTGrid.hh"

#include <vector>

namespace Katydid
{
  class KTAggregatedFrequencySpectrumDataFFTW :public KTFrequencySpectrumDataFFTWCore,public KTGrid,public Nymph::KTExtensibleData< KTAggregatedFrequencySpectrumDataFFTW >
  {
  public:
    KTAggregatedFrequencySpectrumDataFFTW();
    
    virtual ~KTAggregatedFrequencySpectrumDataFFTW();
    
    static const std::string sName;
    
    virtual KTAggregatedFrequencySpectrumDataFFTW& SetNComponents(unsigned);
  };
}/* namespace Katydid */

#endif /* KTCHANNELAGGREGATEDDATA_HH_ */
