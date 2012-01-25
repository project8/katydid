/*
 * KTFFTEHuntProcessor.cc
 *
 *  Created on: Jan 24, 2012
 *      Author: nsoblath
 */

#include "KTFFTEHuntProcessor.hh"

#include "KTPhysicalArray.hh"

#include <list>
#include <map>

using std::list;
using std::multimap;

namespace Katydid
{

    KTFFTEHuntProcessor::KTFFTEHuntProcessor() :
        fSimpleFFTProc(),
        fWindowFFTProc(),
        fGainNormProc()
    {
        // link fGainNormProc to fWindowFFTProc in first position
        // link fClusterProc to fWindowFFTProc in second position
    }

    KTFFTEHuntProcessor::~KTFFTEHuntProcessor()
    {
    }

    Bool_t KTFFTEHuntProcessor::ApplySetting(const KTSetting* setting)
    {
        return kFALSE;
    }

    void KTFFTEHuntProcessor::ProcessHeader(KTEgg::HeaderInfo headerInfo)
    {
        // Initialize the processors that will be used for each event
        KTSetting settingFFTTransFlag("TransformFlag", string("ES"));
        fSimpleFFTProc.ApplySetting(&settingFFTTransFlag);

        //KTSetting settingFFTTransFlag("TransformFlag", string("ES"));
        fWindowFFTProc.ApplySetting(&settingFFTTransFlag);

        KTWindowFunction* winFunc = new KTHannWindow();
        winFunc->SetLength(1.e-5);
        KTSetting settingWinFunc("WindowFunction", winFunc);
        KTSetting settingFFTOverlap("OverlapFrac", 0.2);
        fWindowFFTProc.ApplySetting(&settingWinFunc);
        fWindowFFTProc.ApplySetting(&settingFFTOverlap);

        // Process the header information
        fSimpleFFTProc.ProcessHeader(headerInfo);
        fWindowFFTProc.ProcessHeader(headerInfo);

        return;
    }

    void KTFFTEHuntProcessor::ProcessEvent(UInt_t iEvent, const KTEvent* event)
    {
        // Perform a 1-D FFT on the entire event
        fSimpleFFTProc.ProcessEvent(iEvent, event);
        KTPhysicalArray< 1, Double_t >* fullFFT = fSimpleFFTProc.GetFFT()->CreatePowerSpectrumPhysArr();

        // Use the data from the full FFT to create a gain normalization
        fGainNormProc.PrepareNormalization(fullFFT, (UInt_t)fWindowFFTProc.GetFFT()->GetFrequencySize(), fWindowFFTProc.GetFFT()->GetFreqBinWidth());

        // Prepare to run the windowed FFT
        list< multimap< Int_t, Int_t >* > eventPeakBins;

        // Run the windowed FFT; the grouping algorithm is triggered by a single from fWindowFFTProc.
        fWindowFFTProc.ProcessEvent(iEvent, event);

        return;
    }

} /* namespace Katydid */
