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
    }

    KTSimpleFFTProcessor::~KTSimpleFFTProcessor()
    {
        fHeaderConnection.disconnect();
        fEventConnection.disconnect();
    }

    Bool_t KTSimpleFFTProcessor::Configure(const KTPStoreNode* node)
    {
        fFFT.SetTransformFlag(node->GetData<string>("transform_flag", ""));
        return true;
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
