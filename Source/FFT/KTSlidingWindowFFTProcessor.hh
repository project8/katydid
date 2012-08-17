/**
 @file KTSlidingWindowFFTProcessor.hh
 @brief Contains KTSlidingWindowFFTProcessor
 @details Creates a 2-D (frequency vs. time) power spectrum from an event
 @author: N. S. Oblath
 @date: Jan 17, 2012
 */

#ifndef KTSLIDINGWINDOWFFTPROCESSOR_HH_
#define KTSLIDINGWINDOWFFTPROCESSOR_HH_

#include "KTProcessor.hh"

#include "KTEgg.hh"
#include "KTSlidingWindowFFT.hh"
#include "KTEventWindowFunction.hh"

#include "Rtypes.h"

#include <string>

namespace Katydid
{
    class KTEggHeader;

    class KTSlidingWindowFFTProcessor : public KTProcessor
    {
        public:
            typedef KTSignal< void (UInt_t, const KTSlidingWindowFFT*) >::signal FFTSignal;

        public:
            KTSlidingWindowFFTProcessor();
            virtual ~KTSlidingWindowFFTProcessor();

            Bool_t ApplySetting(const KTSetting* setting);

            void ProcessHeader(const KTEggHeader* header);

            void ProcessEvent(UInt_t iEvent, const KTEvent* event);

            KTSlidingWindowFFT* GetFFT() const;
            KTEventWindowFunction* GetWindowFunc() const;

        private:
            KTSlidingWindowFFT fFFT;
            KTEventWindowFunction* fWindowFunc;


            //***************
            // Signals
            //***************

        private:
            FFTSignal fFFTSignal;


    };

    inline KTSlidingWindowFFT* KTSlidingWindowFFTProcessor::GetFFT() const
    {
        return const_cast< KTSlidingWindowFFT* >(&fFFT);
    }

    inline KTEventWindowFunction* KTSlidingWindowFFTProcessor::GetWindowFunc() const
    {
        return fWindowFunc;
    }

} /* namespace Katydid */
#endif /* KTSLIDINGWINDOWFFTPROCESSOR_HH_ */
