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
  KTLOGGER(evlog, "KTChannelAggregator");
  
  // Register the processor
  KT_REGISTER_PROCESSOR(KTChannelAggregator, "channel-aggregator");
  
  KTChannelAggregator::KTChannelAggregator(const std::string& name) :
  KTProcessor(name),
  fChPowerSumSlot("ps", this, &KTChannelAggregator::SumChannelPower, &fSummedPowerData),
  fChPSDSumSlot("psd", this, &KTChannelAggregator::SumChannelPSD, &fSummedPSDData),
  fPhaseChPowerSumSlot("fft", this, &KTChannelAggregator::SumChannelPowerWithPhase, &fPhaseSummedPowerData),
  //  fChPSDSumSlot("fftw-psd", this, &KTChannelAggregator::SumChannelPSDWithPhase, &fPhaseSummedPSDData),
  fSummedPowerData("agg-ps", this),
  fSummedPSDData("agg-psd", this),
  fPhaseSummedPowerData("fft", this)
  //  fPhaseSummedPSDData("agg-psd-phase", this)
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
  
  bool KTChannelAggregator::SumChannelPower(KTPowerSpectrumData& chData)
  {
    unsigned nComponents = chData.GetNComponents();
    
    int arraySize=(chData.GetSpectrum(0))->size();
    KTChannelAggregatedData& newData = chData.Of< KTChannelAggregatedData >().SetNComponents(1);
    
    KTPowerSpectrum* newSpectrum = new KTPowerSpectrum(arraySize, chData.GetSpectrum(0)->GetRangeMin(), chData.GetSpectrum(0)->GetRangeMax());
    
    NullPowerSpectrum(*newSpectrum);
    //Looping through all the components
    for (unsigned iComponent=0; iComponent<nComponents; ++iComponent)
    {
      (*newSpectrum)+=(*chData.GetSpectrum(iComponent));
      newSpectrum->ConvertToPowerSpectrum();
      KTDEBUG(evlog, "Channel "<< iComponent <<" added to the aggregate");
    }
    newData.SetSpectrum(newSpectrum, 0);
    KTINFO(evlog, "Completed channel aggegation of " << nComponents << " power spectra");
    return true;
  }
  
  bool KTChannelAggregator::SumChannelPSD(KTPowerSpectrumData& chData)
  {
    unsigned nComponents = chData.GetNComponents();
    
    int arraySize=(chData.GetSpectrum(0))->size();
    KTChannelAggregatedData& newData = chData.Of< KTChannelAggregatedData >().SetNComponents(1);
    
    KTPowerSpectrum* newSpectrum = new KTPowerSpectrum(arraySize, chData.GetSpectrum(0)->GetRangeMin(), chData.GetSpectrum(0)->GetRangeMax());
    
    NullPowerSpectrum(*newSpectrum);
    //Looping through all the components
    for (unsigned iComponent=0; iComponent<nComponents; ++iComponent)
    {
      (*newSpectrum)+=(*chData.GetSpectrum(iComponent));
      newSpectrum->ConvertToPowerSpectralDensity();
      KTDEBUG(evlog, "Channel "<< iComponent <<" added to the aggregate");
    }
    newData.SetSpectrum(newSpectrum, 0);
    KTINFO(evlog, "Completed channel aggegation of " << nComponents << " power spectra");
    return true;
  }
  
  bool KTChannelAggregator::SumChannelPowerWithPhase(KTFrequencySpectrumDataFFTW& fftwData)
  {
    // Get the number of frequency bins from the first component of fftwData
    // Breaks if fftwData is empty
    int nFreqBins=(fftwData.GetSpectrumFFTW(0))->GetNFrequencyBins();
    KTFrequencySpectrumDataFFTW& newFreqData=fftwData.Of< KTFrequencySpectrumDataFFTW >().SetNComponents(1);
    // Assuming that N(Freq bins) = N(input bins)
    KTFrequencySpectrumFFTW* newFreqSpectrum=new KTFrequencySpectrumFFTW(nFreqBins, newFreqData.GetSpectrumFFTW(0)->GetRangeMin(), fftwData.GetSpectrumFFTW(0)->GetRangeMax());
//    NullFreqSpectrum(*newFreqSpectrum);
    
    // Loop over each component
    for (unsigned iComponent=0; iComponent<fftwData.GetNComponents(); ++iComponent){
      KTFrequencySpectrumFFTW* freqSpectrum =fftwData.GetSpectrumFFTW(iComponent);
      //Loop over each frequency bin
      for (unsigned iFreqBin=0; iFreqBin<freqSpectrum->GetNFrequencyBins(); ++iFreqBin){
        double testVal=newFreqSpectrum->GetReal(iFreqBin);
        double realVal=freqSpectrum->GetReal(iFreqBin)+newFreqSpectrum->GetReal(iFreqBin);
        double imagVal=freqSpectrum->GetImag(iFreqBin)+newFreqSpectrum->GetImag(iFreqBin);
        (*newFreqSpectrum)(iFreqBin)[0]=realVal;
        (*newFreqSpectrum)(iFreqBin)[1]=imagVal;
        
      if(newFreqSpectrum->GetAbs(iFreqBin)>1e-6)  std::cout<< testVal<<" : "<<newFreqSpectrum->GetReal(iFreqBin) << std::endl;
      }
    }
    
    for (unsigned iFreqBin=0; iFreqBin<newFreqSpectrum->GetNFrequencyBins(); ++iFreqBin){
      if(newFreqSpectrum->GetAbs(iFreqBin)>1e-6){
        std::cout<<iFreqBin<<"    "<<fftwData.GetSpectrumFFTW(0)->GetAbs(iFreqBin)<<"   "<<newFreqSpectrum->GetAbs(iFreqBin)<<std::endl;
      }
    }
    newFreqData.SetSpectrum(newFreqSpectrum, 0);
    return true;
  }

  bool KTChannelAggregator::SumChannelPowerWithVmagVPhase(KTFrequencySpectrumDataFFTW& fftwData)
  {
    // Get the number of frequency bins from the first component of fftwData
    // Breaks if fftwData is empty
    int nFreqBins=(fftwData.GetSpectrumFFTW(0))->GetNFrequencyBins();
    KTFrequencySpectrumDataFFTW& newFreqData=fftwData.Of< KTFrequencySpectrumDataFFTW >().SetNComponents(1);
    // Assuming that N(Freq bins) = N(input bins)
    KTFrequencySpectrumFFTW* newFreqSpectrum=new KTFrequencySpectrumFFTW(nFreqBins, newFreqData.GetSpectrumFFTW(0)->GetRangeMin(), fftwData.GetSpectrumFFTW(0)->GetRangeMax());
//    NullFreqSpectrum(*newFreqSpectrum);

    // Loop over each component
    for (unsigned iComponent=0; iComponent<fftwData.GetNComponents(); ++iComponent){
      KTFrequencySpectrumFFTW* freqSpectrum =fftwData.GetSpectrumFFTW(iComponent);
      //Loop over each frequency bin
      for (unsigned iFreqBin=0; iFreqBin<freqSpectrum->GetNFrequencyBins(); ++iFreqBin){
    	double vphase = atan(freqSpectrum->GetImag(iFreqBin)/freqSpectrum->GetReal(iFreqBin));
    	double vmag = pow(freqSpectrum->GetReal(iFreqBin)*freqSpectrum->GetReal(iFreqBin) +
    		freqSpectrum->GetImag(iFreqBin)*freqSpectrum->GetImag(iFreqBin), 0.5);
        double testVal=newFreqSpectrum->GetReal(iFreqBin);
        double realVal=vmag*cos(vphase)+newFreqSpectrum->GetReal(iFreqBin);
        double imagVal=vmag*sin(vphase)+newFreqSpectrum->GetImag(iFreqBin);
        (*newFreqSpectrum)(iFreqBin)[0]=realVal;
        (*newFreqSpectrum)(iFreqBin)[1]=imagVal;

      if(newFreqSpectrum->GetAbs(iFreqBin)>1e-6)  std::cout<< testVal<<" : "<<newFreqSpectrum->GetReal(iFreqBin) << std::endl;
      }
    }

    for (unsigned iFreqBin=0; iFreqBin<newFreqSpectrum->GetNFrequencyBins(); ++iFreqBin){
      if(newFreqSpectrum->GetAbs(iFreqBin)>1e-6){
        std::cout<<iFreqBin<<"    "<<fftwData.GetSpectrumFFTW(0)->GetAbs(iFreqBin)<<"   "<<newFreqSpectrum->GetAbs(iFreqBin)<<std::endl;
      }
    }
    newFreqData.SetSpectrum(newFreqSpectrum, 0);
    return true;
  }

} // namespace Katydid
