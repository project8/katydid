/*
 * KTChannelAggregatedData.cc
 *
 *  Created on: Apr 8, 2019
 *      Author: P. T. Surukuchi
 */

#include "KTChannelAggregatedData.hh"

namespace Katydid
{
  
  const std::string KTAggregatedFrequencySpectrumDataFFTW::sName("aggregated-frequency-spectrum-fftw");
  
  KTAggregatedFrequencySpectrumDataFFTW::KTAggregatedFrequencySpectrumDataFFTW() :
  KTFrequencySpectrumDataFFTWCore(),
  KTExtensibleData<KTAggregatedFrequencySpectrumDataFFTW>()
  {
  }
  
  KTAggregatedFrequencySpectrumDataFFTW::~KTAggregatedFrequencySpectrumDataFFTW()
  {
    fGridPoints.clear();
  }
  
  KTAggregatedFrequencySpectrumDataFFTW& KTAggregatedFrequencySpectrumDataFFTW::SetNComponents(unsigned num)
  {
    unsigned oldSize = fSpectra.size();
    
    // If old size is bigger than num, delete all the extra terms
    for (unsigned iComponent = num; iComponent < oldSize; ++iComponent)
    {
      delete fSpectra[iComponent];
    }
    if(oldSize>num)fGridPoints.erase(fGridPoints.begin()+num,fGridPoints.begin()+oldSize);
    
    //Resize old size is smaller than old size
    fSpectra.resize(num);
    fGridPoints.resize(num);
    for (unsigned iComponent = oldSize; iComponent < num; ++iComponent)
    {
      fSpectra[iComponent] = NULL;
    }
    return *this;
  }
  
} /* namespace Katydid */

