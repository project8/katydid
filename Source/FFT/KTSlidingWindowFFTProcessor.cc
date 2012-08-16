/*
 * KTSlidingWindowFFTProcessor.cc
 *
 *  Created on: Jan 5, 2012
 *      Author: nsoblath
 */

#include "KTSlidingWindowFFTProcessor.hh"

#include "KTSlidingWindowFFT.hh"

#include <iostream>
using std::cout;
using std::endl;

using std::string;

namespace Katydid
{

    KTSlidingWindowFFTProcessor::KTSlidingWindowFFTProcessor() :
            fFFT(),
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

    void KTSlidingWindowFFTProcessor::ProcessHeader(KTEgg::HeaderInfo headerInfo)
    {
        fWindowFunc->SetBinWidth(1. / headerInfo.fSampleRate);
        fFFT.RecreateFFT();
        fFFT.InitializeFFT();
        fFFT.SetFreqBinWidth(headerInfo.fSampleRate / (Double_t)fWindowFunc->GetSize());
        return;
    }

    void KTSlidingWindowFFTProcessor::ProcessEvent(UInt_t iEvent, const KTEvent* event)
    {
        if (fFFT.TakeData(event))
        {
            cout << "Data transferred to sliding window fft; performing transform" << endl;
            fFFT.Transform();
            fFFTSignal(iEvent, &fFFT);
        }
        return;
    }

} /* namespace Katydid */
