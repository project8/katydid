/*
 * KTROOTSpectrogramTypeWriterSpectrumAnalysis.cc
 *
 *  Created on: Feb 18, 2019
 *      Author: P. T. Surukuchi
 */

#include "KTROOTSpectrogramTypeWriterSpectrumAnalysis.hh"

//#include "KTFrequencySpectrumDataPolar.hh"
//#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTPowerSpectrumData.hh"
#include "KTChannelAggregatedData.hh"

using std::vector;

namespace Katydid
{
  static Nymph::KTTIRegistrar< KTROOTSpectrogramTypeWriter, KTROOTSpectrogramTypeWriterSpectrumAnalysis > sRSTWTRegistrar;
  
  KTROOTSpectrogramTypeWriterSpectrumAnalysis::KTROOTSpectrogramTypeWriterSpectrumAnalysis() :
  KTROOTSpectrogramTypeWriter(),
  //KTTypeWriterTransform()
  //            fFSPolarBundle("FSPolarSpectrogram"),
  //            fFSFFTWBundle("FSFFTWSpectrogram"),
  fPowerBundle("AggregatePowerSpectrogram"),
  fPSDBundle("AggregatePSDSpectrogram")
  {
  }
  
  KTROOTSpectrogramTypeWriterSpectrumAnalysis::~KTROOTSpectrogramTypeWriterSpectrumAnalysis()
  {
  }
  
  void KTROOTSpectrogramTypeWriterSpectrumAnalysis::OutputSpectrograms()
  {
    if (! fWriter->OpenAndVerifyFile()) return;
    
    KTDEBUG("calling output each spectrogram set")
    // OutputASpectrogramSet takes the spectrograms in the corresponding bundle and writes them to the root file.
    //        OutputASpectrogramSet(fFSPolarBundle, false);
    //        OutputASpectrogramSet(fFSFFTWBundle, false);
    OutputASpectrogramSet(fPowerBundle, false);
    OutputASpectrogramSet(fPSDBundle, false);
    
    return;
  }
  
  void KTROOTSpectrogramTypeWriterSpectrumAnalysis::ClearSpectrograms()
  {
    //        ClearASpectrogramSet(fFSPolarBundle);
    //        ClearASpectrogramSet(fFSFFTWBundle);
    ClearASpectrogramSet(fPowerBundle);
    ClearASpectrogramSet(fPSDBundle);
    return;
  }
  
  void KTROOTSpectrogramTypeWriterSpectrumAnalysis::RegisterSlots()
  {
    //        fWriter->RegisterSlot("fs-polar", this, &KTROOTSpectrogramTypeWriterSpectrumAnalysis::AddFrequencySpectrumDataPolar);
    //        fWriter->RegisterSlot("fs-fftw", this, &KTROOTSpectrogramTypeWriterSpectrumAnalysis::AddFrequencySpectrumDataFFTW);
//    fWriter->RegisterSlot("agg-ps", this, &KTROOTSpectrogramTypeWriterSpectrumAnalysis::AddAggregatePowerSpectrumData);
//    fWriter->RegisterSlot("agg-psd", this, &KTROOTSpectrogramTypeWriterSpectrumAnalysis::AddAggregatePSDSpectrumData);
    return;
  }
  
  /*
   //************************
   // Frequency Spectrum Data
   //************************
   
   void KTROOTSpectrogramTypeWriterSpectrumAnalysis::AddFrequencySpectrumDataPolar(Nymph::KTDataPtr data)
   {
   AddFrequencySpectrumDataHelper< KTFrequencySpectrumDataPolar >(data, fFSPolarBundle);
   return;
   }
   
   void KTROOTSpectrogramTypeWriterSpectrumAnalysis::AddFrequencySpectrumDataFFTW(Nymph::KTDataPtr data)
   {
   AddFrequencySpectrumDataHelper< KTFrequencySpectrumDataFFTW >(data, fFSFFTWBundle);
   return;
   }
  //********************
  // Power Spectrum Data
  //********************
  
  void KTROOTSpectrogramTypeWriterSpectrumAnalysis::AddAggregatePowerSpectrumData(Nymph::KTDataPtr data)
  {
    AddAggregatePowerSpectrumDataCoreHelper< KTChannelAggregatedData >(data, fPowerBundle);
    return;
  }
  
  void KTROOTSpectrogramTypeWriterSpectrumAnalysis::AddAggregatePSDSpectrumData(Nymph::KTDataPtr data)
  {
    AddAggregatePSDDataCoreHelper< KTChannelAggregatedData >(data, fPSDBundle);
    return;
  }
   
   */
  
} /* namespace Katydid */
