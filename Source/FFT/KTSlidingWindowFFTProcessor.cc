/*
 * KTSlidingWindowFFTProcessor.cc
 *
 *  Created on: Jan 5, 2012
 *      Author: nsoblath
 */

#include "KTSlidingWindowFFTProcessor.hh"

#include "KTFactory.hh"
#include "KTLogger.hh"
#include "KTEggHeader.hh"
#include "KTSlidingWindowFFT.hh"

using std::string;

namespace Katydid
{
    KTLOGGER(fftlog, "katydid.fft");

    KTSlidingWindowFFTProcessor::KTSlidingWindowFFTProcessor() :
            fFFT(),
            fWindowFunc(NULL),
            fFFTSignal()
    {
        RegisterSignal("fft", &fFFTSignal);

        RegisterSlot("header", this, &KTSlidingWindowFFTProcessor::ProcessHeader);
        RegisterSlot("event", this, &KTSlidingWindowFFTProcessor::ProcessEvent);
    }

    KTSlidingWindowFFTProcessor::~KTSlidingWindowFFTProcessor()
    {
    }

    Bool_t KTSlidingWindowFFTProcessor::ApplySetting(const KTSetting* setting)
    {
        if (setting->GetName() == "TransformFlag")
        {
            fFFT.SetTransformFlag(setting->GetValue< const string& >());
            return kTRUE;
        }
        if (setting->GetName() == "OverlapTime")
        {
            fFFT.SetOverlap(setting->GetValue< Double_t >());
            return kTRUE;
        }
        if (setting->GetName() == "OverlapSize")
        {
            fFFT.SetOverlap(setting->GetValue< UInt_t >());
            return kTRUE;
        }
        if (setting->GetName() == "OverlapFrac")
        {
            fFFT.SetOverlapFrac(setting->GetValue< Double_t >());
            return kTRUE;
        }
        if (setting->GetName() == "WindowFunction")
        {
            KTEventWindowFunction* tempWF = dynamic_cast< KTEventWindowFunction* >(setting->GetValue< KTWindowFunction* >());
            if (tempWF == NULL) return kFALSE;
            fWindowFunc = tempWF;
            fFFT.SetWindowFunction(tempWF);
            return kTRUE;
        }
        return kFALSE;
    }

    void KTSlidingWindowFFTProcessor::ProcessHeader(const KTEggHeader* header)
    {
        fWindowFunc->SetBinWidth(1. / header->GetAcquisitionRate());
        fFFT.RecreateFFT();
        fFFT.InitializeFFT();
        fFFT.SetFreqBinWidth(header->GetAcquisitionRate() / (Double_t)fWindowFunc->GetSize());
        return;
    }

    void KTSlidingWindowFFTProcessor::ProcessEvent(UInt_t iEvent, const KTEvent* event)
    {
        fFFT.TransformEvent(event);
        fFFTSignal(iEvent, &fFFT);
        return;
    }

} /* namespace Katydid */
