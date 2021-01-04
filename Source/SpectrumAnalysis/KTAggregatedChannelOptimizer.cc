/*
 * KTAggregatedChannelOptimizer.hh
 *
 *  Created on: Apr 21, 2019
 *      Author: P. T. Surukuchi
 */

#include "KTAggregatedChannelOptimizer.hh"
#include "KTFSCDCRESUtils.hh"

#include "KTLogger.hh"

namespace Katydid
{
    KTLOGGER(aggoptlog, "KTAggregatedChannelOptimizer");
    
    // Register the processor
    KT_REGISTER_PROCESSOR(KTAggregatedChannelOptimizer, "aggregated-channel-optimizer");
    
    KTAggregatedChannelOptimizer::KTAggregatedChannelOptimizer(const std::string& name) :
            KTProcessor(name),
            fSummedFrequencyData("agg-fft", this),
            fOptimalSumSlot("agg-fft", this, &KTAggregatedChannelOptimizer::FindOptimumSum, &fSummedFrequencyData),
            fApplyOptimumFrequencyShiftsSlot("agg-fft-shift",this,&KTAggregatedChannelOptimizer::ApplyOptimumFrequencyShifts, &fSummedFrequencyData)
    {
    }
    
    KTAggregatedChannelOptimizer::~KTAggregatedChannelOptimizer()
    {
    }
    
    bool KTAggregatedChannelOptimizer::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return false;
        return true;
    }
    
    bool KTAggregatedChannelOptimizer::FindOptimumSum(KTAggregatedFrequencySpectrumDataFFTW& aggData)
    {
        const KTFrequencySpectrumFFTW* freqSpectrum=aggData.GetSpectrumFFTW(0);
        // Get the number of frequency bins from the first component of aggData
        int nFreqBins=freqSpectrum->GetNFrequencyBins();
        int nGridPoints=aggData.GetNComponents(); // Get number of components
        
        int maxGridPoint=0.0;
        double maxVoltage=0.0;
        // Loop over each component and find and save the maximum summed absolute voltages and the corresponding frequencies
        for (unsigned iGridPoint=0; iGridPoint<nGridPoints; ++iGridPoint)
        {
            double gridLocationX,gridLocationY,gridLocationZ;
            freqSpectrum=aggData.GetSpectrumFFTW(iGridPoint);
            aggData.GetGridPoint(iGridPoint,gridLocationX,gridLocationY,gridLocationZ);
            double maxVoltageFreq=aggData.GetSummedGridVoltage(iGridPoint);
            
            if(maxVoltageFreq>maxVoltage)
            {
                maxVoltage=maxVoltageFreq;
                maxGridPoint=iGridPoint;
            }
            
        }// end of grid points loop
        
        aggData.SetOptimizedGridPointValue(maxGridPoint,maxVoltage);
        double x,y,z;
        aggData.GetGridPoint(maxGridPoint,x,y,z);
        KTDEBUG(aggoptlog,"Optimized grid point found at ("<<x<<","<<y<<","<<z<< ") with the maximum voltage of "<<maxVoltage);
        return true;
    }

    
    bool KTAggregatedChannelOptimizer::ApplyOptimumFrequencyShifts(KTFrequencySpectrumDataFFTW& fftwData, KTAggregatedFrequencySpectrumDataFFTW& aggData)
    {
        KTFrequencySpectrumFFTW* freqSpectrum = fftwData.GetSpectrumFFTW(0);
        // PTS: Assumes only a single ring for now.
        int nComponents = fftwData.GetNComponents(); // Get number of components 
        double x,y,z;
        int maxGridPoint = aggData.GetOptimizedGridPoint();
        aggData.GetGridPoint(maxGridPoint,x,y,z);
        KTFSCDCRESUtils fscdCRESUtils;
        std::cout<<fscdCRESUtils.testConstant<<std::endl;
        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            freqSpectrum = fftwData.GetSpectrumFFTW(iComponent);
            double channelAngle = KTMath::TwoPi() * iComponent / nComponents;
            fscdCRESUtils.ApplyFrequencyShifts(*freqSpectrum,x,y,channelAngle,0,200e6);
        }
        return true;
    }

}
