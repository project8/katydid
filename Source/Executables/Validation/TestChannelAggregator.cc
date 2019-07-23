/*
 * TestChannelAggregator.cc
 *
 *  Created on: July 8, 2019
 *      Author: Pranava Teja Surukuchi
 *
 *  Generate a sample signal of amplitude 1 at (X,Y) = (1.4 cm,-1.7 cm) and validate
 *  the functionality of KTChannelAggregator and KTAggregatedChannelOptimizer.
 
 *
 *  Usage:  > TestChannelAggregator
 */

#include "KTFrequencySpectrumFFTW.hh"

#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTChannelAggregatedData.hh"

#include "KTChannelAggregator.hh"
#include "KTAggregatedChannelOptimizer.hh"

#include "KTLogger.hh"
#include "KTMath.hh"

#include <cmath>

using namespace std;
using namespace Katydid;

KTLOGGER(vallog, "TestChannelAggregator");

// Find the location of the grid points for a given grid number
void GetGridLocation(int gridNumber, int gridSize, double activeRadius, double &gridLocationX, double &gridLocationY)
{
    int gridNumberX=(int)(gridNumber/gridSize);
    int gridNumberY=gridNumber%gridSize;
    gridLocationX = activeRadius * (((2.0 * gridNumberX + 1.0) / gridSize) - 1);
    gridLocationY = activeRadius * (((2.0 * gridNumberY + 1.0) / gridSize) - 1);
    return;
}

// get shift in the phase based on the channel location and the X and Y positions
double GetPhaseShift(double xPosition, double yPosition, double wavelength, double channelAngle, double activeRadius)
{
    // X position based on the angle of the channel
    double xChannel = activeRadius * cos(channelAngle);
    // X position based on the angle of the channel
    double yChannel = activeRadius * sin(channelAngle);
    // Distance of the input point from the input channel
    double pointDistance = pow(pow(xChannel - xPosition, 2) + pow(yChannel - yPosition, 2), 0.5);
    // Phase of the input signal based on the input point, channel location and the wavelength
    return 2.0 * KTMath::Pi() * pointDistance / wavelength;
}

int main()
{
    const int nFreqBins=1000;// Number of frequency bins to use
    const double minRange = 100;
    const double maxRange = 200;
    // These values are not realistic but are enough nonetheless
    
    const int nComponents = 30;// Number of channels used for testing
    const int nGrid = 35;// Number of points along one side of the grid
    
    const double activeRadius=0.0516;// Grid radius
    const double wavelength= 0.011;// Wavelength
    
    KTChannelAggregator *channelAggregator=new KTChannelAggregator();
    channelAggregator->SetActiveRadius(activeRadius);
    channelAggregator->SetNGrid(nGrid);
    channelAggregator->SetWavelength(wavelength);
    
    KTAggregatedChannelOptimizer *aggregatedChannelOptimizer=new KTAggregatedChannelOptimizer();
    
    // Crete a KTFrequencySpectrumDataFFTW object to fill frequency spectrum
    KTFrequencySpectrumDataFFTW newFreqData;
    newFreqData.SetNComponents(nComponents);
    
    double gridLocationX = 0.014;
    double gridLocationY = -0.017;
    for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
    {
        KTFrequencySpectrumFFTW *fftwSpectrum=new KTFrequencySpectrumFFTW(nFreqBins, minRange, maxRange);
        // Arbitarily assign 0 to the first channel and progresively add 2pi/N for the rest of the channels in increasing order
        double channelAngle = 2 * KTMath::Pi() * iComponent / nComponents;
        double phaseShift = GetPhaseShift(gridLocationX, gridLocationY, wavelength, channelAngle,activeRadius);

        for (int iFreqBin=0; iFreqBin<nFreqBins; iFreqBin++) {
            //Arbitrarily chose the middle bin for the signal frequency
            if(iFreqBin==(int)(nFreqBins/2))
            {
                (*fftwSpectrum)(iFreqBin)[0]= cos(phaseShift);
                (*fftwSpectrum)(iFreqBin)[1]= -sin(phaseShift);
            }
            // Since non-signal frequencies are irrelavant, manually setthing them to 0
            else{
                (*fftwSpectrum)(iFreqBin)[0]= 0.0;
                (*fftwSpectrum)(iFreqBin)[1]= 0.0;
            }
        }
        newFreqData.SetSpectrum(fftwSpectrum, iComponent);
    }
    
    // Actual channel voltage summation part
    channelAggregator->SumChannelVoltageWithPhase(newFreqData);
    // Optimize the summed voltages
    aggregatedChannelOptimizer->FindOptimumSum(newFreqData.Of<KTAggregatedFrequencySpectrumDataFFTW >());
    
    // Get the point and value that correspomnds to the optimal summation of the voltages
    int optimizedGridPoint=(newFreqData.Of<KTAggregatedFrequencySpectrumDataFFTW >()).GetOptimizedGridPoint();
    double optimizedGridValue=(newFreqData.Of<KTAggregatedFrequencySpectrumDataFFTW >()).GetOptimizedGridValue();
    double optimizedGridLocationX;
    double optimizedGridLocationY;
    // Get the physical positon of the reconstructed point
    GetGridLocation(optimizedGridPoint,nGrid,activeRadius ,optimizedGridLocationX,optimizedGridLocationY);

    KTINFO(vallog, "Testing the KTChannelAggregator and KTAggregatedChannelOptimizer processors for "
           << nComponents << " channels and a square grid of size "<< nGrid*nGrid<<"\n"
           "Assume the main lobe at a frequency value midway between the bins\n"
           "Input (X,Y)= ("<<gridLocationX <<","<<gridLocationY <<", Output (X,Y)= ("<<optimizedGridLocationX <<"), "<<optimizedGridLocationY <<")\n"
           "(deltaX, deltaY)= ("<<optimizedGridLocationX-gridLocationX <<","<<optimizedGridLocationY-gridLocationY <<"),precision of grid in (X,Y)= ("<<activeRadius/nGrid <<","<< activeRadius/nGrid <<")\n");
    KTINFO(vallog, "Optimized aggregated volatge = " << optimizedGridValue/nComponents<<" per channel, ideally has to be close to unity\n");
    delete channelAggregator;
    return 0;
    
}




