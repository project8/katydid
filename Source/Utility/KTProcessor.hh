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
#include <boost/signals2.hpp>

#include <exception>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "Rtypes.h"

namespace Katydid
{
    class ProcessorException : public std::logic_error
    {
        public:
            ProcessorException(std::string const& why);
    };

    class KTProcessor
    {
        protected:
            typedef std::map< std::string, KTSignalWrapper* > SignalMap;
            typedef SignalMap::iterator SigMapIt;
            typedef SignalMap::value_type SigMapVal;

            typedef std::map< std::string, KTSlotWrapper* > SlotMap;
            typedef SlotMap::iterator SlotMapIt;
            typedef SlotMap::value_type SlotMapVal;

        public:
            KTProcessor();
            virtual ~KTProcessor();

            template< typename XSignalSig >
            KTConnection ConnectToSignal_old(const std::string& signalName, const typename boost::signals2::signal< XSignalSig >::slot_type& slot)
            {
                KTSignalWrapper* signal = GetSignal(signalName);
                return signal->Connect< XSignalSig >(slot);
            }

            template< class XProcessor, typename XSignature >
            void ConnectToSignal(const std::string& signalName, XProcessor* processor, const std::string& slotName)
            {
                KTSignalWrapper* signal = GetSignal(signalName);
                KTSlotWrapper* slot = processor->GetSlot(slotName);

                try
                {
                    //slot1Ptr->SetConnection(signalPtr)
                    slot->SetConnection(signal);
                    //slot->SetConnection(signal->Connect< XSignature >(slot->GetSlot< XSignature >()));
                }
                catch (std::exception& e)
                {
                    std::cout << "error while setting connection" << std::endl;
                    std::cout << e.what() << std::endl;
                    throw ProcessorException("Something went wrong while connecting signal <" + signalName + "> to slot <" + slotName + ">");
                }

                return;
            }


            //void RegisterSignal(std::string name, boost::signals2::signal< XSignalSig >* signalPtr)
            template< typename XSignalSig >
            void RegisterSignal(std::string name, XSignalSig* signalPtr)
            {
                //boost::assign::ptr_map_insert(fSignalMap)(name, signalPtr);
                KTSignalWrapper* sig = new KTSignalWrapper(signalPtr);
                fSignalMap.insert(SigMapVal(name, sig));
                return;
            }

            template< typename XSignalSig >
            void RegisterSignal2(std::string name, boost::signals2::signal< XSignalSig >* signalPtr)
            {
                //boost::assign::ptr_map_insert(fSignalMap)(name, signalPtr);
                KTSignalWrapper* sig = new KTSignalWrapper(signalPtr);
                fSignalMap.insert(SigMapVal(name, sig));
                return;
            }

/*
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
                KTSlotWrapper* slot = new KTSlotWrapper(target, func, nArgs);
                fSlotMap.insert(SlotMapVal(name, slot));
                return;
            }
*/

            template< class XTarget, typename XReturn >
            void RegisterSlot(std::string name, XTarget* target, XReturn (XTarget::* funcPtr)())
            {
                RawFuncSignatureContainer< XReturn () > funcSig;

                //boost::assign::ptr_map_insert(fSlotMap)(name, target, funcPtr, nArgs);
                boost::function< XReturn () > *func = new boost::function< XReturn () >(boost::bind(funcPtr, target));

                KTSlotWrapper* slot = new KTSlotWrapper(func, &funcSig);
                fSlotMap.insert(SlotMapVal(name, slot));
                return;
            }

            template< class XTarget, typename XReturn, typename XArg1 >
            void RegisterSlot(std::string name, XTarget* target, XReturn (XTarget::* funcPtr)(XArg1))
            {
                RawFuncSignatureContainer< XReturn (XArg1) > funcSig;

                //boost::assign::ptr_map_insert(fSlotMap)(name, target, funcPtr, nArgs);
                boost::function< XReturn (XArg1) > *func = new boost::function< XReturn (XArg1) >(boost::bind(funcPtr, target, _1));

                KTSlotWrapper* slot = new KTSlotWrapper(func, &funcSig);
                fSlotMap.insert(SlotMapVal(name, slot));
                return;
            }

            template< class XTarget, typename XReturn, typename XArg1, typename XArg2 >
            void RegisterSlot(std::string name, XTarget* target, XReturn (XTarget::* funcPtr)(XArg1, XArg2))
            {
                RawFuncSignatureContainer< XReturn (XArg1, XArg2) > funcSig;

                //boost::assign::ptr_map_insert(fSlotMap)(name, target, funcPtr, nArgs);
                boost::function< XReturn (XArg1, XArg2) > *func = new boost::function< XReturn (XArg1, XArg2) >(boost::bind(funcPtr, target, _1, _2));

                KTSlotWrapper* slot = new KTSlotWrapper(func, &funcSig);
                fSlotMap.insert(SlotMapVal(name, slot));
                return;
            }

            KTSignalWrapper* GetSignal(const std::string& name)
            {
                SigMapIt iter = fSignalMap.find(name);
                if (iter == fSignalMap.end())
                {
                    return NULL;
                }
                return iter->second;
            }

            KTSlotWrapper* GetSlot(const std::string& name)
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
