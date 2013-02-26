/*
 * KTSignal.hh
 *
 *  Created on: Jan 15, 2013
 *      Author: nsoblath
 */

#ifndef KTSIGNAL_HH_
#define KTSIGNAL_HH_

#include "KTProcessor.hh"

#include <boost/shared_ptr.hpp>
#include <boost/signals2.hpp>

#include <string>

namespace Katydid
{
    class KTData;

    /*!
     @class KTSignalOneArg
     @author N. S. Oblath

     @brief Creates a signal that takes a single argument.

     @details
     The signal is emitted by calling operator().
     If a KTDataSlot is being used, and the Slot has been given a pointer to this signal, the Slot will emit the Signal.

     Usage:
     In your Processor's header add a member variable of type KTDataSignal< ArgumentType >.

     Initialize the signal with the processor's 'this' pointer and the name of the signal.

     To use the signal, call it as: fSignalObject(arg);
    */
    template< class XSignalArgument >
    class KTSignalOneArg
    {
        public:
            typedef void (signature)(XSignalArgument);
            typedef boost::signals2::signal< signature > boost_signal;
            typedef typename boost::signals2::signal< signature >::slot_type slot_type;

        public:
            KTSignalOneArg(const std::string& name, KTProcessor* proc);
            virtual ~KTSignalOneArg();

        protected:
            KTSignalOneArg();
            KTSignalOneArg(const KTSignalOneArg&);

        public:
            void operator()(XSignalArgument arg);

        protected:
            boost_signal fSignal;

    };


    template<>
    class KTSignalOneArg< void >
    {
        public:
            typedef void (signature)(void);
            typedef boost::signals2::signal< signature > boost_signal;
            typedef typename boost::signals2::signal< signature >::slot_type slot_type;

        public:
            KTSignalOneArg(const std::string& name, KTProcessor* proc);
            virtual ~KTSignalOneArg();

        protected:
            KTSignalOneArg();
            KTSignalOneArg(const KTSignalOneArg&);

        public:
            void operator()();

        protected:
            boost_signal fSignal;

    };



    /*!
     @class KTSignalData
     @author N. S. Oblath

     @brief Creates a signal that takes a boost::shared_ptr< KTData > object as its argument.

     @details
     The purpose of the signal is for passing KTData pointers between Processors.
     The signal is emitted by calling operator().
     If a KTDataSlot is being used, and the Slot has been given a pointer to this signal, the Slot will emit the Signal.

     Usage:
     In your Processor's header add a member variable of type KTSignalData.

     Initialize the signal with the processor's 'this' pointer and the name of the signal.

     That's it!
    */

    class KTSignalData : public KTSignalOneArg< boost::shared_ptr< KTData > >
    {
        public:
            KTSignalData(const std::string& name, KTProcessor* proc);
            virtual ~KTSignalData();

        protected:
            KTSignalData();
            KTSignalData(const KTSignalData&);
    };



    template< class XSignalArgument >
    KTSignalOneArg< XSignalArgument >::KTSignalOneArg(const std::string& name, KTProcessor* proc) :
            fSignal()
    {
        proc->RegisterSignal(name, &fSignal, "");
    }

    template< class XSignalArgument >
    KTSignalOneArg< XSignalArgument >::KTSignalOneArg() :
            fSignal()
    {}

    template< class XSignalArgument >
    KTSignalOneArg< XSignalArgument >::KTSignalOneArg(const KTSignalOneArg& rhs) :
            fSignal()
    {}

    template< class XSignalArgument >
    KTSignalOneArg< XSignalArgument >::~KTSignalOneArg()
    {
    }

    template< class XSignalArgument >
    inline void KTSignalOneArg< XSignalArgument >::operator()(XSignalArgument arg)
    {
        fSignal(arg);
    }



    inline void KTSignalOneArg< void >::operator()()
    {
        fSignal();
    }



} /* namespace Katydid */
#endif /* KTSIGNAL_HH_ */
