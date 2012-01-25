/**
 @file KTFFTEHuntProcessor.hh
 @brief Contains KTFFTEHuntProcessor
 @details Performs the FFT-based electron hunt
 @author: N. S. Oblath
 @date: Jan 24, 2012
 */

#ifndef KTFFTEHUNTPROCESSOR_HH_
#define KTFFTEHUNTPROCESSOR_HH_

#include "KTProcessor.hh"

#include "KTSimpleFFTProcessor.hh"
#include "KTSlidingWindowFFTProcessor.hh"
#include "KTGainNormalizationProcessor.hh"

namespace Katydid
{
    /*!
     @class KTFFTEHuntProcessor
     @author N. S. Oblath

     @brief Performs an FFT-based electron hunt.

     @details
     Uses a windows FFT of Egg events to search for clusters of high-peaked bins moving up in frequency.
    */

    class KTFFTEHuntProcessor : public KTProcessor
    {
        public:
            KTFFTEHuntProcessor();
            virtual ~KTFFTEHuntProcessor();

            Bool_t ApplySetting(const KTSetting* setting);

            void ProcessHeader(KTEgg::HeaderInfo headerInfo);

            void ProcessEvent(UInt_t iEvent, const KTEvent* event);

        private:
            KTSimpleFFTProcessor fSimpleFFTProc;
            KTSlidingWindowFFTProcessor fWindowFFTProc;
            KTGainNormalizationProcessor fGainNormProc;

    };

} /* namespace Katydid */
#endif /* KTFFTEHUNTPROCESSOR_HH_ */
