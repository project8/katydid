/*
 * KTChannelAggregator.cc
 *
 *  Created on: Jan 25, 2019
 *      Author: P. T. Surukuchi
 */

#include "KTChannelAggregator.hh"

#include "KTLogger.hh"

namespace Katydid
{
    KTLOGGER(agglog, "KTChannelAggregator");

    // Register the processor
    KT_REGISTER_PROCESSOR(KTChannelAggregator, "channel-aggregator");

    KTChannelAggregator::KTChannelAggregator(const std::string& name) :
        KTProcessor(name),
        fSummedFrequencyData("agg-fft", this),
        fPhaseChFrequencySumSlot("fft", this, &KTChannelAggregator::SumChannelVoltageWithPhase, &fSummedFrequencyData),
        fActiveRadius(0.0516),
        fNGrid(30),
        fWavelength(0.0115),
        fIsGridDefined(false),
        fSummationMinFreq(0e6),
        fSummationMaxFreq(200e6),
        fUseAntiSpiralPhaseShifts(false),
        fAntiSpiralPhaseShifts(),
        fNRings(1)
    {
    }

    KTChannelAggregator::~KTChannelAggregator()
    {
    }

    bool KTChannelAggregator::Configure(const scarab::param_node* node)
    {
        if (node != NULL)
        {
            fNGrid = node->get_value< signed int >("grid-size", fNGrid);
            fActiveRadius = node->get_value< double >("active-radius", fActiveRadius);
            fWavelength = node->get_value< double >("wavelength", fWavelength);
            fSummationMinFreq= node->get_value< double >("min-freq", fSummationMinFreq);
            fSummationMaxFreq= node->get_value< double >("max-freq", fSummationMaxFreq);
            fNRings = node->get_value< signed int >("n-rings", fNRings);
            fUseAntiSpiralPhaseShifts = node->get_value< bool>("use-antispiral-phase-shifts", fUseAntiSpiralPhaseShifts);
        }
        return true;
    }

    bool KTChannelAggregator::ApplyPhaseShift(double &realVal, double &imagVal, double phase)
    {
        double tempRealVal = realVal;
        double tempImagVal = imagVal;
        realVal = tempRealVal * cos(phase) - tempImagVal * sin(phase);
        imagVal = tempRealVal * sin(phase) + tempImagVal * cos(phase);
        return true;
    }

    double KTChannelAggregator::GetPhaseShift(double xPosition, double yPosition, double wavelength, double channelAngle) const
    {
        // X position based on the angle of the channel
        double xChannel = fActiveRadius * cos(channelAngle);
        // X position based on the angle of the channel
        double yChannel = fActiveRadius * sin(channelAngle);
        // Distance of the input point from the input channel
        double pointDistance = pow(pow(xChannel - xPosition, 2) + pow(yChannel - yPosition, 2), 0.5);
        // Phase of the input signal based on the input point, channel location and the wavelength
        return 2.0 * KTMath::Pi() * pointDistance / wavelength;
    }

    bool KTChannelAggregator::GetGridLocation(int gridNumber, int gridSize, double &gridLocation)
    {
        if (gridNumber >= gridSize) return false;
        gridLocation = fActiveRadius * (((2.0 * gridNumber + 1.0) / gridSize) - 1);
        return true;
    }

    bool KTChannelAggregator::GenerateAntiSpiralPhaseShifts(int channelCount)
    {
        for(int i=0;i<channelCount;++i)
        {
            double phaseShift=0.0;
            if(fUseAntiSpiralPhaseShifts) 
            {
                phaseShift=i*2*KTMath::Pi()/channelCount;
            }
            std::pair<int,double> channelPhaseShift=std::make_pair(i,phaseShift);
            fAntiSpiralPhaseShifts.insert(channelPhaseShift);
        }
        return true;
    }

    bool KTChannelAggregator::SumChannelVoltageWithPhase(KTFrequencySpectrumDataFFTW& fftwData)
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

