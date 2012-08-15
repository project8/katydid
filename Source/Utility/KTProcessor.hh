/**
 @file KTProcessor.hh
 @brief Contains KTProcessor
 @details KTProcessor is the processor base class
 @author: N. S. Oblath
 @date: Jan 5, 2012
 */

#ifndef KTPROCESSOR_HH_
#define KTPROCESSOR_HH_

// part of the deprecated settings system; will be removed
#include "KTSetting.hh"

#include "KTConnection.hh"
#include "KTSignal.hh"
#include "KTSlot.hh"

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/ptr_container/ptr_map.hpp>
#include <boost/assign/ptr_map_inserter.hpp>
#include <boost/signals2.hpp>

#include <exception>
#include <map>
#include <sstream>
#include <string>

#include "Rtypes.h"

namespace Katydid
{
    class B;
    class A
    {
        public:
            A();
            ~A();

            void UseFunc(B* bptr, int (B::*bfunc)());
    };

    class B
    {
        public:
            B();
            ~B();

            int FuncExample();
    };


    class ProcessorException : public std::logic_error
    {
        public:
            ProcessorException(std::string const& why);
    };

    class KTProcessor
    {
        protected:
            //typedef boost::ptr_map< std::string, KTSignal > SignalMap;
            typedef std::map< std::string, KTSignal* > SignalMap;
            typedef SignalMap::iterator SigMapIt;
            typedef SignalMap::value_type SigMapVal;

            //typedef boost::ptr_map< std::string, KTSlot > SlotMap;
            typedef std::map< std::string, KTSlot* > SlotMap;
            typedef SlotMap::iterator SlotMapIt;
            typedef SlotMap::value_type SlotMapVal;

            /*
        private:
            template< typename XSignature >
            class TFunctor
            {
                public:
                //virtual void operator()(const char* string)=0;  // call using operator
                virtual XSignature operator()=0;
                virtual void Call(const char* string)=0;        // call using function
            };

            template <class TClass>
            class TSpecificFunctor : public TFunctor
            {
                private:
                void (TClass::*fpt)(const char*);   // pointer to member function
                TClass* pt2Object;                  // pointer to object

                public:

                // constructor - takes pointer to an object and pointer to a member and stores
                // them in two private variables
                TSpecificFunctor(TClass* _pt2Object, void(TClass::*_fpt)(const char*))
                { pt2Object = _pt2Object;  fpt=_fpt; };

                // override operator "()"
                virtual void operator()(const char* string)
                { (*pt2Object.*fpt)(string);};              // execute member function

                // override function "Call"
                virtual void Call(const char* string)
                { (*pt2Object.*fpt)(string);};             // execute member function
            };

            template< typename XTarget, typename XSignature >
            class  Functor
            {
                public:
                    Functor(XTarget* target, XSignature* funcPtr)
                    {
                        fTarget = target;
                        fFuncPointer = funcPtr;
                    }

                private:
                    XTarget* fTarget;
                    XSignature* fFuncPointer;
            };
             */

        public:
            KTProcessor();
            virtual ~KTProcessor();

            // e.g. subscriber type: boost::bind(&KTFFTEHuntProcessor::ProcessHeader, boost::ref(procEHunt), _1)

            // e.g. XSignalSig: void (KTEgg::HeaderInfo)

            //KTConnection ConnectToSignal(const std::string& signalName, const XSlotType& subscriber)

            template< typename XSignalSig >
            KTConnection ConnectToSignal(const std::string& signalName, const typename boost::signals2::signal< XSignalSig >::slot_type& slot)
            {
                KTSignal* signal = GetSignal(signalName);
                return signal->Connect< XSignalSig >(slot);

                /*
                SigMapIt iter = fSignalMap.find(signalName);
                if (iter == fSignalMap.end())
                {
                    throw ProcessorException("Did not find signal <" + signalName + " >");
                }

                //KTSignal< XSignalSig >* sigPtr = static_cast< KTSignal< XSignalSig >* >(iter->second);
                //return sigPtr->signal.connect(subscriber);
                return iter->Connect< XSignalSig >(slot);
                */
            }

