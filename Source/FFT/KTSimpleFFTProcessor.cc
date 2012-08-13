/*
 * KTSimpleFFTProcessor.cc
 *
 *  Created on: Jan 5, 2012
 *      Author: nsoblath
 */

#include "KTSimpleFFTProcessor.hh"

#include "KTEggHeader.hh"
#include "KTSimpleFFT.hh"

#include <iostream>
using std::cout;
using std::endl;

using std::string;

namespace Katydid
{

    KTSimpleFFTProcessor::KTSimpleFFTProcessor() :
            fFFT()
    {
    }

    KTSimpleFFTProcessor::~KTSimpleFFTProcessor()
    {
        fHeaderConnection.disconnect();
        fEventConnection.disconnect();
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

    void KTSimpleFFTProcessor::ProcessHeader(const KTEggHeader* header)
    {
        fFFT.SetTimeSize(header->GetRecordSize());
        fFFT.InitializeFFT();
        return;
    }

    void KTSimpleFFTProcessor::ProcessEvent(UInt_t iEvent, const KTEvent* event)
    {
        fFFT.TransformEvent(event);
        fFFTSignal(iEvent, &fFFT);
        return;
    }

} /* namespace Katydid */
