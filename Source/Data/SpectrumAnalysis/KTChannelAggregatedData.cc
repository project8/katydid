/*
 * KTChannelAggregatedData.cc
 *
 *  Created on: Apr 8, 2019
 *      Author: P. T. Surukuchi
 */

#include "KTChannelAggregatedData.hh"

namespace Katydid
{
    /*****************  Beginning of KTAggregatedDataCore ************************/
    
    KTAggregatedDataCore::KTAggregatedDataCore():
    fOptimizedGridPoint(-1),
    fIsGridOptimized(false),
    fIsSquareGrid(true)
    {
    }
    
    KTAggregatedDataCore::~KTAggregatedDataCore()
    {
        fGridPoints.clear();
    }
    
    void KTAggregatedDataCore::SetNGridPoints(unsigned num)
    {
        unsigned oldSize = fGridPoints.size();
        if(oldSize>num)
        {
            fGridPoints.erase(fGridPoints.begin()+num,fGridPoints.begin()+oldSize);
        }
        fGridPoints.resize(num);
    }
    /*****************  End of KTAggregatedDataCore ************************/
    
    /*****************  Beginning of KTAggregatedFrequencySpectrumDataFFTW ************************/
    
    const std::string KTAggregatedFrequencySpectrumDataFFTW::sName("aggregated-frequency-spectrum-fftw");
    
    KTAggregatedFrequencySpectrumDataFFTW::KTAggregatedFrequencySpectrumDataFFTW() : KTAggregatedDataCore(), KTFrequencySpectrumDataFFTWCore(), Nymph::KTExtensibleData<KTAggregatedFrequencySpectrumDataFFTW>()
    {
    }
    
    KTAggregatedFrequencySpectrumDataFFTW::~KTAggregatedFrequencySpectrumDataFFTW()
    {
    }
    
    KTAggregatedFrequencySpectrumDataFFTW& KTAggregatedFrequencySpectrumDataFFTW::SetNComponents(unsigned num)
    {
        unsigned oldSize = fSpectra.size();
        
        // If old size is bigger than num, delete all the extra terms
        for (unsigned iComponent = num; iComponent < oldSize; ++iComponent)
        {
            delete fSpectra[iComponent];
        }
        
        //Resize old size is smaller than old size
        fSpectra.resize(num);
        
        for (unsigned iComponent = oldSize; iComponent < num; ++iComponent)
        {
            fSpectra[iComponent] = NULL;
        }
        SetNGridPoints(num);
        return *this;
    }
    
    /*****************  End of KTAggregatedFrequencySpectrumDataFFTW ************************/
    
    const std::string KTAggregatedPowerSpectrumData::sName("aggregated-frequency-spectrum-fftw");
    
    KTAggregatedPowerSpectrumData::KTAggregatedPowerSpectrumData() : KTAggregatedDataCore(),KTPowerSpectrumDataCore(), Nymph::KTExtensibleData<KTAggregatedPowerSpectrumData>()
    {
    }
    
    KTAggregatedPowerSpectrumData::~KTAggregatedPowerSpectrumData()
    {
    }
    
    KTAggregatedPowerSpectrumData& KTAggregatedPowerSpectrumData::SetNComponents(unsigned num)
    {
        unsigned oldSize = fSpectra.size();
        
        // If old size is bigger than num, delete all the extra terms
        for (unsigned iComponent = num; iComponent < oldSize; ++iComponent)
        {
            delete fSpectra[iComponent];
        }
        
        //Resize old size is smaller than old size
        fSpectra.resize(num);
        
        for (unsigned iComponent = oldSize; iComponent < num; ++iComponent)
        {
            fSpectra[iComponent] = NULL;
        }
        SetNGridPoints(num);
        return *this;
    }
    
} /* namespace Katydid */

