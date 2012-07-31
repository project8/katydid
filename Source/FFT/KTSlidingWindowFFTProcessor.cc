/*
 * KTSlidingWindowFFTProcessor.cc
 *
 *  Created on: Jan 5, 2012
 *      Author: nsoblath
 */

#include "KTSlidingWindowFFTProcessor.hh"

#include "KTFactory.hh"
#include "KTLogger.hh"
#include "KTSlidingWindowFFT.hh"

using std::string;

namespace Katydid
{
    KTLOGGER(fftlog, "katydid.fft");

    KTSlidingWindowFFTProcessor::KTSlidingWindowFFTProcessor() :
            fFFT(),
            fWindowFunc(NULL),
            fFFTSignal(),
            fHeaderConnection(),
            fEventConnection()
    {
    }

    KTSlidingWindowFFTProcessor::~KTSlidingWindowFFTProcessor()
    {
        fHeaderConnection.disconnect();
        fEventConnection.disconnect();
    }

    Bool_t KTSlidingWindowFFTProcessor::Configure(const KTPStoreNode* node)
    {
        fFFT.SetTransformFlag(node->GetData< string >("transform_flag", ""));
        fFFT.SetOverlap(node->GetData< Double_t >("overlap_time", 0));
        fFFT.SetOverlap(node->GetData< UInt_t >("overlap_size", 0));
        fFFT.SetOverlapFrac(node->GetData< Double_t >("overlap_frac", 0.));

        string windowType = node->GetData< string >("window_function", "rectangular");
        KTEventWindowFunction* tempWF = KTFactory< KTEventWindowFunction >::GetInstance()->Create(windowType);
        if (tempWF == NULL)
        {
            KTERROR(fftlog, "Invalid window function type given: <" << windowType << ">.");
            return false;
        }
        fWindowFunc = tempWF;
        fFFT.SetWindowFunction(tempWF);

        return true;
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
            KTINFO(fftlog, "Data transferred to sliding window fft; performing transform");
            fFFT.Transform();
            fFFTSignal(iEvent, &fFFT);
        }
        return;
    }

} /* namespace Katydid */