            template< class XProcessor, typename XSignature >
            void ConnectToSignal2(const std::string& signalName, XProcessor* processor, const std::string& slotName)
            {
                KTSignal* signal = GetSignal(signalName);
                KTSlot* slot = processor->GetSlot(slotName);

                try
                {
                    slot->SetConnection(signal->Connect< XSignature >(slot->GetSlot< XProcessor, XSignature >()));
                }
                catch (std::exception& e)
                {
                    std::cout << "error while setting connection" << std::endl;
                    std::cout << e.what() << std::endl;
                    throw ProcessorException("Something went wrong while connecting signal <" + signalName + "> to slot <" + slotName + ">");
                }

                return;
            }


/*
            template< typename XSignalSig >
            KTConnection ConnectToSignal(const std::string& signalName, const typename KTSignal< XSignalSig >::signal_type::slot_type& subscriber)
            {
                SigMapIt iter = fSignalMap.find(signalName);
                if (iter == fSignalMap.end())
                {
                    throw ProcessorSignalException();
                }

                KTSignal< XSignalSig >* sigPtr = static_cast< KTSignal< XSignalSig >* >(iter->second);
                return sigPtr->signal.connect(subscriber);
            }
            */

            template< typename XSignalSig >
            void RegisterSignal(std::string name, boost::signals2::signal< XSignalSig >* signalPtr)
            {
                //boost::assign::ptr_map_insert(fSignalMap)(name, signalPtr);
                KTSignal* sig = new KTSignal(signalPtr);
                fSignalMap.insert(SigMapVal(name, sig));
                return;
            }

            template< class XTargetType, typename XSlotSig >
            void RegisterSlot(std::string name, XTargetType* target, XSlotSig funcPtr, int nArgs)
            {
                //boost::assign::ptr_map_insert(fSlotMap)(name, target, funcPtr, nArgs);
                boost::function< XSlotSig > func;
                switch (nArgs)
                {
                    case 0:
                        func = boost::bind(funcPtr, target);
                        break;
                    case 1:
                        func = boost::bind(funcPtr, target, _1);
                        break;
                    case 2:
                        func = boost::bind(funcPtr, target, _1, _2);
                        break;
                    default:
                        std::stringstream errorMsg;
                        errorMsg << "Cannot create a slot with " << nArgs << " arguments.";
                        throw ProcessorException(errorMsg.str());
                }
                KTSlot* slot = new KTSlot(target, func, nArgs);
                fSlotMap.insert(SlotMapVal(name, slot));
                return;
            }

            template< class XTarget, typename XReturn >
            void RegisterSlot2(std::string name, XTarget* target, XReturn (XTarget::* funcPtr)())
            {
                //boost::assign::ptr_map_insert(fSlotMap)(name, target, funcPtr, nArgs);
                boost::function< XReturn () > *func = new boost::function< XReturn () >(boost::bind(funcPtr, target));

                KTSlot* slot = new KTSlot(target, func, 0);
                fSlotMap.insert(SlotMapVal(name, slot));
                return;
            }

            template< class XTarget, typename XReturn, typename XArg1 >
            void RegisterSlot2(std::string name, XTarget* target, XReturn (XTarget::* funcPtr)(XArg1))
            {
                //boost::assign::ptr_map_insert(fSlotMap)(name, target, funcPtr, nArgs);
                boost::function< XReturn (XArg1) > *func = new boost::function< XReturn (XArg1) >(boost::bind(funcPtr, target, _1));

                KTSlot* slot = new KTSlot(target, func, 1);
                fSlotMap.insert(SlotMapVal(name, slot));
                return;
            }

            template< class XTarget, typename XReturn, typename XArg1, typename XArg2 >
            void RegisterSlot2(std::string name, XTarget* target, XReturn (XTarget::* funcPtr)(XArg1, XArg2))
            {
                //boost::assign::ptr_map_insert(fSlotMap)(name, target, funcPtr, nArgs);
                boost::function< XReturn (XArg1, XArg2) > *func = new boost::function< XReturn (XArg1, XArg2) >(boost::bind(funcPtr, target, _1, _2));

                KTSlot* slot = new KTSlot(target, func, 2);
                fSlotMap.insert(SlotMapVal(name, slot));
                return;
            }

            KTSignal* GetSignal(const std::string& name)
            {
                SigMapIt iter = fSignalMap.find(name);
                if (iter == fSignalMap.end())
                {
                    return NULL;
                }
                return iter->second;
            }

            KTSlot* GetSlot(const std::string& name)
            {
                SlotMapIt iter = fSlotMap.find(name);
                if (iter == fSlotMap.end())
                {
                    return NULL;
                }
                return iter->second;
            }

        protected:

            SignalMap fSignalMap;

            SlotMap fSlotMap;

    };
} /* namespace Katydid */
#endif /* KTPROCESSOR_HH_ */
