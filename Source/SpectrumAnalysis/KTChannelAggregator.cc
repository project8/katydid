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
	    fUseAntiSpiralPhaseShifts(false),
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
            //std::cout << node;
            fNGrid = node->get_value< signed int >("grid-size", fNGrid);
            //std::cout << fNGrid;
            fActiveRadius = node->get_value< double >("active-radius", fActiveRadius);
            //std::cout << fActiveRadius;
            fWavelength = node->get_value< double >("wavelength", fWavelength);
            //std::cout << fWavelength;
            fUseAntiSpiralPhaseShifts = node->get_value< bool>("use-antispiral-phase-shifts", fUseAntiSpiralPhaseShifts);
            //std::cout << fUseAntiSpiralPhaseShifts;
        }
        return true;
    }

    bool KTChannelAggregator::ApplyPhaseShift(double &realVal, double &imagVal, double phase)
    {
        double tempRealVal = realVal;
        double tempImagVal = imagVal;
        //std::cout << realVal;
        //std::cout << imagVal;
        realVal = tempRealVal * cos(phase) - tempImagVal * sin(phase);
        imagVal = tempRealVal * sin(phase) + tempImagVal * cos(phase);
        //std::cout << realVal;
        //std::cout << imagVal;
        //std::cout << "applying the phase shift\n";
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
        //std::cout << xChannel;
        //std::cout << yChannel;
        //std::cout << pointDistance;
        //std::cout <<"getting the phase shift\n";
        return 2.0 * KTMath::Pi() * pointDistance / wavelength;
    }

    bool KTChannelAggregator::GetGridLocation(int gridNumber, int gridSize, double &gridLocation)
    {
        if (gridNumber >= gridSize) return false;
        gridLocation = fActiveRadius * (((2.0 * gridNumber + 1.0) / gridSize) - 1);
        //std::cout << gridLocation;
        std::cout <<"getting the grid location\n";
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
	    std::pair<int,double> channelPhaseShift=std::make_pair(i,phaseShift); // declare a std::pair object (think python tuple) with the channel index and associated phaseShift.
      //std::cout << channelPhaseShift;
	    fAntiSpiralPhaseShifts.insert(channelPhaseShift); // inserts the pair (channel_ind, phaseShift) into a std::map container object (python dictionary).
                                                        //insert is a map method
	}
      //std::cout << fAntiSpiralPhaseShifts;
      std::cout <<"Generating antispiral shifts.\n";
	    return true;
    }

    bool KTChannelAggregator::SumChannelVoltageWithPhase(KTFrequencySpectrumDataFFTW& fftwData)
    {
        const KTFrequencySpectrumFFTW* freqSpectrum = fftwData.GetSpectrumFFTW(0); //assign the pointer of the 0th or first component of a
                                                                                  //vector of pointers to KTFrequencySpectrumFFTW data to the pointer variable
                                                                                  // freqSpectrum

        int nTimeBins = freqSpectrum->GetNTimeBins(); // get the number of time bins from the first component of the vector
                                                      //fftwData which is a vector of pointers to KTFrequencySpectrumFFTW data

        // Get the number of frequency bins from the first component of fftwData
        int nFreqBins = freqSpectrum->GetNFrequencyBins();
        int nComponents = fftwData.GetNComponents(); // Get number of components which corresponds to the number of channels in the KTFrequencySpectrum object.

	      GenerateAntiSpiralPhaseShifts(nComponents); // Genererate the antispiral shifts for an antenna array with nComponents worth of antennas.
                                              // stores them in the member variable of the KTChannelAggregator class fAntiSpiralPhaseShifts.
                                              // thats why nothing is returned.
        double maxValue = 0.0;
        double maxGridLocationX = 0.0;
        double maxGridLocationY = 0.0;

        // Assume a square grid. i.e, number of points in X= no of points in Y
        // here we're creating a new reference to KTAggregatedFrequencySpectrumDataFFTW and calling it newAggFreqData
        // the line fftwData.Of< KTAggregatedFrequencySpectrumDataFFTW >().SetNComponents(fNGrid * fNGrid); is confusing to me.
        // fftwData is a reference to the data object where the data is stored. I believe that the .Of function is adding KTAggregatedFrequencySpectrumDataFFTW to
        // the data object. It's weird to me that this is done through interacting with what I thought was just the KTFrequencySpectrumDataFFTW object.
        // I suppose inheritance makes this possible. The NComponents of the KTAggregatedFrequencySpectrumDataFFTW are set to the square of the NGrid points.
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
}
