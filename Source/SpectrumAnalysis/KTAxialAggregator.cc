/*
 * KTAxialAggregator.cc
 *
 *  Created on: Jan 25, 2019
 *      Author: P. T. Surukuchi
 */

#include "KTAxialAggregator.hh"

#include "KTLogger.hh"

namespace Katydid
{
    KTLOGGER(agglog, "KTAxialAggregator");

    // Register the processor
    KT_REGISTER_PROCESSOR(KTAxialAggregator, "axial-aggregator");

    KTAxialAggregator::KTAxialAggregator(const std::string& name) :
        KTProcessor(name),
        fAxialSummedFrequencyData("axial-agg-fft", this),
        fAxialFrequencySumSlot("fft", this, &KTAxialAggregator::SumChannelVoltage, &fAxialSummedFrequencyData),
        fNRings(1)
    {
    }

    KTAxialAggregator::~KTAxialAggregator()
    {
    }

    bool KTAxialAggregator::Configure(const scarab::param_node* node)
    {
        if (node != NULL)
        {
            fNRings= node->get_value< signed int >("n-rings", fNRings);
        }
        return true;
    }

    bool KTAxialAggregator::SumChannelVoltage(KTFrequencySpectrumDataFFTW& fftwData)
    {
        const KTFrequencySpectrumFFTW* freqSpectrum = fftwData.GetSpectrumFFTW(0);
        int nTimeBins = freqSpectrum->GetNTimeBins();
        // Get the number of frequency bins from the first component of fftwData
        int nFreqBins = freqSpectrum->GetNFrequencyBins();
        int nTotalComponents = fftwData.GetNComponents(); // Get number of components
                std::cout<<"nTotalComponents "<<fftwData.GetNComponents()<<std::endl;
        if(nTotalComponents%fNRings!=0)
        {
            KTERROR(agglog,"The number of rings has to be an integer multiple of total components");
        }
        int nComponents = nTotalComponents/fNRings;// Get number of components

        KTFrequencySpectrumDataFFTW& newAxialSummedFreqData = fftwData.Of< KTFrequencySpectrumDataFFTW>().SetNComponents(nComponents);

        for (int iComponent = 0; iComponent< nComponents; ++iComponent)
        {
                KTFrequencySpectrumFFTW* newFreqSpectrum = new KTFrequencySpectrumFFTW(nFreqBins, freqSpectrum->GetRangeMin(), freqSpectrum->GetRangeMax());
            for (int iRing = 0; iRing < fNRings; ++iRing)
            {
                std::cout<<iComponent+nComponents*iRing<<std::endl;
                std::cout<<fftwData.GetNComponents()<<std::endl;
                (*newFreqSpectrum)+=*fftwData.GetSpectrumFFTW(iComponent+nComponents*iRing);
            }// End of loop over all rings
            newAxialSummedFreqData.SetSpectrum(newFreqSpectrum,iComponent);
        }
        KTDEBUG(agglog,"Axial channel summation performed over "<< fNRings<<" rings");
        return true;
    }
}

