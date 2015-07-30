/**
 @file KTProcessor.hh
 @brief Contains KTProcessor
 @details KTProcessor is the processor base class
 @author: N. S. Oblath
 @date: Jan 5, 2012
 */

#ifndef KTPROCESSOR_HH_
#define KTPROCESSOR_HH_

#include "KTConfigurable.hh"

#include "KTConnection.hh"
#include "KTLogger.hh"
#include "KTNOFactory.hh"
#include "KTSignalWrapper.hh"
#include "KTSlotWrapper.hh"

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/signals2.hpp>

#include <exception>
#include <map>
#include <sstream>
#include <string>

namespace Nymph
{
    KTLOGGER(processorlog, "KTProcessor.hh");

    class ProcessorException : public std::logic_error
    {
        public:
            ProcessorException(std::string const& why);
    };

    class KTProcessor : public KTConfigurable
    {
        protected:
            typedef std::map< std::string, KTSignalWrapper* > SignalMap;
            typedef SignalMap::iterator SigMapIt;
            typedef SignalMap::value_type SigMapVal;

            typedef std::map< std::string, KTSlotWrapper* > SlotMap;
            typedef SlotMap::iterator SlotMapIt;
            typedef SlotMap::value_type SlotMapVal;

        public:
            KTProcessor(const std::string& name="default-proc-name");
            virtual ~KTProcessor();

        public:

            void ConnectASlot(const std::string& signalName, KTProcessor* processor, const std::string& slotName, int groupNum=-1);
            void ConnectASignal(KTProcessor* processor, const std::string& signalName, const std::string& slotName, int groupNum=-1);
            void ConnectSignalToSlot(KTSignalWrapper* signal, KTSlotWrapper* slot, int groupNum=-1);

            template< class XProcessor >
            void RegisterSignal(std::string name, XProcessor* signalPtr);

            template< class XTarget, typename XReturn >
            void RegisterSlot(std::string name, XTarget* target, XReturn (XTarget::* funcPtr)());

            template< class XTarget, typename XReturn, typename XArg1 >
            void RegisterSlot(std::string name, XTarget* target, XReturn (XTarget::* funcPtr)(XArg1));

            template< class XTarget, typename XReturn, typename XArg1, typename XArg2 >
            void RegisterSlot(std::string name, XTarget* target, XReturn (XTarget::* funcPtr)(XArg1, XArg2));

            KTSignalWrapper* GetSignal(const std::string& name);

            KTSlotWrapper* GetSlot(const std::string& name);

        protected:

            SignalMap fSignalMap;

            SlotMap fSlotMap;

    };


    template< typename XSignalSig >
    void KTProcessor::RegisterSignal(std::string name, XSignalSig* signalPtr)
    {
        KTDEBUG(processorlog, "Registering signal <" << name << "> in processor <" << fConfigName << ">");
        KTSignalWrapper* sig = new KTSignalWrapper(signalPtr);
        fSignalMap.insert(SigMapVal(name, sig));
        return;
    }

    template< class XTarget, typename XReturn >
    void KTProcessor::RegisterSlot(std::string name, XTarget* target, XReturn (XTarget::* funcPtr)())
    {
        KTDEBUG(processorlog, "Registering slot <" << name << "> in processor <" << fConfigName << ">");

        KTSignalConcept< XReturn () > signalConcept;

        boost::function< XReturn () > *func = new boost::function< XReturn () >(boost::bind(funcPtr, target));

        KTSlotWrapper* slot = new KTSlotWrapper(func, &signalConcept);
        fSlotMap.insert(SlotMapVal(name, slot));
        return;
    }

    template< class XTarget, typename XReturn, typename XArg1 >
    void KTProcessor::RegisterSlot(std::string name, XTarget* target, XReturn (XTarget::* funcPtr)(XArg1))
    {
        KTDEBUG(processorlog, "Registering slot <" << name << "> in processor <" << fConfigName << ">");

        KTSignalConcept< XReturn (XArg1) > signalConcept;

        boost::function< XReturn (XArg1) > *func = new boost::function< XReturn (XArg1) >(boost::bind(funcPtr, target, _1));

        KTSlotWrapper* slot = new KTSlotWrapper(func, &signalConcept);
        fSlotMap.insert(SlotMapVal(name, slot));
        return;
    }

    template< class XTarget, typename XReturn, typename XArg1, typename XArg2 >
    void KTProcessor::RegisterSlot(std::string name, XTarget* target, XReturn (XTarget::* funcPtr)(XArg1, XArg2))
    {
        KTDEBUG(processorlog, "Registering slot <" << name << "> in processor <" << fConfigName << ">");

        KTSignalConcept< XReturn (XArg1, XArg2) > signalConcept;

        boost::function< XReturn (XArg1, XArg2) > *func = new boost::function< XReturn (XArg1, XArg2) >(boost::bind(funcPtr, target, _1, _2));

        KTSlotWrapper* slot = new KTSlotWrapper(func, &signalConcept);
        fSlotMap.insert(SlotMapVal(name, slot));
        return;
    }


#define KT_REGISTER_PROCESSOR(proc_class, proc_name) \
        static ::Nymph::KTNORegistrar< ::Nymph::KTProcessor, proc_class > sProc##proc_class##Registrar( proc_name );

} /* namespace Nymph */
#endif /* KTPROCESSOR_HH_ */
