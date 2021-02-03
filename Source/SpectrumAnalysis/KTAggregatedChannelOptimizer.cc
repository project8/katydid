/*
 * KTAggregatedChannelOptimizer.hh
 *
 *  Created on: Apr 21, 2019
 *      Author: P. T. Surukuchi
 */

#include "KTAggregatedChannelOptimizer.hh"

#include "KTLogger.hh"

namespace Katydid
{
    KTLOGGER(aggoptlog, "KTAggregatedChannelOptimizer");

    // Register the processor
    KT_REGISTER_PROCESSOR(KTAggregatedChannelOptimizer, "aggregated-channel-optimizer");

    KTAggregatedChannelOptimizer::KTAggregatedChannelOptimizer(const std::string& name) :
            KTProcessor(name),
            fSummedFrequencyData("agg-fft", this),
            fSummedTimeData("agg-ts", this),
            fOptimalFreqSumSlot("agg-fft", this, &KTAggregatedChannelOptimizer::FindOptimumSum, &fSummedFrequencyData),
            fOptimalTimeSumSlot("agg-ts", this, &KTAggregatedChannelOptimizer::FindOptimumSum, &fSummedFrequencyData)
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
            double gridLocationX;
            double gridLocationY;
            freqSpectrum=aggData.GetSpectrumFFTW(iGridPoint);
            aggData.GetGridPoint(iGridPoint,gridLocationX,gridLocationY);
            double maxVoltageFreq=aggData.GetSummedGridVoltage(iGridPoint);

            if(maxVoltageFreq>maxVoltage)
            {
                maxVoltage=maxVoltageFreq;
                maxGridPoint=iGridPoint;
            }

        }// end of grid points loop

        aggData.SetOptimizedGridPointValue(maxGridPoint,maxVoltage);
        return true;
    }

    bool KTAggregatedChannelOptimizer::FindOptimumSum(KTAggregatedTimeSeriesData& aggData)
    {
        const KTTimeSeriesFFTW* timeSeries=dynamic_cast< const KTTimeSeriesFFTW * >(aggData.GetTimeSeries(0));
        // Get the number of frequency bins from the first component of aggData
        int nTimeBins=timeSeries->GetNTimeBins();
        int nGridPoints=aggData.GetNComponents(); // Get number of components

        int maxGridPoint=0.0;
        double maxVoltage=0.0;
        // Loop over each component and find and save the maximum summed absolute voltages and the corresponding frequencies
        for (unsigned iGridPoint=0; iGridPoint<nGridPoints; ++iGridPoint)
        {
            double gridLocationX;
            double gridLocationY;
            //timeSeries = dynamic_cast< const KTTimeSeriesFFTW * >(aggData.GetTimeSeries(iGridPoint));
            aggData.GetGridPoint(iGridPoint,gridLocationX,gridLocationY);
            double maxVoltageTime=aggData.GetSummedGridVoltage(iGridPoint);

            if(maxVoltageTime>maxVoltage)
            {
                maxVoltage=maxVoltageTime;
                maxGridPoint=iGridPoint;
            }

        }// end of grid points loop

        aggData.SetOptimizedGridPointValue(maxGridPoint,maxVoltage);
        return true;
    }
}
