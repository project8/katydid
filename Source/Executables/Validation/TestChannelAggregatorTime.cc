/*
 * TestChannelAggregator.cc
 *
 *  Created on: Feb 4, 2021
 *      Author: Andrew Ziegler and Pranava Teja Surukuchi
 *
 *  Generate a sample signal of amplitude 1 at (X,Y) = (1.5 cm,-1.5 cm) and validate
 *  the functionality of KTChannelAggregator and KTAggregatedChannelOptimizer.

 *
 *  Usage:  > TestChannelAggregator
 */

#include "KTFrequencySpectrumFFTW.hh"
#include "KTTimeSeriesFFTW.hh"

#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTTimeSeriesData.hh"
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
    const int nTimeBins=4200;// Number of time domain samples
    const double minRange = 0;
    const double maxRange = 42e-6;
    const double testSignalFrequency = 42e6;
    const double gridFreq = 3e3;
    // These values are not realistic but are enough nonetheless

    const int nComponents = 42;// Number of channels used for testing
    const int nGrid = 11;// Number of points along one side of the grid

    const double activeRadius=0.0516;// Grid radius
    const double wavelength= 0.011;// Wavelength

    KTChannelAggregator *channelAggregator=new KTChannelAggregator();
    channelAggregator->SetActiveRadius(activeRadius);
    channelAggregator->SetNGrid(nGrid);
    channelAggregator->SetWavelength(wavelength);
    channelAggregator->SetGridFreq(gridFreq);

    KTAggregatedChannelOptimizer *aggregatedChannelOptimizer=new KTAggregatedChannelOptimizer();

    // Crete a KTTimeSeriesData object to fill frequency spectrum
    KTTimeSeriesData newTimeData;
    newTimeData.SetNComponents(nComponents);

    double gridLocationX = 0.015;
    double gridLocationY = -0.015;
    double gridLocationR = pow(pow(gridLocationX,2)+pow(gridLocationY,2),0.5);
    double gridLocationT = atan2(gridLocationY,gridLocationX);
    double rotationAngle;
    double rotationPhaseShift;
    for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
    {
        KTTimeSeriesFFTW *timeSeries=new KTTimeSeriesFFTW(nTimeBins, minRange, maxRange);
        // Arbitarily assign 0 to the first channel and progresively add 2pi/N for the rest of the channels in increasing order
        double channelAngle = 2 * KTMath::Pi() * iComponent / nComponents;
        double phaseShift = GetPhaseShift(gridLocationX, gridLocationY, wavelength, channelAngle,activeRadius);
        for (int iTimeBin=0; iTimeBin<nTimeBins; iTimeBin++)
        {
            rotationAngle = 2 * KTMath::Pi() * iTimeBin * timeSeries->GetTimeBinWidth() * gridFreq;
            rotationPhaseShift = GetPhaseShift(gridLocationR * cos(gridLocationT + rotationAngle), gridLocationR * sin(gridLocationT + rotationAngle), wavelength, channelAngle, activeRadius);
            timeSeries->SetValue(iTimeBin,cos( 2 * KTMath::Pi() * testSignalFrequency * iTimeBin * timeSeries->GetTimeBinWidth() + phaseShift + rotationPhaseShift));
        }
        newTimeData.SetTimeSeries(timeSeries, iComponent);
    }

    // Actual channel voltage summation part
    channelAggregator->SumChannelVoltageWithPhase(newTimeData);
    // Optimize the summed voltages
    aggregatedChannelOptimizer->FindOptimumSum(newTimeData.Of< KTAggregatedTimeSeriesData >());

    // Get the point and value that correspomnds to the optimal summation of the voltages
    int optimizedGridPoint=(newTimeData.Of< KTAggregatedTimeSeriesData >()).GetOptimizedGridPoint();
    double optimizedGridValue=(newTimeData.Of< KTAggregatedTimeSeriesData >()).GetOptimizedGridValue();
    double optimizedGridLocationX;
    double optimizedGridLocationY;
    // Get the physical positon of the reconstructed point
    GetGridLocation(optimizedGridPoint,nGrid,activeRadius ,optimizedGridLocationX,optimizedGridLocationY);

    KTINFO(vallog, "Testing the KTChannelAggregator and KTAggregatedChannelOptimizer processors with time data for "
           << nComponents << " channels and a square grid of size "<< nGrid*nGrid<<"\n"
           "Assume the signal is a real sinusoid\n"
           "Input (X,Y)= ("<<gridLocationX <<","<<gridLocationY <<", Output (X,Y)= ("<<optimizedGridLocationX <<"), "<<optimizedGridLocationY <<")\n"
           "(deltaX, deltaY)= ("<<optimizedGridLocationX-gridLocationX <<","<<optimizedGridLocationY-gridLocationY <<"),precision of grid in (X,Y)= ("<<activeRadius/nGrid <<","<< activeRadius/nGrid <<")\n");
    //KTINFO(vallog, "Optimized aggregated volatge = " << optimizedGridValue/nComponents<<" per channel, ideally has to be close to unity\n");

    delete channelAggregator;

    return 0;

}
