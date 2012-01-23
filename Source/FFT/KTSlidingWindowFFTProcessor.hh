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

#include "boost/signals2.hpp"

#include <string>

namespace Katydid
{

    class KTSlidingWindowFFTProcessor : public KTProcessor
    {
        public:
            typedef boost::signals2::signal< void (UInt_t, const KTSlidingWindowFFT*) > FFTSignal;

        public:
            KTSlidingWindowFFTProcessor();
            virtual ~KTSlidingWindowFFTProcessor();

            Bool_t ApplySetting(const KTSetting* setting);

            void ProcessHeader(KTEgg::HeaderInfo headerInfo);

            void ProcessEvent(UInt_t iEvent, const KTEvent* event);

        private:
            KTSlidingWindowFFT fFFT;
            KTEventWindowFunction* fWindowFunc;


            //***************
            // Signals
            //***************

        public:
            boost::signals2::connection ConnectToFFTSignal(const FFTSignal::slot_type &subscriber);

        private:
            FFTSignal fFFTSignal;


            //****************
            // Slot connection
            //****************

        public:
            //void ConnectToHeaderSignalFrom(KTSignalEmitter* sigEmit);
            //void ConnectToEventSignalFrom(KTSignalEmitter* sigEmit);
            void SetHeaderSlotConnection(boost::signals2::connection headerConn);
            void SetEventSlotConnection(boost::signals2::connection eventConn);

        private:
            boost::signals2::connection fHeaderConnection;
            boost::signals2::connection fEventConnection;

    };

    inline boost::signals2::connection KTSlidingWindowFFTProcessor::ConnectToFFTSignal(const FFTSignal::slot_type &subscriber)
    {
        return fFFTSignal.connect(subscriber);
    }

    inline void KTSlidingWindowFFTProcessor::SetHeaderSlotConnection(boost::signals2::connection headerConn)
    {
        fHeaderConnection = headerConn;
        return;
    }

    inline void KTSlidingWindowFFTProcessor::SetEventSlotConnection(boost::signals2::connection eventConn)
    {
        fEventConnection = eventConn;
        return;
    }

} /* namespace Katydid */
#endif /* KTSLIDINGWINDOWFFTPROCESSOR_HH_ */
