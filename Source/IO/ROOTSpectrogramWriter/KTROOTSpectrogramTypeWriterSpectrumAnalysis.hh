/*
 * KTROOTSpectrogramTypeWriterSpectrumAnalysis.hh
 *
 *  Created on: Feb 18, 2019
 *      Author: P. T. Surukuchi
 */

#ifndef KTROOTSPECTROGRAMTYPEWRITERSPECTRUMANALYSIS_HH_
#define KTROOTSPECTROGRAMTYPEWRITERSPECTRUMANALYSIS_HH_

#include "KTROOTSpectrogramWriter.hh"

namespace Katydid
{
  
  class KTROOTSpectrogramTypeWriterSpectrumAnalysis : public KTROOTSpectrogramTypeWriter
  {
  public:
    KTROOTSpectrogramTypeWriterSpectrumAnalysis();
    virtual ~KTROOTSpectrogramTypeWriterSpectrumAnalysis();
    
    void RegisterSlots();
    
  public:
    // PTS: All frequency related stuff needs implementation
    //            void AddFrequencySpectrumDataPolar(Nymph::KTDataPtr data);
    //            void OutputFrequencySpectrumDataPolar();
    //
    //            void AddFrequencySpectrumDataFFTW(Nymph::KTDataPtr data);
    //            void OutputFrequencySpectrumDataFFTW();
    // Add aggregated power spectrum histogram time slices to make a spectrogram
//    void AddAggregatePowerSpectrumData(Nymph::KTDataPtr data);
    
    // Add aggregated PSD histogram time slices to make a spectrogram
//    void AddAggregatePSDSpectrumData(Nymph::KTDataPtr data);
    
  private:
    //            DataTypeBundle fFSPolarBundle;
    //            DataTypeBundle fFSFFTWBundle;
    // DataTypeBundle is a struct with some the histograms and timing and histogram count information used by KTROOTSpectrogramWriter
    DataTypeBundle fPowerBundle;
    DataTypeBundle fPSDBundle;
    
  public:
    void OutputSpectrograms();
    void ClearSpectrograms();
    
  };
  
  
} /* namespace Katydid */
#endif /* KTROOTSPECTROGRAMTYPEWRITERSPECTRUMANALYSIS_HH_ */
