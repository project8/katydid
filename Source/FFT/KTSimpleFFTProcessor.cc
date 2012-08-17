/*
 * KTSimpleFFTProcessor.cc
 *
 *  Created on: Jan 5, 2012
 *      Author: nsoblath
 */

#include "KTSimpleFFTProcessor.hh"

#include "KTLogger.hh"
#include "KTSimpleFFT.hh"

using std::string;

namespace Katydid
{
    KTLOGGER(fftlog, "katydid.fft");

    KTSimpleFFTProcessor::KTSimpleFFTProcessor() :
            fFFT()
    {
        RegisterSlot("header", this, &KTSimpleFFTProcessor::ProcessHeader);
        RegisterSlot("event", this, &KTSimpleFFTProcessor::ProcessEvent);
    }

    KTSimpleFFTProcessor::~KTSimpleFFTProcessor()
    {
    }

    Bool_t KTSimpleFFTProcessor::ApplySetting(const KTSetting* setting)
    {
        if (setting->GetName() == "TransformFlag")
        {
            fFFT.SetTransformFlag(setting->GetValue< const string& >());
            return kTRUE;
        }
        return kFALSE;
    }

    void KTSimpleFFTProcessor::ProcessHeader(KTEgg::HeaderInfo headerInfo)
    {
        fFFT.SetTimeSize(headerInfo.fRecordSize);
        fFFT.InitializeFFT();
        return;
    }

    void KTSimpleFFTProcessor::ProcessEvent(UInt_t iEvent, const KTEvent* event)
    {
        if (fFFT.TakeData(event))
        {
            KTINFO(fftlog, "Data transferred to simple fft; performing transform");
            fFFT.Transform();
            fFFTSignal(iEvent, &fFFT);
        }
        return;
    }

} /* namespace Katydid */
