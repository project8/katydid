/**
 @file KTSimpleFFTProcessor.hh
 @brief Contains KTSimpleFFTProcessor
 @details Creates a 1-D power spectrum from an event; optionally accumulates an average spectrum
 @author: N. S. Oblath
 @date: Jan 5, 2012
 */

#ifndef KTSIMPLEFFTPROCESSOR_HH_
#define KTSIMPLEFFTPROCESSOR_HH_

#include "KTProcessor.hh"

#include "KTEgg.hh"
#include "KTSimpleFFT.hh"

#include "Rtypes.h"

#include <string>

namespace Katydid
{
    /*!
     @class KTSimpleFFTProcessor
     @author N. S. Oblath

     @brief Performs 1-dimensional FFT using KTSimpleFFT.

     @details
     Processor class for performing 1-dimensional FFTs using KTSimpleFFT.

     Slots: ProcessHeader and ProcessEvent
     Signals: void (UInt_t, const KTSimpleFFT*) emitted upon performance of a transform.
    */

    class KTSimpleFFTProcessor : public KTProcessor
    {
        public:
            typedef KTSignal< void (UInt_t, const KTSimpleFFT*) >::signal FFTSignal;

        public:
            KTSimpleFFTProcessor();
            virtual ~KTSimpleFFTProcessor();

            Bool_t ApplySetting(const KTSetting* setting);

            void ProcessHeader(KTEgg::HeaderInfo headerInfo);

            void ProcessEvent(UInt_t iEvent, const KTEvent* event);

            KTSimpleFFT* GetFFT() const;

        private:
            KTSimpleFFT fFFT;


            //***************
            // Signals
            //***************

        public:
            KTConnection ConnectToFFTSignal(const FFTSignal::slot_type &subscriber);

        private:
            FFTSignal fFFTSignal;


    };

    inline KTSimpleFFT* KTSimpleFFTProcessor::GetFFT() const
    {
        return const_cast< KTSimpleFFT* >(&fFFT);
    }

    inline boost::signals2::connection KTSimpleFFTProcessor::ConnectToFFTSignal(const FFTSignal::slot_type &subscriber)
    {
        return fFFTSignal.connect(subscriber);
    }

} /* namespace Katydid */
#endif /* KTSIMPLEFFTPROCESSOR_HH_ */
