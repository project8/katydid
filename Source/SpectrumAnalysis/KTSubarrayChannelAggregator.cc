/*
 * KTSubarrayChannelAggregator.cc
 *
 *  Created on: Apr 14, 2020
 *      Author: P. T. Surukuchi
 */

#include "KTSubarrayChannelAggregator.hh"

#include "KTLogger.hh"

namespace Katydid
{
    KTLOGGER(agglog, "KTSubarrayChannelAggregator");

    // Register the processor
    KT_REGISTER_PROCESSOR(KTSubarrayChannelAggregator, "subarray-channel-aggregator");

    KTSubarrayChannelAggregator::KTSubarrayChannelAggregator(const std::string& name) :
        KTChannelAggregator(name),
        fNRings(1)
    {
    }

    KTSubarrayChannelAggregator::~KTSubarrayChannelAggregator()
    {
    }

    bool KTSubarrayChannelAggregator::Configure(const scarab::param_node* node)
    {
        if (node != NULL)
        {
            fNRings = node->get_value< signed int >("n-rings", fNRings);
        }
        KTChannelAggregator::Configure(node);
        return true;
    }

    bool KTSubarrayChannelAggregator::SumChannelVoltageWithPhase(KTFrequencySpectrumDataFFTW& fftwData)
    {
        const KTFrequencySpectrumFFTW* freqSpectrum = fftwData.GetSpectrumFFTW(0);
        int nTimeBins = freqSpectrum->GetNTimeBins();
        // Get the number of frequency bins from the first component of fftwData
        int nFreqBins = freqSpectrum->GetNFrequencyBins();
        int nTotalComponents = fftwData.GetNComponents(); // Get number of components
        if(nTotalComponents%fNRings!=0)
        {
            KTERROR(agglog,"The number of rings has to be an integer multiple of total components");
        }
        int nComponents = nTotalComponents/fNRings;// Get number of components

        GenerateAntiSpiralPhaseShifts(nComponents);
        double maxValue = 0.0;
        double maxGridLocationX = 0.0;
        double maxGridLocationY = 0.0;
        int  maxGridRing = -1;

        int nGrid = GetNGrid();
        double wavelength = GetWavelength();
        double radius = GetActiveRadius();

        // Assume a square grid. i.e, number of points in X= no of points in Y
        KTAggregatedFrequencySpectrumDataFFTW& newAggFreqData = fftwData.Of< KTAggregatedFrequencySpectrumDataFFTW >().SetNComponents(nGrid*nGrid*fNRings);

        // Setting up the active radius of the KTAggregatedFrequencySpectrumDataFFTW object to maintain consistency
        // This doesn't need to be done if there is a way to provide config values to data objects
        newAggFreqData.SetActiveRadius(radius);

        int nTotalGridPoints = 0;
        // Loop over the grid points and rings and fill the values
        for (int iRing = 0; iRing < fNRings; ++iRing)
        {
            for (int iGridX = 0; iGridX < nGrid; ++iGridX)
            {
                double gridLocationX = 0;
                GetGridLocation(iGridX, nGrid, gridLocationX);
                for (int iGridY = 0; iGridY < nGrid; ++iGridY)
                {
                    double gridLocationY = 0;
                    GetGridLocation(iGridY, nGrid, gridLocationY);
                    // Check to make sure that the grid point is within the active detector volume, skip otherwise
                    //        if((pow(gridLocationX,2)+pow(gridLocationY,2))>pow(radius,2)) continue;
                    newAggFreqData.SetGridPoint(nTotalGridPoints, gridLocationX, gridLocationY,iRing);
                    ++nTotalGridPoints;
                }
            }
        }
        int  gridPointsPerRing=nTotalGridPoints/fNRings;
        for (unsigned iRing = 0; iRing < fNRings; ++iRing)
        {
            // Loop over all grid points and find the one that gives the highest value
            for (int iGrid = 0; iGrid < gridPointsPerRing; ++iGrid)
            { // Loop over the grid points
                KTFrequencySpectrumFFTW* newFreqSpectrum = new KTFrequencySpectrumFFTW(nFreqBins, freqSpectrum->GetRangeMin(), freqSpectrum->GetRangeMax());
                // Empty values in the frequency spectrum, not sure if this is needed but there were some issues when this was not done for the power spectrum
                NullFreqSpectrum(*newFreqSpectrum);
                double gridLocationX = 0;
                double gridLocationY = 0;
                double gridLocationZ = 0;
                int ringLocation = 0;
                newAggFreqData.GetGridPoint(iGrid+gridPointsPerRing*iRing, gridLocationX, gridLocationY,gridLocationZ);
                for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
                {
                    // Arbitarily assign 0 to the first channel and progresively add 2pi/N for the rest of the channels in increasing order
                    double channelAngle = 2 * KTMath::Pi() * iComponent / nComponents;
                    double phaseShift = GetPhaseShift(gridLocationX, gridLocationY, wavelength, channelAngle);
                    // Just being redundantly cautious, the phaseShifts are already zerors but checking to make sure anyway
                    if(GetUseAntiSpiralPhaseShifts())
                    {
                        phaseShift-=fAntiSpiralPhaseShifts.at(iComponent);
                        //std::cout<<"phasehift "<<phaseShift<<std::endl;
                    }
                    // Get the frequency spectrum for that specific component
                    freqSpectrum = fftwData.GetSpectrumFFTW(iComponent+iRing*nComponents);
                    double maxVoltage = 0.0;
                    int maxFrequencyBin = 0;
                    //Loop over the frequency bins
                    for (unsigned iFreqBin = 0; iFreqBin < nFreqBins; ++iFreqBin)
                    {
                        double realVal = freqSpectrum->GetReal(iFreqBin);
                        double imagVal = freqSpectrum->GetImag(iFreqBin);
                        ApplyPhaseShift(realVal, imagVal, phaseShift);
                        double summedRealVal = realVal + newFreqSpectrum->GetReal(iFreqBin);
                        double summedImagVal = imagVal + newFreqSpectrum->GetImag(iFreqBin);
                        (*newFreqSpectrum)(iFreqBin)[0] = summedRealVal;
                        (*newFreqSpectrum)(iFreqBin)[1] = summedImagVal;
                    } // End of loop over freq bins
                } // End of loop over all comps
                newFreqSpectrum->SetNTimeBins(nTimeBins);
                newAggFreqData.SetSpectrum(newFreqSpectrum, iGrid+gridPointsPerRing*iRing);

                double maxVoltageFreq = 0.0;
                //Loop over all the freq bins and get the highest value and save to the aggregated frequency data
                for (unsigned iFreqBin = 0; iFreqBin < nFreqBins; ++iFreqBin)
                {
                    if(newFreqSpectrum->GetBinCenter(iFreqBin)<GetSummationMinFreq() || newFreqSpectrum->GetBinCenter(iFreqBin)>GetSummationMaxFreq()) continue;
                    if (newFreqSpectrum->GetAbs(iFreqBin) > maxVoltageFreq)
                    {
                        maxVoltageFreq = newFreqSpectrum->GetAbs(iFreqBin);
                    }
                } // end of freqeuncy bin loops
                newAggFreqData.SetSummedGridVoltage(iGrid+gridPointsPerRing*iRing, maxVoltageFreq);
            } // End of grid loop
        }// End of loop over all rings
        KTDEBUG(agglog,"Channel summation performed over "<< fNRings<<" rings and "<<gridPointsPerRing<<" grid points per ring");
        return true;
    }
}

