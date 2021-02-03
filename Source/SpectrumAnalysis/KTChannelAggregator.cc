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
            fSummedTimeData("agg-ts", this),
            fPhaseChFrequencySumSlot("fft", this, &KTChannelAggregator::SumChannelVoltageWithPhase, &fSummedFrequencyData),
            fPhaseChTimeSumSlot("ts-fftw", this, &KTChannelAggregator::SumChannelVoltageWithPhase, &fSummedTimeData),
            fActiveRadius(0.0516),
            fNGrid(30),
            fWavelength(0.0115),
            fIsGridDefined(false),
      	    fUseAntiSpiralPhaseShifts(false),
            fGridFreq(0.0),
      	    fAntiSpiralPhaseShifts()
    {
    }

    KTChannelAggregator::~KTChannelAggregator()
    {
    }

    bool KTChannelAggregator::Configure(const scarab::param_node* node)
    {
        if (node != NULL)
        {
            std::cout << "configuring\n";
            fNGrid = node->get_value< signed int >("grid-size", fNGrid);
            fActiveRadius = node->get_value< double >("active-radius", fActiveRadius);
            fWavelength = node->get_value< double >("wavelength", fWavelength);
            fUseAntiSpiralPhaseShifts = node->get_value< bool>("use-antispiral-phase-shifts", fUseAntiSpiralPhaseShifts);
            fGridFreq = node->get_value< double >("grid-rotation-freq",fGridFreq);
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

    double KTChannelAggregator::GetPhaseShift(double xPosition, double yPosition, double wavelength, double channelAngle, double gridFreq, int iTimeBin, double timeBinWidth) const
    {
        // x position based on the angle of the channel
        double xChannel = fActiveRadius * cos(channelAngle);
        // y position based on the angle of the channel
        double yChannel = fActiveRadius * sin(channelAngle);
        // radial position of the grid point
        double rPosition =  pow(pow(xPosition,2)+pow(yPosition,2),0.5);
        // angular position of the grid point, add rotation motion in the same line
        double aPosition = atan2(yPosition,xPosition) + 2 * KTMath::Pi() * gridFreq * iTimeBin * timeBinWidth;
        // calculte the rotated xPosition
        xPosition = rPosition * cos(aPosition);
        // calculate the rotated yPosition
        yPosition = rPosition * sin(aPosition);
        // Distance of the input point from the input channel
        double pointDistance = pow(pow(xChannel - xPosition, 2) + pow(yChannel - yPosition, 2), 0.5);
        // Phase of the input signal based on the input point, channel location and the wavelength
        return 2.0 * KTMath::Pi() * pointDistance / wavelength;
    }

    bool KTChannelAggregator::GetGridLocation(int gridNumber, int gridSize, double &gridLocation)
    {
        if (gridNumber >= gridSize) return false;
        gridLocation = fActiveRadius * (((2.0 * gridNumber + 1.0) / gridSize) - 1);
        //std::cout << gridLocation;
        //std::cout <<"getting the grid location\n";
        return true;
    }

    bool KTChannelAggregator::GenerateAntiSpiralPhaseShifts(int channelCount) // method that calculates the antispiral phase shifts for the array
    {
	for(int i=0;i<channelCount;++i) // iterate over the channels
	{
	    double phaseShift=0.0; // declare local variable phaseShift, and set it to 0.0
	    if(fUseAntiSpiralPhaseShifts) // if we are using the antispiral correction then calculatee the phase shift.
	    {
	        phaseShift=i*2*KTMath::Pi()/channelCount; //calculate the phaseshift, it depends on the channel index.
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
        int nComponents = fftwData.GetNComponents();

	      GenerateAntiSpiralPhaseShifts(nComponents);
        double maxValue = 0.0;
        double maxGridLocationX = 0.0;
        double maxGridLocationY = 0.0;

        // Assumes a square grid.
        KTAggregatedFrequencySpectrumDataFFTW& newAggFreqData = fftwData.Of< KTAggregatedFrequencySpectrumDataFFTW >().SetNComponents(fNGrid * fNGrid);

        // Setting up the active radius of the KTAggregatedFrequencySpectrumDataFFTW object to maintain consistency
        // This doesn't need to be done if there is a way to provide config values to data objects
        newAggFreqData.SetActiveRadius(fActiveRadius);

        int nTotalGridPoints = 0;
        // Loop over the grid points and fill the values
        for (int iGridX = 0; iGridX < fNGrid; ++iGridX)
        {
            double gridLocationX = 0;
            // use GetGridLocation so set the value of gridLocationX
            GetGridLocation(iGridX, fNGrid, gridLocationX);
            for (int iGridY = 0; iGridY < fNGrid; ++iGridY)
            {
                double gridLocationY = 0;
                // use GetGridLocation so set the value of gridLocationY
                GetGridLocation(iGridY, fNGrid, gridLocationY);
                // Check to make sure that the grid point is within the active detector volume, skip otherwise
                //        if((pow(gridLocationX,2)+pow(gridLocationY,2))>pow(fActiveRadius,2)) continue;
                // set the nth gridpoint to the values calculated above.
                newAggFreqData.SetGridPoint(nTotalGridPoints, gridLocationX, gridLocationY);
                ++nTotalGridPoints;
            }
        }

        // Loop over all grid points and find the one that gives the highest value
        for (int iGrid = 0; iGrid < nTotalGridPoints; ++iGrid)
        { // Loop over the grid points
            double gridLocationX = 0;
            double gridLocationY = 0;
            newAggFreqData.GetGridPoint(iGrid, gridLocationX, gridLocationY);
            KTFrequencySpectrumFFTW* newFreqSpectrum = new KTFrequencySpectrumFFTW(nFreqBins, freqSpectrum->GetRangeMin(), freqSpectrum->GetRangeMax());
            // Empty values in the frequency spectrum, not sure if this is needed but there were some issues when this was not done for the power spectrum
            NullFreqSpectrum(*newFreqSpectrum);
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
                freqSpectrum = fftwData.GetSpectrumFFTW(iComponent);
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
            newAggFreqData.SetSpectrum(newFreqSpectrum, iGrid);

            double maxVoltageFreq = 0.0;
            //Loop over all the freq bins and get the highest value and save to the aggregated frequency data
            for (unsigned iFreqBin = 0; iFreqBin < nFreqBins; ++iFreqBin)
            {
                if (newFreqSpectrum->GetAbs(iFreqBin) > maxVoltageFreq)
                {
                    maxVoltageFreq = newFreqSpectrum->GetAbs(iFreqBin);
                }
            } // end of freqeuncy bin loops
            newAggFreqData.SetSummedGridVoltage(iGrid, maxVoltageFreq);
        } // End of grid
        return true;
    }

    bool KTChannelAggregator::SumChannelVoltageWithPhase(KTTimeSeriesData& timeData)
    {
        const KTTimeSeriesFFTW* timeSeries = dynamic_cast< const KTTimeSeriesFFTW* >(timeData.GetTimeSeries(0));

        int nTimeBins = timeSeries->GetNTimeBins();

        double timeBinWidth = timeSeries->GetTimeBinWidth();
        //std::cout << timeBinWidth<< std::endl;

        int nComponents = timeData.GetNComponents();

	      GenerateAntiSpiralPhaseShifts(nComponents);

        double maxValue = 0.0;
        double maxGridLocationX = 0.0;
        double maxGridLocationY = 0.0;

        // Assumes a square grid.
        KTAggregatedTimeSeriesData& newAggTimeData = timeData.Of< KTAggregatedTimeSeriesData >().SetNComponents(fNGrid * fNGrid);

        // Setting up the active radius of the KTAggregatedFrequencySpectrumDataFFTW object to maintain consistency
        // This doesn't need to be done if there is a way to provide config values to data objects
        newAggTimeData.SetActiveRadius(fActiveRadius);

        int nTotalGridPoints = 0;
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
                // set the nth gridpoint to the values calculated above.
                newAggTimeData.SetGridPoint(nTotalGridPoints, gridLocationX, gridLocationY);
                ++nTotalGridPoints;
            }
        }

        // Loop over all grid points fill grid data with Vrms, for time domain
        for (int iGrid = 0; iGrid < nTotalGridPoints; ++iGrid)
        { // Loop over the grid points
            double gridLocationX = 0;
            double gridLocationY = 0;
            newAggTimeData.GetGridPoint(iGrid, gridLocationX, gridLocationY);
            KTTimeSeriesFFTW* newTimeSeries = new KTTimeSeriesFFTW(nTimeBins);

            for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
            {
                // Arbitarily assign 0 to the first channel and progresively add 2pi/N for the rest of the channels in increasing order
                double channelAngle = 2 * KTMath::Pi() * iComponent / nComponents;
                // Get the time series for that specific component. Treat as FFTW series.
                timeSeries = dynamic_cast< const KTTimeSeriesFFTW* >(timeData.GetTimeSeries(iComponent));
                double maxVoltage = 0.0;
                int maxFrequencyBin = 0;
                //Loop over the time bins
                for (unsigned iTimeBin = 0; iTimeBin < nTimeBins; ++iTimeBin)
                {
                    double phaseShift = GetPhaseShift(gridLocationX, gridLocationY, fWavelength, channelAngle, fGridFreq, iTimeBin, timeBinWidth);
                    if(fUseAntiSpiralPhaseShifts){
                      phaseShift-=fAntiSpiralPhaseShifts.at(iComponent);
                    }
                    double realVal = timeSeries->GetReal(iTimeBin);
                    double imagVal = timeSeries->GetImag(iTimeBin);
                    ApplyPhaseShift(realVal, imagVal, phaseShift);
                    double summedRealVal = realVal + newTimeSeries->GetReal(iTimeBin);
                    double summedImagVal = imagVal + newTimeSeries->GetImag(iTimeBin);
                    (*newTimeSeries)(iTimeBin)[0] = summedRealVal;
                    (*newTimeSeries)(iTimeBin)[1] = summedImagVal;
                } // End of loop over time bins
            } // End of loop over all comps
            newAggTimeData.SetTimeSeries(newTimeSeries, iGrid);

            double sumAbsVoltageTime = 0.0;
            //loop over all time bins in the series to calculate Vrms
            for (unsigned iTimeBin = 0; iTimeBin < nTimeBins; ++iTimeBin)
            {
                sumAbsVoltageTime+=newTimeSeries->GetAbs(iTimeBin);

            } // end of time bin loops
            // set grid value to the mean of the sum of the abs voltages.
            newAggTimeData.SetSummedGridVoltage(iGrid,sumAbsVoltageTime/nTimeBins);

        } // End of grid
        return true;
    }
}
