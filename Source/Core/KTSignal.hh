/*
 * KTSignal.hh
 *
 *  Created on: Jan 15, 2013
 *      Author: nsoblath
 */

#ifndef KTSIGNAL_HH_
#define KTSIGNAL_HH_

#include <boost/signals2.hpp>

#include <string>

namespace Katydid
{

    /*!
     @class KTDataSignal
     @author N. S. Oblath

     @brief Creates a signal that is emitted when the processor creates the Data object.

     @details
     The signal is emitted by calling EmitSignal.
     If a KTDataSlot is being used, and the Slot has been given a pointer to this signal, the Slot will emit the Signal.

     Usage:
     In your Processor's header add a member variable of type KTDataSignal< ProcessorType, OutputDataType >.

     Initialize the signal with the address of the Processor.

     In your Processor's constructor, call the KTDataSignal::RegisterSlot function, passing the following as arguments:
     - slot name
     - the function signature as a string
    */
    template< class XProcessorType, class XDataType >
    class KTDataSignal
    {
        public:
            typedef void (signature)(const XDataType*);
            typedef boost::signals2::signal< signature > boost_signal;
            typedef typename boost::signals2::signal< signature >::slot_type slot_type;

        public:
            KTDataSignal(XProcessorType* proc);
            virtual ~KTDataSignal();

        public:
            void RegisterSignal(const std::string& name, const std::string& signature);

            void EmitSignal(const XDataType* data);

        private:
            XProcessorType* fProcessor;

            boost_signal fSignal;

    };

    template< class XProcessorType, class XDataType >
    KTDataSignal< XProcessorType, XDataType >::KTDataSignal(XProcessorType* proc) :
            fSignal(),
            fProcessor(proc)
    {
    }

    template< class XProcessorType, class XDataType >
    KTDataSignal< XProcessorType, XDataType >::~KTDataSignal()
    {
    }

    template< class XProcessorType, class XDataType >
    void KTDataSignal< XProcessorType, XDataType >::RegisterSignal(const std::string& name, const std::string& signature)
    {
        fProcessor->RegisterSignal(name, &fSignal, signature);
    }

    template< class XProcessorType, class XDataType >
    void KTDataSignal< XProcessorType, XDataType >::EmitSignal(const XDataType* data)
    {
        fSignal(data);
    }

} /* namespace Katydid */
#endif /* KTSIGNAL_HH_ */
