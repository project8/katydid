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
            fAggPowerBundle("AggregatePowerSpectrogram"),
            fAggPSDBundle("AggregatePSDSpectrogram")
    {
    }

    KTROOTSpectrogramTypeWriterSpectrumAnalysis::~KTROOTSpectrogramTypeWriterSpectrumAnalysis()
    {
    }

    void KTROOTSpectrogramTypeWriterSpectrumAnalysis::OutputSpectrograms()
    {
        if (!fWriter->OpenAndVerifyFile()) return;

        LDEBUG("calling output each spectrogram set")
        // OutputASpectrogramSet takes the spectrograms in the corresponding bundle and writes them to the root file.
        //        OutputASpectrogramSet(fFSPolarBundle, false);
        //        OutputASpectrogramSet(fFSFFTWBundle, false);
        OutputASpectrogramSet(fAggPowerBundle, false);
        OutputASpectrogramSet(fAggPSDBundle, false);

        return;
    }

    void KTROOTSpectrogramTypeWriterSpectrumAnalysis::ClearSpectrograms()
    {
        //        ClearASpectrogramSet(fFSPolarBundle);
        //        ClearASpectrogramSet(fFSFFTWBundle);
        ClearASpectrogramSet(fAggPowerBundle);
        ClearASpectrogramSet(fAggPSDBundle);
        return;
    }

    void KTROOTSpectrogramTypeWriterSpectrumAnalysis::RegisterSlots()
    {
        //        fWriter->RegisterSlot("fs-polar", this, &KTROOTSpectrogramTypeWriterSpectrumAnalysis::AddFrequencySpectrumDataPolar);
        //        fWriter->RegisterSlot("fs-fftw", this, &KTROOTSpectrogramTypeWriterSpectrumAnalysis::AddFrequencySpectrumDataFFTW);
        fWriter->RegisterSlot("agg-ps", this, &KTROOTSpectrogramTypeWriterSpectrumAnalysis::AddAggregatePowerSpectrumData);
        fWriter->RegisterSlot("agg-psd", this, &KTROOTSpectrogramTypeWriterSpectrumAnalysis::AddAggregatePSDSpectrumData);
        return;
    }

     //************************
     // Aggregated Frequency Spectrum Data
     //************************
/*
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
*/
    //********************
    // Aggregated Power Spectrum Data
    //********************
    void KTROOTSpectrogramTypeWriterSpectrumAnalysis::AddAggregatePowerSpectrumData(Nymph::KTDataPtr data)
    {
        AddPowerSpectrumDataCoreHelper< KTAggregatedPowerSpectrumData >(data, fAggPowerBundle);
        return;
    }

    void KTROOTSpectrogramTypeWriterSpectrumAnalysis::AddAggregatePSDSpectrumData(Nymph::KTDataPtr data)
    {
        AddPowerSpectralDensityDataCoreHelper< KTAggregatedPowerSpectrumData >(data, fAggPSDBundle);
        return;
    }

} /* namespace Katydid */
