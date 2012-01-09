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

#include "boost/signals2.hpp"

#include <string>

namespace Katydid
{

    class KTSimpleFFTProcessor : public Katydid::KTProcessor
    {
        public:
            typedef boost::signals2::signal< void (UInt_t, const KTSimpleFFT*) > FFTSignal;

        public:
            KTSimpleFFTProcessor();
            virtual ~KTSimpleFFTProcessor();

            void SetTransformFlag(const std::string& transformFlag);

            void ProcessHeader(KTEgg::HeaderInfo headerInfo);

            void ProcessEvent(UInt_t iEvent, const KTEvent* event);

        private:
            KTSimpleFFT fFFT;


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

    inline void KTSimpleFFTProcessor::SetTransformFlag(const std::string& transformFlag)
    {
        fFFT.SetTransformFlag(transformFlag);
        return;
    }

    inline boost::signals2::connection KTSimpleFFTProcessor::ConnectToFFTSignal(const FFTSignal::slot_type &subscriber)
    {
        return fFFTSignal.connect(subscriber);
    }

    inline void KTSimpleFFTProcessor::SetHeaderSlotConnection(boost::signals2::connection headerConn)
    {
        fHeaderConnection = headerConn;
        return;
    }

    inline void KTSimpleFFTProcessor::SetEventSlotConnection(boost::signals2::connection eventConn)
    {
        fEventConnection = eventConn;
        return;
    }

} /* namespace Katydid */
#endif /* KTSIMPLEFFTPROCESSOR_HH_ */
