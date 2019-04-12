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
  fPhaseChFrequencySumSlot("fft", this, &KTChannelAggregator::SumChannelVoltageWithPhase, &fSummedFrequencyData),
  fSummedFrequencyData("fft", this)
  {
  }
  
  KTChannelAggregator::~KTChannelAggregator()
  {
  }
  
  bool KTChannelAggregator::Configure(const scarab::param_node* node)
  {
    if (node == NULL) return false;
    return true;
  }
  
  bool ApplyPhaseShift(double &realVal, double &imagVal, double phase)
  {
    double tempRealVal=realVal;
    double tempImagVal=imagVal;
    realVal=tempRealVal*cos(phase)-tempImagVal*sin(phase);
    imagVal=tempRealVal*sin(phase)+tempImagVal*cos(phase);
    return true;
  }
  
  double KTChannelAggregator::GetPhaseShift(double xPosition, double yPosition, double wavelength, double channelAngle)
  {
    // X position based on the angle of the channel
    double xChannel = fActiveRadius*cos(channelAngle);
    // X position based on the angle of the channel
    double yChannel = fActiveRadius*sin(channelAngle);
    // Distance of the input point from the input channel
    double pointDistance = pow(pow(xChannel-xPosition,2)+pow(yChannel-yPosition,2),0.5);
    // Phase of the input signal based on the input point, channel location and the wavelength
    return 2.0*KTMath::Pi()*pointDistance/wavelength;
  }
  
  bool KTChannelAggregator::GetGridLocation(int gridNumber, int gridSize, double &gridLocation)
  {
    if (gridNumber>=gridSize) return false;
    gridLocation=fActiveRadius*(((2.0*gridNumber+1.0)/gridSize)-1);
    return true;
  }
  
  /*
   int KTAggregatedFrequencySpectrumDataFFTW::DefineRectangularGrid(unsigned int gridSizeX, unsigned int gridSizeY)
   {
   //gridNumber goes from 0 to gridSize-1 and the gridLocation is the numerical location of the point
   //check to make sure that the the requested points are smaller than the grid size
   if(GetIsGridDefined()) return -1;
   for (unsigned int nX=0; nX<gridSizeX; nX++) {
   for (unsigned int nY=0; nY<gridSizeY; nY++) {
   double gridLocationX=fActiveRadius*(((2.0*nX+1.0)/gridSizeX)-1);
   double gridLocationY=fActiveRadius*(((2.0*nY+1.0)/gridSizeY)-1);
   fGridPoints[nX+nY*gridSizeY]=std::make_pair(gridLocationX,gridLocationY);
   }
   }
   return fGridPoints.size();
   }
   
   int KTAggregatedFrequencySpectrumDataFFTW::DefineSquareGrid(unsigned int gridSize)
   {
   return DefineRectangularGrid(gridSize,gridSize);
   }*/
  
  bool KTChannelAggregator::SumChannelVoltageWithPhase(KTFrequencySpectrumDataFFTW& fftwData)
  {
    const KTFrequencySpectrumFFTW* freqSpectrum=fftwData.GetSpectrumFFTW(0);
    // Get the number of frequency bins from the first component of fftwData
    int nFreqBins=freqSpectrum->GetNFrequencyBins();
    int nComponents=fftwData.GetNComponents(); // Get number of components
    KTFrequencySpectrumFFTW* newFreqSpectrum=new KTFrequencySpectrumFFTW(nFreqBins, freqSpectrum->GetRangeMin(), freqSpectrum->GetRangeMax());
    // Empty values in the frequency spectrum, not sure if this is needed but there were some issues when this was not done for the power spectrum
    NullFreqSpectrum(*newFreqSpectrum);
    
    std::vector<double> maxVoltages;
    std::vector<int> maxValueFrequencyBins;
    // Loop over each component and find and save the maximum summed absolute voltages and the corresponding frequencies
    for (unsigned iComponent=0; iComponent<nComponents; ++iComponent){
      // Get the frequency spectrum for that specific component
      freqSpectrum =fftwData.GetSpectrumFFTW(iComponent);
      double maxVoltage=0.0;
      int maxFrequencyBin=0;
      //Loop over the frequency bins
      for (unsigned iFreqBin=0; iFreqBin<nFreqBins; ++iFreqBin){
        double realVal=freqSpectrum->GetReal(iFreqBin)+newFreqSpectrum->GetReal(iFreqBin);
        double imagVal=freqSpectrum->GetImag(iFreqBin)+newFreqSpectrum->GetImag(iFreqBin);
        (*newFreqSpectrum)(iFreqBin)[0]=realVal;
        (*newFreqSpectrum)(iFreqBin)[1]=imagVal;
        if(freqSpectrum->GetAbs(iFreqBin)>maxVoltage){
          maxVoltage=freqSpectrum->GetAbs(iFreqBin);
          maxFrequencyBin=iFreqBin;
        }
      }// end of freqeuncy bin loops
      maxValueFrequencyBins.push_back(maxFrequencyBin);
      maxVoltages.push_back(maxVoltage);
      //      maxValues[iComponent]=maxValue;
      //(*newFreqSpectrum)+=*freqSpectrum;// This gives weird results, not sure why
    }// end of components loop
    double cumulativeAbsVoltage=0.0;
    // This can be used for the comparison with the phase-summed values and make sure that the phase-summation is done right.
    for (auto& maxVolts : maxVoltages)cumulativeAbsVoltage += maxVolts;
    
    double wavelength=0.0115; // PTS:: for 18.6 keV electrons, this somehow needs to come from the data file or config file
    double maxValue=0.0;
    double maxGridLocationX=0.0;
    double maxGridLocationY=0.0;
    
    // Assume a square grid. i.e, number of points in X= no of points in Y
    int nGridPoints=29;
    KTAggregatedFrequencySpectrumDataFFTW& newAggFreqData=fftwData.Of< KTAggregatedFrequencySpectrumDataFFTW >().SetNComponents(nGridPoints*nGridPoints);
    
    int nTotalGridPoints=0;
    // Loop over the grid points and fill the values
    for (int iGridX=0; iGridX<nGridPoints; iGridX++) {
      double gridLocationX=0;
      GetGridLocation(iGridX,nGridPoints,gridLocationX);
      for (int iGridY=0; iGridY<nGridPoints; iGridY++) {
        double gridLocationY=0;
        GetGridLocation(iGridY,nGridPoints,gridLocationY);
        // Check to make sure that the grid point is within the active detector volume, skip otherwise
        if((pow(gridLocationX,2)+pow(gridLocationY,2))>pow(fActiveRadius,2)) continue;
        newAggFreqData.SetGridPoint(nTotalGridPoints,gridLocationX,gridLocationY);
        newAggFreqData.GetGridPoint(nTotalGridPoints,gridLocationX,gridLocationY);
        nTotalGridPoints++;
      }
    }
    newAggFreqData=fftwData.Of< KTAggregatedFrequencySpectrumDataFFTW >().SetNComponents(nTotalGridPoints);
    std::cout << newAggFreqData.GetNComponents() <<std::endl;
    // Loop over all grid points and find the one that gives the highest value
    for (int iGrid=0; iGrid<nTotalGridPoints; iGrid++) { // Loop over the grid points
      double gridLocationX=0;
      double gridLocationY=0;
      newAggFreqData.GetGridPoint(iGrid,gridLocationX,gridLocationY);
      
      NullFreqSpectrum(*newFreqSpectrum);
      
      for (unsigned iComponent=0; iComponent<nComponents; ++iComponent){
        // Arbitarily assign 0 to the first channel and progresively add 2pi/N for the rest of the channels in increasing order
        double channelAngle=2*KTMath::Pi()*iComponent/nComponents;
        double phaseShift=GetPhaseShift(gridLocationX,gridLocationY,wavelength,channelAngle);
        // Get the frequency spectrum for that specific component
        freqSpectrum =fftwData.GetSpectrumFFTW(iComponent);
        double maxVoltage=0.0;
        int maxFrequencyBin=0;
        //Loop over the frequency bins
        for (unsigned iFreqBin=0; iFreqBin<nFreqBins; ++iFreqBin){
          double realVal=freqSpectrum->GetReal(iFreqBin);
          double imagVal=freqSpectrum->GetImag(iFreqBin);
          ApplyPhaseShift(realVal,imagVal,-phaseShift);
          double summedRealVal=realVal+newFreqSpectrum->GetReal(iFreqBin);
          double summedImagVal=imagVal+newFreqSpectrum->GetImag(iFreqBin);
          (*newFreqSpectrum)(iFreqBin)[0]=summedRealVal;
          (*newFreqSpectrum)(iFreqBin)[1]=summedImagVal;
        }// End of loop over freq bins
      }// End of loop over all comps
      newAggFreqData.SetSpectrum(newFreqSpectrum, iGrid);
      // Loop over all the freq bins and get the highest value and the corresponding X and Y grid locations.
    }// End of grid
    /*
     double tempMaxVoltage=0.0;
     
     NullFreqSpectrum(*newFreqSpectrum);
     // Based on the best grid location phase shift all the frequency bins and assign the phase shifted values to the new KTFrequencySpectrumDataFFTW data object
     for (unsigned iComponent=0; iComponent<nComponents; ++iComponent){
     // Arbitarily assign 0 to the first channel and progresively add 2pi/N for the rest of the channels in increasing order
     double channelAngle=2*KTMath::Pi()*iComponent/nComponents;
     double phaseShift=GetPhaseShift(maxGridLocationX,maxGridLocationY,wavelength,channelAngle);
     // Get the frequency spectrum for that specific component
     freqSpectrum =fftwData.GetSpectrumFFTW(iComponent);
     //Loop over the frequency bins
     for (unsigned iFreqBin=0; iFreqBin<nFreqBins; ++iFreqBin){
     double realVal=freqSpectrum->GetReal(iFreqBin);
     double imagVal=freqSpectrum->GetImag(iFreqBin);
     ApplyPhaseShift(realVal,imagVal,-phaseShift);
     double summedRealVal=realVal+newFreqSpectrum->GetReal(iFreqBin);
     double summedImagVal=imagVal+newFreqSpectrum->GetImag(iFreqBin);
     (*newFreqSpectrum)(iFreqBin)[0]=summedRealVal;
     (*newFreqSpectrum)(iFreqBin)[1]=summedImagVal;
     if(tempMaxVoltage<newFreqSpectrum->GetAbs(iFreqBin))tempMaxVoltage=newFreqSpectrum->GetAbs(iFreqBin);
     }
     }
     //    std::cout<< cumulativeAbsVoltage << ": " <<tempMaxVoltage<<std::endl;
     //    std::cout << maxGridLocationX << ":"<<maxGridLocationY<<std::endl;
     
     //Add the new KTFrequencySpectrumFFTW object corresponding to the channel summed frequency sepctrum as the N+1th (where N is the number of channels)  object
     newAggFreqData.SetSpectrum(newFreqSpectrum, nComponents);*/
    return true;
  }
}

