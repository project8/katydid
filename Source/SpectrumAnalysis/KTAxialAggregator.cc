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
    KT_REGISTER_PROCESSOR(KTAxialAggregator, "axial-channel-aggregator");

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
            fNRings= node->get_value< unsigned >("n-rings", fNRings);
        }
        return true;
    }

    bool KTAxialAggregator::SumChannelVoltage(KTFrequencySpectrumDataFFTW& fftwData)
    {
        const KTFrequencySpectrumFFTW* freqSpectrum = fftwData.GetSpectrumFFTW(0);
        unsigned nTimeBins = freqSpectrum->GetNTimeBins();
        // Get the number of frequency bins from the first component of fftwData
        unsigned nFreqBins = freqSpectrum->GetNFrequencyBins();
        unsigned nTotalComponents = fftwData.GetNComponents(); // Get number of components
        if(nTotalComponents%fNRings!=0)
        {
            KTERROR(agglog,"The number of rings has to be an integer multiple of total components");
        }
        unsigned nComponents = nTotalComponents/fNRings;// Get number of components

        KTAxialAggregatedFrequencySpectrumDataFFTW& newAxialSummedFreqData = fftwData.Of< KTAxialAggregatedFrequencySpectrumDataFFTW>().SetNComponents(nComponents);

        for (unsigned iComponent = 0; iComponent< nComponents; ++iComponent)
        {
            KTFrequencySpectrumFFTW* newFreqSpectrum = new KTFrequencySpectrumFFTW(nFreqBins, freqSpectrum->GetRangeMin(), freqSpectrum->GetRangeMax());
            for (unsigned iRing = 0; iRing < fNRings; ++iRing)
            {
                KTFrequencySpectrumFFTW* freqSpectrum = fftwData.GetSpectrumFFTW(iComponent+iRing*nComponents);
                for (unsigned iFreqBin = 0; iFreqBin < nFreqBins; ++iFreqBin)
                {
                    double realVal = freqSpectrum->GetReal(iFreqBin);
                    double imagVal = freqSpectrum->GetImag(iFreqBin);
                    double summedRealVal = realVal + newFreqSpectrum->GetReal(iFreqBin);
                    double summedImagVal = imagVal + newFreqSpectrum->GetImag(iFreqBin);
                    (*newFreqSpectrum)(iFreqBin)[0] = summedRealVal;
                    (*newFreqSpectrum)(iFreqBin)[1] = summedImagVal;
                // This doesn't seem to work
                //(*newFreqSpectrum)+=*fftwData.GetSpectrumFFTW(iComponent+nComponents*iRing);
                }
            }// End of loop over all rings
            newAxialSummedFreqData.SetSpectrum(newFreqSpectrum, iComponent);
        }
        KTDEBUG(agglog,"Axial channel summation performed over "<< fNRings<<" rings");
        return true;
    }
}