        KTAggregatedFrequencySpectrumDataFFTW& newAggFreqData = fftwData.Of< KTAggregatedFrequencySpectrumDataFFTW >().SetNComponents(fNGrid * fNGrid);

        // Setting up the active radius of the KTAggregatedFrequencySpectrumDataFFTW object to maintain consistency
        // This doesn't need to be done if there is a way to provide config values to data objects
        newAggFreqData.SetActiveRadius(fActiveRadius);
        // Set the number of rings present
        newAggFreqData.SetNAxialPositions(1);

        int nTotalGridPoints = 0;
        // Loop over the grid points and rings and fill the values
        for (int iRing = 0; iRing < fNRings; ++iRing)
        {
            // Loop over the grid points and fill the values
            for (int iGridX = 0; iGridX < fNGrid; ++iGridX)
            {
                double gridLocationX = 0;
                GetGridLocation(iGridX, fNGrid, gridLocationX);
                for (int iGridY = 0; iGridY < fNGrid; ++iGridY)
                {
                    double gridLocationY = 0;
                    GetGridLocation(iGridY, fNGrid, gridLocationY);
                    // Check to make sure that the grid point is within the active detector volume, skip otherwise
                    //        if((pow(gridLocationX,2)+pow(gridLocationY,2))>pow(fActiveRadius,2)) continue;
                    newAggFreqData.SetGridPoint(nTotalGridPoints, gridLocationX, gridLocationY,iRing);
                    ++nTotalGridPoints;
                }
            }
        }
        int  gridPointsPerRing=nTotalGridPoints/fNRings;
        for (unsigned iRing = 0; iRing < fNRings; ++iRing)
        {
            // Loop over all grid points and find the one that gives the highest value
            for (int iGrid = 0; iGrid < nTotalGridPoints; ++iGrid)
            { // Loop over the grid points
                KTFrequencySpectrumFFTW* newFreqSpectrum = new KTFrequencySpectrumFFTW(nFreqBins, freqSpectrum->GetRangeMin(), freqSpectrum->GetRangeMax());
                // Empty values in the frequency spectrum, not sure if this is needed but there were some issues when this was not done for the power spectrum
                NullFreqSpectrum(*newFreqSpectrum);
                double gridLocationX = 0;
                double gridLocationY = 0;
                double gridLocationZ = 0;
                newAggFreqData.GetGridPoint(iGrid+gridPointsPerRing*iRing, gridLocationX, gridLocationY,gridLocationZ);
                for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
                {
                    // Arbitarily assign 0 to the first channel and progresively add 2pi/N for the rest of the channels in increasing order
                    double channelAngle = 2 * KTMath::Pi() * iComponent / nComponents;
                    double phaseShift = GetPhaseShift(gridLocationX, gridLocationY, fWavelength, channelAngle);
                    // Just being redundantly cautious, the phaseShifts are already zerors but checking to make sure anyway
                    if(fUseAntiSpiralPhaseShifts)
                    {
                        phaseShift-=fAntiSpiralPhaseShifts.at(iComponent);
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
                    if(newFreqSpectrum->GetBinCenter(iFreqBin)<fSummationMinFreq || newFreqSpectrum->GetBinCenter(iFreqBin)>fSummationMaxFreq) continue;
                    if (newFreqSpectrum->GetAbs(iFreqBin) > maxVoltageFreq)
                    {
                        maxVoltageFreq = newFreqSpectrum->GetAbs(iFreqBin);
                    }
                } // end of freqeuncy bin loops
                newAggFreqData.SetSummedGridVoltage(iGrid+gridPointsPerRing*iRing, maxVoltageFreq);
            } // End of grid
        }// End of loop over all rings
        KTDEBUG(agglog,"Channel summation performed over "<< fNRings<<" rings and "<<gridPointsPerRing<<" grid points per ring");
        return true;
    }
}

